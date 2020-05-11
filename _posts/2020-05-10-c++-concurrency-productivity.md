---
layout: post
title: "C++ Concurrency Library"
excerpt: "In search of a good library"
tags: [Linux, Code, C++, Promise, Future, STL, algorithms, tasks, concurrency]
comments: true
---
I am working on a project that requires me to build an asynchronous serving
solution which is able to manage concurrency natively and easily. This requires
me to be able to specify things like thread level parallelism out of the
box. But also, it requires me not to be bogged down by an overhead of details to
be specified.
I went through a bunch of solutions and talks that aim to provide this out of
the box. In the next few series of posts I hope to evaluate some of these
solutions cursorily in a more generic way.
Micro-benchmarking mentioned in the earlier [Google
Benchmarking](http://www.mycpu.org/google-benchmark/) may not be the best way
but in the absence of an alternative this might just be the best way possible.
