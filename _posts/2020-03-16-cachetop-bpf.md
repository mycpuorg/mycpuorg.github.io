---
layout: post
title: "Writing Linux File Buffer Cache top in C++ with BPF - Part I"
excerpt: "Use BPF to Write Linux File Buffer Util"
tags: [Linux, Code, Kernel, BPF, C++]
comments: true
---
[In](http://www.mycpu.org/writing-bpf-c++/)
[earlier](http://www.mycpu.org/bpf-c++-off-cpu/)
[posts](http://www.mycpu.org/debugging-c++-with-bpf/), we have seen examples of
writing utilites in C++ using BCC to obtain various system statistics. In this
post, we will write the ``cachetop`` which is one of the ``BCC`` tools
implemented in Python. If all you care about is obtaining these statistics on
demand via console/terminal then re-implementing these tools in C++ adds little
value. The advantages of implementing the existing tools in C++ is
manifold. Since C++ is more often used in building larger systems that can
benefit from having easy ready implementations that can provide low level system
statistics. 

### Using Kprobes
In the above mentioned posts, I have written about using ``kprobes`` to add
hooks in the Kernel to query interesting info from the running kernel.

### Important Methods in Kernel
The methods that give us all the info we need in terms of Linux File Buffer
Cache are the following:
``add_to_page_cache_lru``
``mark_page_accessed``
``account_page_dirtied``
``mark_buffer_dirty``

To start with we will try to setup enough infrastructure to keep track of the
number of times these functions were called for each process.

### Show Me The Code
```cpp
    std::vector<std::string> kernel_funcs_probed = {
        "add_to_page_cache_lru",
        "mark_page_accessed",
        "account_page_dirtied",
        "mark_buffer_dirty"
    };

    auto init_res = bpf.init(BPF_PROGRAM);
    if (init_res.code() != 0) {
        std::cerr << init_res.msg() << std::endl;
        return -1;
    }

    for (auto kf : kernel_funcs_probed) {

        auto attach_res = bpf.attach_kprobe(kf, "do_count");

        if (attach_res.code() != 0) {
            std::cerr << "failed to attach" << std::endl;
            std::cerr << attach_res.msg() << std::endl;
            return -1;
        }

        std::cerr << "attached successfully" << std::endl;
    }
```

```
"It looks like we are asking BPF to simply keep a count each time these
functions are called in the kernel"
```
Exactly.

#### And the BPF Code?
```cpp
const std::string BPF_PROGRAM = R"(
    #include <uapi/linux/ptrace.h>
    struct key_t {
        u64 ip;
        u32 pid;
        u32 uid;
        char comm[16];
    };

    BPF_HASH(counts, struct key_t);

    int do_count(struct pt_regs *ctx) {
        struct key_t key = {};
        u64 pid = bpf_get_current_pid_tgid();
        u32 uid = bpf_get_current_uid_gid();

        key.ip = PT_REGS_IP(ctx);
        key.pid = pid & 0xFFFFFFFF;
        key.uid = uid & 0xFFFFFFFF;
        bpf_get_current_comm(&(key.comm), 16);

        counts.increment(key);
        return 0;
})";
```
This should be a sufficient start for now, in subsequent posts we will come back
and try to make sense of all this and obtain that cache stat of ours.
