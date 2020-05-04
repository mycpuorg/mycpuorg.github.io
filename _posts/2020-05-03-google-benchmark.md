---
layout: post
title: "Beware the Benchmark Trap"
excerpt: "Beware! Google Benchmark A Terribly useful tool"
tags: [Linux, Code, Benchmark, C++]
comments: true
---
[Google Benchmark](https://github.com/google/benchmark) is an unusually useful
tool that lets you get an idea of how long a piece of code took to run with
different arguments and more.
Beware this trap since sum of the parts are not always the whole. Over-optmizing
a piece of code especially something that's not slow can result in unexpected
behavior since overly optimized code tends to be overfitted for the set of
inputs you are testing against.
