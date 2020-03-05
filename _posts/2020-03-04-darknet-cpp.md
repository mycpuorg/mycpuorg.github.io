---
layout: post
title: "Debugging Multi-Thread C++ Applications with BPF"
excerpt: "Using BCC to debug in C++"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM]
comments: true
---
In [earlier post](http://www.mycpu.org/bpf-c++-off-cpu/), I had written about
attempting to debug C++ applications and find the stack traces that are blocked
off-CPU. However, if you followed towards the end the of the post you can
clearly see that whatever I had tried hadn't worked. I concluded saying there
was going to be a follow up post. So here it is...

## Writing BPF Programs
In an [earlier post](http://www.mycpu.org/writing-bpf-c++/), I had disucussed
with an example on writing useful programs in C++ that allows us to gather
important low level system information. We are going to continue with the trend
here and try to write a simple BPF program compiled with `BCC`.

### Why Is Off-CPU Stats?
A majority of the systems latencies that are not directly from programming or
cannot be attributed to inefficient language semantics typically arise from
system latency. Furthermore, a large portion of such wait times are caused by
waiting for some events. If this wait is deliberate such as waiting for some
event, then Kernel puts the task in an *interruptible wait* if the wait is not
voluntary i.e., I/O wait then the task enters an *uninterruptible wait*. In such
a state, the CPU cannot move on to doing other useful tasks but it has to wait
for the action to complete. This results in higher / variable latency for the
system since the wait times are not under the control of the waiting party.

### What Is Off-CPU Analysis?
For a detailed analysis of off-CPU analysis, read [Brendan Gregg's
article](http://www.brendangregg.com/offcpuanalysis.html)
In this entry, we will simply setup the initial ``BPF`` program to be able to
collect data from the system for both on and off CPU stats.

### BPF Code
```cpp
const std::string BPF_PROGRAM = R"(
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>

struct key_t {
    u32 pid;
    u32 tgid;
    int user_stack_id;
    int kernel_stack_id;
    char name[TASK_COMM_LEN];
};
BPF_HASH(counts, struct key_t, u64);
BPF_HASH(start, u32, u64);
BPF_STACK_TRACE(stack_traces, 16384);

int my_on_cpu(struct pt_regs *ctx, struct task_struct *prev) {
    u32 pid = prev->pid;
    u32 tgid = prev->tgid;
    u64 ts, *tsp;

    // record previous thread sleep time
    ts = bpf_ktime_get_ns();
    start.update(&pid, &ts);

    // get the current thread's start time
    pid = bpf_get_current_pid_tgid();
    tgid = bpf_get_current_pid_tgid() >> 32;
    tsp = start.lookup(&pid);
    // missed start or filtered
    if (tsp == 0) {
        return 0;
    }

    // calculate current thread's delta time
    u64 delta = bpf_ktime_get_ns() - *tsp;
    start.delete(&pid);
    delta = delta / 1000;

    // create map key
    struct key_t key = {};

    key.pid = pid;
    key.tgid = tgid;
    key.user_stack_id = stack_traces.get_stackid(ctx, BPF_F_USER_STACK);
    key.kernel_stack_id = stack_traces.get_stackid(ctx, 0);
    bpf_get_current_comm(&key.name, sizeof(key.name));

    counts.update(&key, &delta);

    return 0;
})";
```

### What are KProbes??
According to the [Linux Kernel
Documentation](https://www.kernel.org/doc/html/latest/trace/kprobetrace.html),
event tracing with ``krpobes`` uses the same infrastructure that hooks into
kprobes. Here's a blurb from the website that introduces `kprobes`:

```plain
These events are similar to tracepoint based events. Instead of Tracepoint, this is based on kprobes (kprobe and kretprobe). So it can probe wherever kprobes can probe (this means, all functions except those with __kprobes/nokprobe_inline annotation and those marked NOKPROBE_SYMBOL). Unlike the Tracepoint based event, this can be added and removed dynamically, on the fly.
```

### Important Pieces of Code
The above section described some common used structure and captured a couple of
important timestamps.

#### Compute the Delta Time
We are attaching our `kprobe` to the function in scheduler code that is called
to finish switching the current task into the next one.

```cpp
    // record previous thread sleep time
    ts = bpf_ktime_get_ns();
    start.update(&pid, &ts);

    // get the current thread's start time
    pid = bpf_get_current_pid_tgid();
    tgid = bpf_get_current_pid_tgid() >> 32;
    tsp = start.lookup(&pid);
    // missed start or filtered
    if (tsp == 0) {
        return 0;
    }

    // calculate current thread's delta time
    u64 delta = bpf_ktime_get_ns() - *tsp;
    start.delete(&pid);
    delta = delta / 1000;

```

#### Map for BCC Program
Now we store a map whose key contains the ``PID``, ``TGID``. We also save the
user stack ID and kernel stack ID. This combo is very crucial in getting a stack
trace that can tell us if a thread is waiting OFF-CPU.

```cpp
    // create map key
    struct key_t key = {};

    key.pid = pid;
    key.tgid = tgid;
    key.user_stack_id = stack_traces.get_stackid(ctx, BPF_F_USER_STACK);
    key.kernel_stack_id = stack_traces.get_stackid(ctx, 0);
    bpf_get_current_comm(&key.name, sizeof(key.name));

    counts.update(&key, &delta);
```

#### Post-Processing Data
Here you can see how the data obtained from these multiple ``BPF_HASH`` tables.
```cpp
    for (auto it : counts_tbl_offline) {
    std::cout << "TGID: " << it.first.tgid << " (" << it.first.name << ") "
          << "made " << it.second
          << " was off CPU on the following stack: " << std::endl;
    if (it.first.kernel_stack_id >= 0) {
        std::cout << "  Kernel Stack:" << std::endl;
        auto syms = stack_stuff.get_stack_symbol(it.first.kernel_stack_id, -1);
        for (auto sym : syms)
        std::cout << "    " << sym << std::endl;
    } else {
        // -EFAULT normally means the stack is not availiable and not an error
        if (it.first.kernel_stack_id != -EFAULT) {
        lost_stacks++;
        std::cout << "    [Lost Kernel Stack" << it.first.kernel_stack_id << "]"
              << std::endl;
        }
    }
    if (it.first.user_stack_id >= 0) {
        std::cout << "  User Stack:" << std::endl;
        auto syms = stack_stuff.get_stack_symbol(it.first.user_stack_id, it.first.pid);
        for (auto sym : syms)
        std::cout << "    " << sym << std::endl;
    } else {
        if (it.first.user_stack_id != -EFAULT) {
            lost_stacks++;
            std::cout << "    [Lost User Stack " << it.first.user_stack_id << "]"
                << std::endl;
        }
        }
    }
```

### Demo
I compiled the program with BCC engine and produced a ELF binary output.
```bash
sudo ./MethodOffCPU
```

The following stacks are a tiny snippet of what I obtained.
```bash
TGID: 10 (rcu_sched) made 7998 was off CPU on the following stack:
  Kernel Stack:
    finish_task_switch
    schedule
    schedule_timeout
    rcu_gp_kthread
    kthread
    ret_from_fork
TGID: 27947 (python3) made 6 was off CPU on the following stack:
  Kernel Stack:
    finish_task_switch
    schedule
    exit_to_usermode_loop
    prepare_exit_to_usermode
    swapgs_restore_regs_and_return_to_usermode
  User Stack:
    [UNKNOWN]
    [UNKNOWN]
TGID: 17976 (gnome-terminal-) made 59 was off CPU on the following stack:
  Kernel Stack:
    finish_task_switch
    schedule
    schedule_hrtimeout_range_clock
    schedule_hrtimeout_range
    poll_schedule_timeout.constprop.11
    do_sys_poll
    __x64_sys_poll
    do_syscall_64
    entry_SYSCALL_64_after_hwframe
  User Stack:
    poll
	.....
	.....
```

## Conclusion
We have overcome all the obstacles for getting off CPU stack trace and the time
spent on the glass. I think this is a highly useful tool. BPF allows you to
generate such low level information without much experience.
