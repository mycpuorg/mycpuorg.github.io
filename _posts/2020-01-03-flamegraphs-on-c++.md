---
layout: post
title: "Generate Statistics For C++ Applications"
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
deep I can go with profiling such a system. So I wrote a small function which
simply pushes elements to a vector. But before we go too deep into the weeds, we
would like to see some system level statistics on critical events in the system
that indicate the application's performance.

#### Sample Application
(Yes, this is a dummy piece of code with no real utility)

```cpp
void foo()
{
    std::vector<int> my_vec;

    my_vec.push_back(5);
    my_vec.push_back(6);
    my_vec.push_back(7);
    my_vec.push_back(8);
    my_vec.push_back(9);
    my_vec.push_back(0);
    my_vec.push_back(1);
    my_vec.push_back(1);
    my_vec.push_back(4);
    my_vec.push_back(5);

    for (int i = 0; i < 100; i++)
        my_vec.push_back(i - 4);

}

int main(int argc, char *argv[])
{
    for (int i = 0; i < 1000000; i++)
        foo();

    return 0;
}

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

        254.533527      task-clock (msec)         #    0.958 CPUs utilized            ( +-  0.87% )
                 1      context-switches          #    0.005 K/sec                    ( +- 16.67% )
                 0      cpu-migrations            #    0.000 K/sec                  
               118      page-faults               #    0.465 K/sec                    ( +-  0.43% )
       635,108,146      cycles                    #    2.495 GHz                      ( +-  5.06% )  (40.76%)
         3,170,884      stalled-cycles-frontend   #    0.50% frontend cycles idle     ( +- 16.73% )  (42.14%)
        22,419,357      stalled-cycles-backend    #    3.53% backend cycles idle      ( +- 14.04% )  (43.64%)
     1,659,880,996      instructions              #    2.61  insn per cycle         
                                                  #    0.01  stalled cycles per insn  ( +-  4.03% )  (44.76%)
       491,582,686      branches                  # 1931.308 M/sec                    ( +-  3.95% )  (45.35%)
         1,987,532      branch-misses             #    0.40% of all branches          ( +- 13.18% )  (45.71%)
       614,620,488      L1-dcache-loads           # 2414.694 M/sec                    ( +-  4.77% )  (41.14%)
            37,428      L1-dcache-load-misses     #    0.01% of all L1-dcache hits    ( +-  4.48% )  (39.76%)
   <not supported>      LLC-loads                                                   
   <not supported>      LLC-load-misses                                             
         9,669,527      L1-icache-loads           #   37.989 M/sec                    ( +- 58.22% )  (38.26%)
            30,287      L1-icache-load-misses                                         ( +-  5.43% )  (37.13%)
             2,853      dTLB-loads                #    0.011 M/sec                    ( +-  4.12% )  (36.54%)
               565      dTLB-load-misses          #   19.81% of all dTLB cache hits   ( +-  5.73% )  (36.18%)
                 0      iTLB-loads                #    0.000 K/sec                    (36.20%)
                 0      iTLB-load-misses          #    0.00% of all iTLB cache hits   (36.21%)
             6,326      L1-dcache-prefetches      #    0.025 M/sec                    ( +-  4.60% )  (36.20%)
   <not supported>      L1-dcache-prefetch-misses                                   

       0.265622339 seconds time elapsed                                          ( +-  3.40% )


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
src="/images/vec_fg-crop-500.png" width="250" height="254"
style="padding-bottom:3px"/></a><br><center><font size=-1><i>Flame Graphs for Vector Ops</i></font></center></div>

I will leave this here to whet your appetite for more. If you are not familiar
with any of this, do not worry. It is not as important to know each and every
detail as it is to know that a tool exists that can easily generate all this
information when the need arises.

Coming up next is understanding the FlameGraphs really well and correlating with
statistics.

Happy whatever!
