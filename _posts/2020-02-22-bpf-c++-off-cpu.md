---
layout: post
title: "Debugging C++ Applications with BPF"
excerpt: "Using BCC to debug in C++"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM]
comments: true
---
In an [earlier post](http://www.mycpu.org/writing-bpf-c++/) I showed how to
write BPF programs in C++ to obtain low level system information like branch
prediction hit rate. I'd like to explore how to build a useful repository of
useful functions that can be readily used to debug and understand the system
behaviour in high level application.

I [whined](http://www.mycpu.org/back-to-bcc/) earlier, that the obstacle to
*quickly* finding information on the lower level systems is higher than while
programming in the kernel. Well it's a different thing that most device drivers
wouldn't care about scheduling stats and things of that sort [^1].

So I started writing about how to build a basic sort of tracer that can be used
to measure how long a method spent on CPU and how long it spent outside of it.

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

### BPF Program
The heart of the solution is to write a ``BPF`` program that can capture all the
information we need. For this, I started with code in
[BCC](https://github.com/iovisor/bcc/) project and modified on top.
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
BPF_HASH(counts, struct key_t);
BPF_HASH(start, u32);
BPF_STACK_TRACE(stack_traces, 10240);

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
    if (tsp == 0) {
        return 0;        // missed start or filtered
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

    counts.increment(key, delta);
    return 0;
})";
```
### Minimal Objective
For this exercise I want to make sure that for each call to attach the probe we
must be able to store the time delta.

```cpp
    auto attach_res = bpf.attach_kprobe("finish_task_switch", "my_on_cpu");
```
This keeps returning an empty map each time. So I tried to use tracepoints,
since ``scheduler`` has the ``sched:sched_switch`` tracepoint that is of
interest.

The tracepoint usage will look something like this:
```cpp
    auto attach_res = bpf.attach_tracepoint("sched:sched_switch", "my_on_cpu");
```

### Conclusion
The usage of BPF looks promising but it is ridden with obstacles that need to be
passed first to be able to use it fruitfully. A follow-up post looks imminent.


[^1]: Could be argued that device drivers should not have to know about these things if they are doing their job well.
