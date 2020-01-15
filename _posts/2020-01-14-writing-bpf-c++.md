---
layout: post
title: "So... You wanna measure Branch Prediction Hit Rate with BPF and C++?"
excerpt: "A walk-through of my understanding of the BCC"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM]
comments: true
---
```
Disclaimer: [Brendan Gregg](http://www.brendangregg.com) is a leading expert on
this topic. This post is an attempt to try and simplify things for a mortal like
myself. If you want authoritative content on anything BCC, BPF, or tracing
please visit Brendan's site.
```
### Why is BCC?
Writing BPF Programs can be hard, so we needed a toolkit that makes writing them
easier, at least, to a certain extent.

### What is BCC?
[BCC](https://github.com/iovisor/bcc) is a toolkit for creating frontend
programs that can efficiently perform Kernel and User Level tracing. The toolkit
comes with several useful tools and examples. This toolkit makes writing BPF
programs easier (and includes a C wrapper around LLVM), and front-ends in Python
and lua.

#### But ... Perf?
With ``perf`` you can [Debug
Applications](http://www.mycpu.org/flamegraphs-on-c++/) or [capture events for
the fun of it](http://www.mycpu.org/perf-events/). However, it is sampling
profiler which collects events periodically and "estimates" the system
performance and statistics based on these samples and Hardware Based Performance
Monitoring Counters. This can add non-trivial amounts of overhead, and we are
not even talking about compute spent post-processing the captured samples.

### Pre-requisites
For you to walk through the examples, you will need to [install BCC
tools](https://github.com/iovisor/bcc/blob/master/INSTALL.md). To find out the
list of Hardware Events supported in your machine you could run ``perf list``
and you will see something like:
<details><summary>CLICK ME</summary>
<p>

#### yes, even hidden code blocks!

```python
print("hello world!")
```

</p>
</details>
You should see the support for below two events:
```
  branch-instructions OR branches                    [Hardware event]
  branch-misses                                      [Hardware event]
```

I am running my Linux Kernel and [you can too, if you have an
hour](http://www.mycpu.org/kernel-n00b-howto/).
```zsh
╭─ ~/s/bcc/bcc/build   master ● ? ⍟1                                                                                      ✔  26.99G RAM  0.20 L
╰─ cat /proc/version 
Linux version 5.5.0-rc4+ (manoj@manoj-desktop) (gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1)) #1 SMP Fri Jan 3 00:01:03 PST 2020
```


### Show Me The Code!
A BCC program tends to have a common structure which is mostly along the lines
of:
+ Define the BPF Program which is going to be run by eBPF in the Kernel
+ Attach the Perf Event or Probe in the Kernel
+ Run whatever code you wanted to profile
+ Detach the Perf Event or Probe
+ Process the Result

The Program we are going to walk through is similar to the ``LLCStat`` which is
present in the BCC Project but does not seem to run on any hardware that I tried
on. However, I thought it would still be interesting enough to profile the
Branch Prediction (or MisPrediction) Rate on all the cores and all PIDs running
in a specified period of time.

#### Define BPF Program
```c
const std::string BPF_PROGRAM = R"(
#include <linux/ptrace.h>
#include <uapi/linux/bpf_perf_event.h>

struct event_t {
    int cpu;
    int pid;
    char name[16];
};

BPF_HASH(ref_count, struct event_t);
BPF_HASH(miss_count, struct event_t);

static inline __attribute__((always_inline)) void get_key(struct event_t* key) {
    key->cpu = bpf_get_smp_processor_id();
    key->pid = bpf_get_current_pid_tgid();
    bpf_get_current_comm(&(key->name), sizeof(key->name));
}

int on_branch_miss(struct bpf_perf_event_data *ctx) {
    struct event_t key = {};
    get_key(&key);

    u64 zero = 0, *val;
    val = miss_count.lookup_or_try_init(&key, &zero);
    if (val) {
        (*val) += ctx->sample_period;
    }

    return 0;
}

int on_branch_ref(struct bpf_perf_event_data *ctx) {
    struct event_t key = {};
    get_key(&key);

    u64 zero = 0, *val;
    val = ref_count.lookup_or_try_init(&key, &zero);
    if (val) {
        (*val) += ctx->sample_period;
    }

    return 0;
}
)";
```
The gist of the above code is that we define two data structures - both BPF Hash
Tables. They are Hash Tables in the Kernel, however, it is not important to know
exactly how they are used. We just need to know that they basically function
like Hash Tables. Their Key is of type ``struct event_t`` and they store a value
of ``uint64_t`` type. In addition, there are a couple of functions defined which
seem to look up some sort of ``key`` which is of type ``struct event_t``. You
have probably connected the dots here :)

But who's calling these functions? And why are they enclosed in double quotes
and stored like ``String`` objects?

#### Init and Attach Events:
```cpp
     ebpf::BPF bpf;
     auto init_res = bpf.init(BPF_PROGRAM);
	 // ...

    auto attach_ref_res =
	  bpf.attach_perf_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
				"on_branch_ref", 100, 0);

	// ...
    
    auto attach_miss_res = bpf.attach_perf_event(PERF_TYPE_HARDWARE,
		PERF_COUNT_HW_BRANCH_MISSES, "on_branch_miss", 100, 0);
	// ...

```
``BPF`` is the Class defined by the BCC toolkit which provides a lot of
utilities for completing BPF related tasks effectively and efficiently. This is
the biggest value of BCC framework in my opinion. We initialize this object
``bpf`` with the BPF Program which we wrote above, this code is going to be run
in the Kernel's BPF JIT VM and it is injected by the ``BPF`` object. __"How
exactly?"__ you ask, through the ``attach_perf_event()`` API. If you notice, the
string literals ``on_branch_ref`` and ``on_branch_miss`` passed as an arg to
this API are the same as the two functions within the BPF Program. So, in
effect, we are attaching two perf events and passing the callbacks as the
functions defined within the BPF program (again, which runs in the Kernel on
each event. What are these *events* that we talk about? They are
``PERF_COUNT_HW_BRANCH_INSTRUCTIONS`` and ``PERF_COUNT_HW_BRANCH_MISSES`` both
are definied in the ``linux/perf_events.h`` header file included in this
program. Note that we are specifying that these two events are of the type
``PERF_TYPE_HARDWARE`` so the Kernel knows to fetch the values for these
requests from the PMC Registers just like it does when ``perf`` utlitiy requests
for these numbers. So we registered our handlers in the BPF program for the
events we are interested in.

#### Run the code to profile
In this case, we are simply capturing all the branch instructions and branch
misses throughout a specified duration (note the ``sleep()`` for an amount of
time.

#### Detach our handlers
After the specified amout of time is finished, we detach our handlers by
calling:
```cpp
     bpf.detach_perf_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
     bpf.detach_perf_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES);
```

#### Process the output
If our program has reached until this point then most hurdles associated with
running BPF programs have been crossed. We only need to make sure the output is
meaningful or even there. In this example, we don't automate the processing of
this output, instead we simply pretty-print the stats on stdout.

```cpp
     auto instrns = bpf.get_hash_table<event_t, uint64_t>("ref_count");
     auto misses = bpf.get_hash_table<event_t, uint64_t>("miss_count");
     for (auto it : instrns.get_table_offline()) {
	  uint64_t hit;
	  try {
	       auto miss = misses[it.first];
	       hit = miss <= it.second ? it.second - miss : 0;
	  } catch (...) {
	       hit = it.second;
	  }
	  double ratio = (double(hit) / double(it.second)) * 100.0;
	  
	  // ...
	  
	  // pretty-print the results
```

In the above code, we are retrieving the hash tables via BPF's
``get_hash_table()`` API. Note that the arg passed to this API contains the
string literal which were the names of the hash tables in our BPF program which
was attached earlier. Now we iterate through the hash tables and calculate the
Branch Hit Ratio and Branch Miss Ratio from the given counters.

The complete code can be downloaded from [here](/images/BranchPrediction.cc)

The output from the above program on my machine as-is is:
```bash
╭─ ~/s/bcc/bcc/build   master ● ? ⍟1                                                                                      ✔  26.89G RAM  0.26 L 
╰─ sudo ./examples/cpp/BranchPrediction 1
Probing for 1 seconds
PID    27835 (tmux: server)     on CPU  4 Hit Rate 58.82% (12000/20400)
PID        0 (swapper/6)        on CPU  6 Hit Rate 96.15% (274600/285600)
PID     4935 (tmux: server)     on CPU  4 Hit Rate 0% (0/20400)
PID     4935 (tmux: server)     on CPU 15 Hit Rate 0% (0/20400)
PID    27840 (python3)          on CPU  2 Hit Rate 0.4902% (400/81600)
PID        0 (swapper/2)        on CPU  2 Hit Rate 95.93% (450100/469200)
PID    27980 (kworker/4:5)      on CPU  4 Hit Rate 88.73% (18100/20400)
PID        0 (swapper/15)       on CPU 15 Hit Rate 96.28% (432100/448800)
PID     4711 (gnome-terminal-)  on CPU 15 Hit Rate 2.451% (500/20400)
PID        0 (swapper/13)       on CPU 13 Hit Rate 95.68% (527000/550800)
PID    20674 (firefox)          on CPU  8 Hit Rate 83.82% (17100/20400)
PID    20856 (WebExtensions)    on CPU  4 Hit Rate 55.39% (11300/20400)
PID        0 (swapper/4)        on CPU  4 Hit Rate 95.48% (448000/469200)
PID    27836 (byobu-status)     on CPU  6 Hit Rate 0% (0/20400)
PID    27850 (mv)               on CPU  0 Hit Rate 5.392% (1100/20400)
PID        0 (swapper/8)        on CPU  8 Hit Rate 89.12% (90900/102000)
PID    20680 (Gecko_IOThread)   on CPU  1 Hit Rate 14.71% (3000/20400)
PID    28632 (kworker/5:1)      on CPU  5 Hit Rate 88.97% (36300/40800)
PID    14423 (Web Content)      on CPU  8 Hit Rate 0% (0/20400)
PID        0 (swapper/3)        on CPU  3 Hit Rate 89.57% (127900/142800)
PID    27817 (BranchPredictio)  on CPU 12 Hit Rate 94.12% (38400/40800)
PID        0 (swapper/14)       on CPU 14 Hit Rate 91.93% (806400/877200)
PID        0 (swapper/1)        on CPU  1 Hit Rate 93.34% (457000/489600)
PID        0 (swapper/7)        on CPU  7 Hit Rate 95.59% (468000/489600)
PID    27836 (byobu-status)     on CPU  1 Hit Rate 0% (0/20400)

#...

```

Happy Whatever!

#### Source:
+ http://www.brendangregg.com
+ https://github.com/iovisor/bcc
+ http://www.mycpu.org/kernel-n00b-howto/
+ http://www.mycpu.org/flamegraphs-on-c++/
+ http://www.mycpu.org/perf-events/
