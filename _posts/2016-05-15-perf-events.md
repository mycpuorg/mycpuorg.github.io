layout: post
title: "perf: A quick look"
excerpt: "Introduction of perf usage"
tags: [Linux, kernel, perf, monitor, tracing, ftrace, tracer, func_graph, call graphs ]
comments: true

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

## Ouput of perf top<a id="orgheadline6"></a>

**perf top**
   PerfTop:    2852 irqs/sec  kernel:74.9%  exact:  0.0% [4000Hz cycles],  (all, 4 CPUs)

---

14.81%  [kernel]       [k] arch<sub>cpu</sub><sub>idle</sub>                     
 3.76%  [unknown]      [.] 0x76e22324                        
 1.90%  [kernel]       [k] \_<sub>memzero</sub>                         
 1.27%  [unknown]      [.] 0x76e21960                        
 1.19%  [kernel]       [k] finish<sub>task</sub><sub>switch</sub>                
 1.16%  [kernel]       [k] \_raw<sub>spin</sub><sub>unlock</sub><sub>irqrestore</sub>       
 1.01%  [unknown]      [.] 0x76e22320                        
 0.98%  [kernel]       [k] rcu<sub>idle</sub><sub>exit</sub>                     
 0.92%  [unknown]      [.] 0x76e21958                        
 0.86%  [kernel]       [k] copy<sub>page</sub>                         
 0.72%  [unknown]      [.] 0x76e21954                        
 0.71%  [kernel]       [k] filemap<sub>map</sub><sub>pages</sub>                 
 0.67%  [kernel]       [k] get<sub>page</sub><sub>from</sub><sub>freelist</sub>            
 0.57%  [kernel]       [k] do<sub>page</sub><sub>fault</sub>                     
 0.51%  [kernel]       [k] handle<sub>mm</sub><sub>fault</sub>                   
 0.49%  [kernel]       [k] unmap<sub>single</sub><sub>vma</sub>                  
 0.37%  [unknown]      [.] 0x76ded280                        
 0.35%  [kernel]       [k] \_<sub>wake</sub><sub>up</sub><sub>bit</sub>                     
 0.34%  [kernel]       [k] free<sub>hot</sub><sub>cold</sub><sub>page</sub>                
 0.34%  [kernel]       [k] memcpy                            
 0.31%  [kernel]       [k] \_<sub>usb</sub><sub>hcd</sub><sub>giveback</sub><sub>urb</sub>            
 0.31%  [kernel]       [k] vector<sub>swi</sub>                        
 0.30%  [kernel]       [k] \_<sub>sync</sub><sub>icache</sub><sub>dcache</sub>              
 0.28%  [kernel]       [k] \_<sub>do</sub><sub>softirq</sub>                      
&#x2026;.