---
layout: post
title: "Better Concurrency"
excerpt: "C++ Seasoning Talk"
tags: [Linux, Code, C++, rotate, STL, algorithms]
comments: false
---
In an [earlier post](http://www.mycpu.org/thats-a-rotate/), I pointed to Sean
Parent's talk called ``C++ Seasoning`` and how it is useful in writing better
programs. After posting multiple articles about
[threading](http://www.mycpu.org/c++-threads-async-deferred/),
[concurrency](http://www.mycpu.org/task-based-programming/), [parallel
programming](http://www.mycpu.org/c++-promise-and-future/) and other rich
sources of strange issues I thought I would watch a talk given by an actual
expert on the subject for a change.

## No Raw Sync Primitives
Basically, Sean discourages you from reinventing the wheel, or a bicycle, or
a motorcycle. In this talk, he helps you [achieve greater speed](https://sean-parent.stlab.cc/presentations/2017-01-18-concurrency/2017-01-18-concurrency.pdf)
He discourages the use of raw synchronization primitives such as mutex,
semaphore, fence etc.

### Why Not?
```
"Because you won't get it right"
"Amen"
```
<iframe width="560" height="315" src="https://www.youtube.com/embed/zULU6Hhp42w" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Traditional Synch Primitives:
![synch primitives](/images/no_raw_synch_prims.png)

Single Queue Example - Don't try to speed the queue up, try to get off it


![single queue](/images/barely_moves_needle.png)


Try To Minimize the time spent in non parallelizable parts.


## Futures in C++
Although it does take away some perf, it provides nice things like provide
exception aggregation. What they lack:

![futures lack](/images/c++_futures_lack.png)

## Conclusion:
Another informative talk by Sean that can hopefully improve our concurrency
coding chops.
