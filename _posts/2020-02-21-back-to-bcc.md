---
layout: post
title: "Let's Go Back to Using BCC"
excerpt: "Liking What I Saw in BCC"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM]
comments: true
---
Over the last couple of weeks I have found myself wanting to find certain things
out which were hard to answer quickly without the deep insight from kernel
level. The problem was that the issues were themselves not dire enough to spend
the extra time cooking up the bpftrace or my own BCC program. So I got around by
using vanilla ``perf`` or ``ftrace`` or ``flamegraphs``, but soon such small
niggles added up. Now I am at a point where I dearly miss programming in kernel
where the inertia to doing such things is minimal and access to info from
scheduler is readily available.

I'd like to quickly answer things like who preempted a ``pid`` out and how many
times in this period. "What are each of the threads spawned by a ``pid`` doing
right now?"

So I think it is time to go back to writing a few BCC programs in C++ to get
information at this level.
