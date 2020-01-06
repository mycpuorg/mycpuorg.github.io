---
layout: post
title: "Systems Software for Deep Learning"
excerpt: "Deep Learning Compilers, OS"
tags: [Deep Learning, MXNet, TVM, AI, Compilers, Linux, OS]
comments: true
---
Here's the story, I thought I had a billion dollar idea recently and found that
there are at least tens of papers written about it, a successful startup and, a
major open source project actively contributed to by hundreds of developers
worldwide.

The idea was pretty simple: Machine Learning and Deep Learning domain is
primarily dominated by code written by Applied Scientists who focus on the
higher level frontend frameworks or "productivity" frameworks, as they are
called. You know people who design different models for achieving a particular
goal which takes a different muscle and skill.

However, what is clearly not available outside the closed doors of behemoth
organizations is the easy access to efficient runtimes and inferences of Deep
Learning Models since most applications as of today require careful tuning and
training of parameters which occurs under the careful guidance of applied
scientists.

TensorFlow, MXNet, PyTorch, are all examples of the frontend frameworks that
provide different benefits to express sophisticated ideas through simple APIs
(and sometimes automatically baked into such as automatic differentials
computation in MXNet).

Companies like Nvidia, Intel, AMD, ARM, Qualcomm, are all scampering to come up
with standard libraries that allow to take complete advantage of their hardware
platforms which were previously unavailable without signing your soul away. Case
in point, ``MKL, SNPE, Halide, ARM Compute Library, CUDA-DNN`` etc.

If the mobile revolution was anything to go by, the _App Generation_ of
developers kept playing the chasing game in keeping their apps in sync with the
whims and fancies of each new generation and modification to their
platforms. Many Android developers suffer from a particular flavor of PTSD that
unites them like no other.

Deep Learning/Machine Learning domain attributing to the explosion of practical
applications in recent times, their attractiveness in academia and industry has
seen meteoric rise. This only meant there are all sorts of complex Neural Nets
being invented for popular applications, say, Computer Vision with varied
degrees of effectiveness in results.

It, therefore, implies that coming up with platform for marrying various
permutations will only result in diluted and fragmented implementations at
best.

An intermediate representation of the results from all the high level frameworks
that can then be translated into each of the different target hardware
environments with a transferable ML Cost Model should be the obvious place to
look for low hanging fruits. As in, *any* implementation of such work should be
lapped up by all the disparate vendors who then choose to compete in a level
playing field with merit based results to show for their differentiation.

Turns out, there is such a thing in the form of
[TVM](https://tvm.apache.org/). I had heard of it but somehow could not bring my
head out of wherever it was stuck. I was deeply embarassed.

But then, I realize that this is not a ``fail``, it's a validation of my
thought, if anything. The fact that hundreds of smart, talented people have
recently started to look closely into this field which has garnered attention
from companies worldwide is net positive for the field. I honestly think this
cannot be a field that would be ``saturated`` in any foreseeable future. Since,
there is going to be disparate sets of hardware computes for different workloads
(CPU, GPU, DSP, NPU etc) there is going to be room in this field for all sorts
of research work.

This gives me hope that free markets might actually do good to emerging fields
in technology after all.
