---
layout: post
title: "Branch Prediction - Fundamentals"
excerpt: "A walk-through of Branch Prediction"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM]
comments: true
---
In an [earlier post](http://www.mycpu.org/writing-bpf-c++/), we discussed about
the capability to measure the branch misprediction rates using BPF. In that
post, the focus was on obtaining a low-level CPU architecture specific counter
easily from a BPF program written in C++ frontend. But we did not dive deeper
into what Branch Prediction is or why it is done at all.

## CPU Pipeline Instructions
Why are CPU instructions pipelined? 
```
"For better throughput, obviously"
```
This warrants a very long discussion about why this is the case. Instead, here's a
thousand words:
![No Pipelines](/images/no_pipeline.png)
![RISC Pipeline](/images/RISC_pipeline.png)

Notice that the entire design assumed that the instructions are flowing
constantly. In straight line code, the pipeline reads the next instruction
either in program order or compiler decided order to take advantage of
Instruction Level Parallelism (ILP). In case of branches, the pipeline does not
know before hand which is the next instruction to execute in the current running
program. Example:

```cpp
auto flag = get_output_some_fn();

if (flag) {
    do_something();
} else {
    do_another_thing();
}
```

In the above case, there is no way to guess which way the execution flow is
going to go. The predicate ``flag`` depends on the outcome of another function
whose execution could in turn be decided by some external unforeseeable
factors.

In such cases, the pipeline always stalls until the correct target instruction
to execute next is determined. This results in a loss of a single cycle at the
time of instruction fetch.
![No Prediction](/images/no_prediction_timing.png)

## Can we avoid the Pipeline Stalls?
Yes, we can by prefetching the next instruction and executing it while hoping
our decision to prefetch was correct. This is called ``Speculative Execution``
of Branches.
![Timing With Prediction](/images/with_prediction_timing.png)


## Dynamic Branch Prediction
The aim of a Dynamic Branch Prediction is to predict whether a conditional
branch will be taken based on the behavior of the program in current
execution. One of the simpler ways to do this is to:
+ maintain state for each conditional branch
+ predict whether branch will be taken based on state
+ change the state based on the whether the branch was taken or not

| Current State | Input (Branch Exec) | Output (Prediction) | Next State |
|:-------------:|:-------------------:|:-------------------:|:----------:|
| 0             | Not Taken           | Not Taken           | 0          |
| 0             | Taken               | Not Taken           | 1          |
| 1             | Not Taken           | Taken               | 0          |
| 1             | Taken               | Taken               | 1          |

## Branch Prediction Buffer
The important question from previous section is - storing and updating next
state for every branch instruction can become expensive. Using a separate Branch
Prediction Buffer to cache the predicted sstates of recently executed
conditional branches can reduce this cost. Branch History Table is
accessed/indexed using the address of the conditional branch instructions.

## Conclusion
In simple CPU Pipeline designs, pipeline stalls can significantly impact
instruction throughput. Implementing even simple branch prediction mechanisms
can improve the performance substantially. The results are most pronounced in
speculative execution strategies.

### Sources:
+ [CS Dept. at UCLA](http://web.cs.ucla.edu/~tamir/courses/)
+ [UW CSE378](https://courses.cs.washington.edu/courses/cse378/09wi/lectures/lec13.pdf)
