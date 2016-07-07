---
layout: post
title: "Ftrace: Function Graph Overview"
excerpt: "Introduction to Function Graph"
tags: [Linux, kernel, tracing, ftrace, tracer, func_graph, call graphs ]
comments: true
---

## What is ftrace <a id="orgheadline1"></a>

-   The generic tracing system in kernel: It is a kernel-only tracer tool that is designed to help developers and designers to understand what's going on inside the kernel. It is widely used for analyzing systems to understand latencies and performance issues.
-   Although it's utility extends beyond that such as event tracing in the kernel. Throughout the kernel there are hundreds of static events available that can be enabled via the debugfs to generate runtime traces of the system. The debugfs filesystem contains control files that can be used to configure parameters of ftrace, as well as the read-only files that can be used to display output on the running system.
-   The default directory where debugfs is mounted in the kernel is under *sys/kernel/debug*, for any reason if your system should not have 'debug' directory mounted under this then you can explicitly mount this by

    sudo mount -t debugfs none /sys/kernel/debug

All the nodes that are required for ftrace, be it, collecting traces or configuring a parameter of ftrace all of them can be found under

    /sys/kernel/debug/tracing

-   Provides various tracers and events: Let us visit some of the key files under debugfs (*sys/kernel/debug/tracing*):

**`current_tracer` - set or display the current tracer**

    root@raspberrypi:/sys/kernel/debug/tracing# cat current_tracer 
    nop

**`available_tracers` - all the tracers supported in this system**

    root@raspberrypi:/sys/kernel/debug/tracing# cat available_tracers 
    blk function_graph wakeup_dl wakeup_rt wakeup irqsoff function nop

**`tracing_on` - switch to toggle tracing on and off**

    root@raspberrypi:/sys/kernel/debug/tracing# echo 1 > tracing_on

**`trace` - node continaing output trace in human readable format**
**`trace_pipe` - same as trace, but meant for live streaming of trace**

-   Uses special data structure to store trace data in memory

## ftrace - Overview of events<a id="orgheadline2"></a>

-   An event in the kernel that needs to be traced in the kernel.

    Ex: ext4_readpage

-   A trace point is a special condition within a function
    
        void foo(u32 *p)
        {
               my_foo_trace();
               ...
               if (*p == 0xdeadbeef)
                       my_cond_trace();
               ...
        }
    
    `my_foo_trace` can be used as an event defined to simply trace the function entry into foo().
    `my_cond_trace` is a suitable event to trace a condition (event) in kernel when a certain specific condition has occured as noted in the above example where contents of address pointed to by `p` is equal to `0xdeadbeef`
    And there can be several conditions in the kernel where the **event** `my_cond_trace` could occur and each of these tracepoints would trace it and write to the trace buffer which is collected in the final trace output.
-   Hundreds of events supported in the kernel by default
    -   4.x kernels support over 600 events

## Types of events<a id="orgheadline5"></a>

Ftrace supports static and dynamic events

### Deliberate/Explicit<a id="orgheadline3"></a>

-   Kernel is sprinkled with trace points for important events
-   Most kernel versions support a majority of static trace events

### Surprise/Implicit<a id="orgheadline4"></a>

-   ftrace has the ability to trace any function in the kernel
    because 'mcount'
-   allows for tracing a subset of functions via filtering too

## mcount - the trampoline<a id="orgheadline6"></a>

-   mcount is a low overhead routine
-   mcount is called by every kernel function
    -   noop if `function_graph` is not supported
-   added by default during entry into function
-   at exit the trampoline may not be available on all SoCs

## Practice: function graph in action<a id="orgheadline7"></a>

    # cd /sys/kernel/debug/tracing
    # echo function_graph > current_tracer
    # echo 1 > tracing_on
    # cat trace
    ...
     3)   0.046 us    |                      _cond_resched();
     1)               |  SyS_close() {
     1)               |    __close_fd() {
    ...
     1)               |      filp_close() {
    ...
     1)               |        fput() {
    ...
     3)   0.051 us    |                    up_write();
     1)   3.168 us    |      }
     1)   4.256 us    |    }
     3)               |                    __put_anon_vma() {
     1)   4.853 us    |  }

## Dealing with output verbiage<a id="orgheadline8"></a>

To reduce the verbose output and configure to show only the
interesting output we can use the `max_graph_depth` to avoid nesting in
the output trace

    # cd /sys/kernel/debug/tracing
    # echo function_graph > current_tracer
    # echo 1 > max_graph_depth
    # echo 1 > tracing_on
    # cat trace
     3) + 81.718 us   |  } /* schedule_preempt_disabled */
     3)   5.468 us    |  tick_nohz_idle_enter();
     2)   0.969 us    |  SyS_dup2();
     2)   1.620 us    |  do_page_fault();
     2)   0.572 us    |  SyS_close();
     3)   0.431 us    |  arch_cpu_idle_enter();
     3)   0.116 us    |  tick_check_broadcast_expired();
     3)   0.632 us    |  rcu_idle_enter();

## Conditional trace: Important filters<a id="orgheadline9"></a>

-   ftrace filtering:
    
        # echo ext4:ext4_readpage > set_ftrace_filter
-   user space conditional:
    -   intended to be used by the application to open this file early on.

    # echo "my_trace_string" > trace_marker

-   `tracing_max_latency` Some of the tracers record their max latency. If a new max trace is recorded (latency greater than the value stored in this file) then it will be stored as the new max latency.

## Summary<a id="orgheadline10"></a>

To summarize, we have learned what ftrace is and how to collect traces for function and function graph. In a future post, we will look at some examples and how they can be used to trace a specific function.