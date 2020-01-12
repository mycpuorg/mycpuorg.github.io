---
layout: post
title: "Performance Analysis For C++ Applications"
excerpt: "C++ Profiling with Perf"
tags: [Linux, Code, C++, Perf, Profiling]
comments: true
---
In the [previous post](http://www.mycpu.org/c++-tuning-tools-explore/), I
mentioned that I am exploring a framework that can be reliably used to profile
C++ code. I also mentioned that system tools have a much higher likelihood of
posessing all the qualities that you would expect from a good tool for such a
job, and, surprise! I think we have a winner. It's the good old ``perf`` which
comes to the rescue again.

### Intent of the exercise
I wanted to quickly understand the time spent in my application and see how for
deep I can go with profiling such a system. So, I wrote a small function which
simply pushes elements to a vector. But before we go too deep into the weeds, we
would like to see some system level statistics on critical events in the system
that indicate the application's performance.

#### Sample Application
Let's start with some dummy piece of code, the focus is simplicity.

```cpp
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

void foo()
{
    std::vector<int> my_vec;
    for (int i = 0; i < 100; i++)
		my_vec.push_back(i - (rand() % 42));
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < 1000000; i++)
		foo();
    return 0;
}

```

### Compile
The above application is compiled with the following flags.
```bash
clang++-7 -fno-rtti -O3 -std=c++17  -fno-omit-frame-pointer -fno-exceptions -pthreads -o bench ./vector.cpp
```

### Perf Statistics: Measure First ...
Here are the perf statistics from running the above code.

#### Exotic options for ``perf stat``
``perf`` supports a slew of Hardware and Software Events that it can
profile. You can list them all by ``perf list``. By default, ``stat`` option
lists stats for Software Events such as ``context-switches``, ``cpu-migrations``
etc. and Hardware Events such as ``branches``.

To print all the events listed under ``perf list`` you could use the ``-d`` option
```bash
       -d, --detailed
           print more detailed statistics, can be specified up to 3 times

                     -d:          detailed events, L1 and LLC data cache
                  -d -d:     more detailed events, dTLB and iTLB events
               -d -d -d:     very detailed events, adding prefetch events
```

You can also repeat the command that is being used to generate stats for.
```bash
	-r, --repeat=<n>
           repeat command and print average + stddev (max: 100). 0 means forever.

```

Here is the complete output generated with the following command.
```bash
$ perf stat -d -d -d -r 5 ./bench

 Performance counter stats for './bench' (5 runs):

       1365.631381      task-clock (msec)         #    0.991 CPUs utilized            ( +-  0.39% )
                 2      context-switches          #    0.002 K/sec                    ( +- 10.21% )
                 0      cpu-migrations            #    0.000 K/sec                  
               118      page-faults               #    0.086 K/sec                    ( +-  0.60% )
     3,254,406,172      cycles                    #    2.383 GHz                      ( +-  1.23% )  (40.00%)
        16,719,210      stalled-cycles-frontend   #    0.51% frontend cycles idle     ( +-  2.46% )  (40.07%)
         9,619,922      stalled-cycles-backend    #    0.30% backend cycles idle      ( +-  8.05% )  (40.19%)
     7,019,806,957      instructions              #    2.16  insn per cycle         
                                                  #    0.00  stalled cycles per insn  ( +-  1.18% )  (40.36%)
     1,638,194,323      branches                  # 1199.587 M/sec                    ( +-  1.35% )  (40.54%)
        17,047,391      branch-misses             #    1.04% of all branches          ( +-  1.03% )  (40.76%)
     3,136,855,834      L1-dcache-loads           # 2297.000 M/sec                    ( +-  0.87% )  (40.04%)
           123,738      L1-dcache-load-misses     #    0.00% of all L1-dcache hits    ( +-  0.63% )  (40.08%)
   <not supported>      LLC-loads                                                   
   <not supported>      LLC-load-misses                                             
        15,360,647      L1-icache-loads           #   11.248 M/sec                    ( +- 17.92% )  (40.04%)
           135,312      L1-icache-load-misses                                         ( +-  1.33% )  (39.99%)
            17,324      dTLB-loads                #    0.013 M/sec                    ( +-  1.64% )  (39.89%)
             2,957      dTLB-load-misses          #   17.07% of all dTLB cache hits   ( +-  9.26% )  (39.77%)
                50      iTLB-loads                #    0.036 K/sec                    ( +- 27.03% )  (39.60%)
                26      iTLB-load-misses          #   52.21% of all iTLB cache hits   ( +- 53.17% )  (39.42%)
            52,568      L1-dcache-prefetches      #    0.038 M/sec                    ( +-  5.25% )  (39.23%)
   <not supported>      L1-dcache-prefetch-misses                                   

       1.377694970 seconds time elapsed                                          ( +-  0.85% )


```
Note that the on 
```model name      : AMD EPYC 7571```
the LLC statistic iss not generated. If the hardware supports it then there is a
way to extract the PMU event with the right hexadecimal event descriptor
passed.

### Flamegraphs for C++:
Flamegraphs are a good option to visually identify bottlenecks within the
system. There might be another tracing tool which provides a similar visual cue
for doing performance analysis on your system. Since we are going to be dealing
with ``perf`` in this article, let's stick with
[flamegraphs](http://www.brendangregg.com/flamegraphs.html) here.

It is interactive, you could click on a tile to zoom in and reset the zoom.

<div
style="float:right;padding-left:30px;padding-right:10px;padding-bottom:3px"><a
href="/images/vec_fg.svg"><img
src="/images/vec_fg-crop-500.png" width="300" height="354"
style="padding-bottom:3px"/></a><br><center><i>Flame Graphs for Vector Ops</i></center></div>

I will leave this here to whet your appetite for more. If you are not familiar
with any of this, do not worry. It is not as important to know each and every
detail as it is to know that a tool exists that can easily generate all this
information when the need arises.

Coming up next is understanding the FlameGraphs really well and correlating with
statistics.

Happy whatever!

Source:
+ Chandler Carruth's excellent 90min talk: https://youtu.be/nXaxk27zwlk
+ Brendan Gregg's Blog: www.brendangregg.com
+ http://www.mycpu.org/perf-events/
