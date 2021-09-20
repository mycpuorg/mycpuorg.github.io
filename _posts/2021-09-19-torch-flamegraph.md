---
layout: post
title: Profiling PyTorch
excerpt: Trying to peek into inner workings on PyTorch
tags: [PyTorch, Deep Learning, Perf, Profiling, eBPF, Flamegraphs]
comments: true
---

In [a few](http://www.mycpu.org/debugging-deep-learning-flamegraph-contd/)
[earlier](http://www.mycpu.org/c++-mxnet-profiler/)
[posts](http://www.mycpu.org/debugging-deep-learning-flamegraph/), we have seen
how system profiler tools can give us a very useful insight into the deep
learning program workloads akin to the traditional software.

For a usecase I started to implement a version of the Variational AutoEncoders
such as [this
paper](https://www.connectedpapers.com/main/f4c5d13a8e9e80edcd4f69f0eab0b4434364c6dd/Variational-Autoencoder-for-Deep-Learning-of-Images-Labels-and-Captions/graph)

So I started to implement a version of it. In the mean time, I found an existing
version of a basic [VAE in
C++](https://github.com/prabhuomkar/pytorch-cpp/tree/master/tutorials/advanced/variational_autoencoder)
implemented with `libtorch`

I wanted to try and get a closer look at what was happening during each epoch. I
wanted the entire gravy not just the high level details, after all if I must
deploy this then I would like to understand it completely.


```
Note: false thumbnail, click for the actual graphs
```
<div
style="float:center;padding-left:30px;padding-right:10px;padding-bottom:3px"><a
href="/images/vae_gpu.svg"><img
src="/images/mlp_cpu_preview.png" width="600" height="500"
style="padding-bottom:3px"/></a><br><center><i>VAE Trained on GPU</i></center></div>

<div
style="float:center;padding-left:30px;padding-right:10px;padding-bottom:3px"><a
href="/images/vae_cpu.svg"><img
src="/images/mlp_cpu_preview.png" width="600" height="500"
style="padding-bottom:3px"/></a><br><center><i>VAE Trained on AVX512 CPU</i></center></div>


As you can see, imagine my disappointment that most time even with CPU version
was spent running HW accelerated code which cannot be profiled here. I won't
bother with off-CPU graphs here. This is what allows efficient execution for OMP
(and of course, CUDA offloading has more code which interfaces between CPU and
GPU). Nonetheless, no real workload can be traced here.

Next Step: Build `libtorch`with symbols ... ugh


