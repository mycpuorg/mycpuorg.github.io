---
layout: post
title: "perf: A quick look"
excerpt: "Introduction of perf usage"
tags: [Linux, kernel, perf, monitor, tracing, ftrace, tracer, func_graph, call graphs ]
comments: true
---

# Introduction<a id="orgheadline6"></a>

## perf - What is perf?<a id="orgheadline1"></a>

-   The generic profiler tool for linux
-   Provides a rich set of tools that abstracts away CPU hardware differences
-   Is available in all modern linux systems

## perf - allows you to trace, collect and analyze data<a id="orgheadline2"></a>

-   perf is available in linux-tools-common package
-   perf supports a rich set of commands. Just type 'perf'..

    annotate        Read perf.data (created by perf record) and display annotated code
    list            List all symbolic event types
    record          Run a command and record its profile into perf.data
    report          Read perf.data (created by perf record) and display the profile
    sched           Tool to trace/measure scheduler properties (latencies)
    script          Read perf.data (created by perf record) and display trace output
    stat            Run a command and gather performance counter statistics
    timechart       Tool to visualize total system behavior during a workload
    top             System profiling tool.

## perf - Sample usage<a id="orgheadline3"></a>

-   In it's simplest form:

    # perf record -a -g -F 99 --

-   This records the activity in the system by:
    -   -F sampling at a sampling frequency of 99Hz
    -   -a profiling all cores
    -   -g generating call graph info
-   Now for generating human readable output from the collected traces

    # perf report

-   This displays the performance counter profile info recorded
    earlier

## perf report<a id="orgheadline4"></a>

    +  27.12%           chrome  chrome                      [.] 0x00000000029633cb                                                                                            
    +   5.18%           chrome  [kernel.kallsyms]           [k] trace_graph_entry                                                                                             
    +   2.67%           chrome  perf-21360.map              [.] 0x000018b2828b9a40                                                                                            
    +   2.54%     byobu-status  [kernel.kallsyms]           [k] trace_graph_entry                                                                                             
    +   2.38%          swapper  [kernel.kallsyms]           [k] intel_idle                                                                                                    
    +   1.86%          swapper  [kernel.kallsyms]           [k] trace_graph_entry                                                                                             
    +   1.45%             Xorg  [kernel.kallsyms]           [k] trace_graph_entry                                                                                             
    +   1.42%        vpnagentd  [kernel.kallsyms]           [k] trace_graph_entry                                                                                             
    +   1.38%           chrome  [kernel.kallsyms]           [k] prepare_ftrace_return                                                                                         
    +   1.38%               sh  [kernel.kallsyms]           [k] trace_graph_entry

## Output of perf report -n &#x2013;stdio<a id="orgheadline5"></a>

**perf report -n &#x2013;stdio**

    # Samples: 556  of event 'cycles'
    # Event count (approx.): 6711822878
    #
    # Overhead   Samples      Command      Shared Object                                                                                                     
    # ........  ............  ..........  ..................
    #
        27.12%   115   chrome  chrome [.] 0x00000000029633cb                                                                             
                  |
                  |
    ...
        5.18%     22   chrome  [kernel.kallsyms] [k] trace_graph_entry                                                                              
                  |
                  --- trace_graph_entry
                             prepare_ftrace_return
                             ftrace_graph_caller
                            |
                            |
    ...
        2.54%     12     byobu-status  [kernel.kallsyms] [k] trace_graph_entry                                                                              
                   |
                   --- trace_graph_entry
                      |
                      |