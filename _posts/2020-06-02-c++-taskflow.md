---
layout: post
title: "Taskflow - C++ Parallel Tasking System"
excerpt: "Parallel Task Library"
tags: [Linux, Code, C++, Promise, Future, STL, algorithms, tasks, concurrency]
comments: true
---
In [earlier](http://www.mycpu.org/c++-concurrency-productivity/)
[posts](http://www.mycpu.org/c++transwarp/), I mentioned about that I was
looking for a library implemented in Modern C++ that allowed me to compose
parallel tasks and help with scheduling them efficiently.

Finally, I was able to play with one such project which looks promising called
C++ [TaskFlow](https://taskflow.github.io). You can find [it on
GitHub](https://github.com/taskflow/).
A good thing about this project is it's well documented and is under active
development.

It is loosely modeled after [Intel
TBB](https://software.intel.com/content/www/us/en/develop/tools/threading-building-blocks.html)
but aims to provide much simpler APIs. I think the clear advantages of using
Taskflow are its features like task graph visualization. I test drove the
`kmeans` example I was able to look at the task graph pretty easily.

## Task Graph Visualization
![](./images/outfile.svg)

I tried out one of the examples and ran it on NVidia's Jetson Xavier AGX SDK
that I [recently unpacked](http://www.mycpu.org/nvidia-xavier-gpu-agx/).

## TaskFlow Profiler
![](./images/tfprof.png)
![](./images/tfprof2.png)
![](./images/tfprof3.png)

## Multiplying Matrices on NVidia Jetson Xavier AGX
Because why not?

### Shapes (100x100) * (100x100)
```
matrix A: 100x100
matrix B: 100x100
matrix C: 100x100
```
**CPU took** 2ms
**GPU took** 114ms

### Shapes (100x500) * (500x500)
```
matrix A: 100x500
matrix B: 500x500
matrix C: 100x500

```
**CPU took** 27ms
**GPU took** 115ms

### Shapes (1000x2000) * (2000x3000)
```
matrix A: 1000x2000
matrix B: 2000x3000
matrix C: 1000x3000
```
**CPU took** 4372ms
**GPU took** 396ms

### Shapes (10000x20000) * (20000x30000)
```
matrix A: 10000x20000
matrix B: 20000x30000
matrix C: 10000x30000
```
**CPU took** 4372ms
**GPU took** 396ms
