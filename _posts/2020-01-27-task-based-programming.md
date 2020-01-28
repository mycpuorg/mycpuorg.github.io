---
layout: post
title: "C++: Task Based Programming"
excerpt: "Scott Meyers Recommendation Makes Most Sense"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM, Threads, Concurrency, async]
comments: true
---
Scott Meyers recommends to "Prefer Task-based Programming to Thread-based" in
his book ``Effective Modern C++``. Consider a function that contains some work
to be done asynchronously. Many years ago, if you had asked enough programmers
how they would run this function asynchronously, a majority of them would have
said something along the lines of:

```cpp
// foo_async does some async work
int foo_async()
{
	 // do something asynchronous
}

std::thread t(foo_async);
```
Sounds reasonable, right? you have a function that needs to be executed
asynchronously, therefore, create a thread and outsource the work to be done to
this thread. We will come to why this the inferior choice among the ones
available. But first, let us look at the other option under consideration for
the job:

```cpp
// same function as before for illustration purposes.
int foo_async();
auto fut = std::async(foo_async);
```
This way is called the ``Task Based Asynchronous Execution`` where the onus of
thread creation, and its management is on the implementor of the language
standard or library. This is obvious. But other benefits of doing so are
slightly less subtle.

By accessing the ``shared state`` in the ``future`` or using ``get()`` you would
get the result from the asynchronous execution of the method ``foo_async()``. So
this returns the result of the operation conveniently.

It also throws the exceptions which are thrown by the async function itself. 
Apart from catching the results and exceptions, it frees the programmer from the
~~headaches~~ nuances and details of thread management. 

I know a lot of people who call this very ``un-C++`` because C++ developers are
used to being treated as adutls. To this, I'd argue that if the community has
found a way to get all the flexibility, without any additional cost. It also
provides a richer set of APIs and interfaces to think about programming.

Like we saw in an
[earlier post](http://www.mycpu.org/c++-threads-async-deferred/), the
``async()`` construct is terribly helpful without giving up much on the
performance benefits either.

