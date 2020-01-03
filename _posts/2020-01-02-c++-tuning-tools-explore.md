---
layout: post
title: "C++ Tuning Tools Exploration"
excerpt: "C++ Tuning Using System Tools"
tags: [Linux, Code, C++, Perf, Profiling, KTap, SystemTap]
comments: true
---
I'm currently exploring a good performance tuning framework for looking into C++
code. I'd like to start with something very simple, like the looking at the
individual operations on frequently used STL containers in C++ to evaluate the
expense of operations.

I have heard Visual Studio offers something for analyzing your application on
the go. However, there are several tools that exist for this purpose in the
Linux world. If I had to bet they are much more powerful just because of their
pervasiveness.

Systems engineers, especially, the ones who work on low levels of the stack have
a proclivity to using command line tools mostly because of their familiarity but
also because of the flexibility they offer to integrate with other power
tools. It is no coincidence that most core systems tools are simple command line
utilities that serve one purpose and serve them well. They are developed and
used by birds of the same feather. Therefore, such predictions are not blanket
statements, they are often mere observations.

This brings me back to my original quest for a tool and framework that I can use
consistently to analyze C++ code and understand better for tuning and optimizing
purposes.

For other tools that I like using, you can refer to [ftrace function
graph](http://www.mycpu.org/ftrace-function-graph/) and [perf
events](http://www.mycpu.org/perf-events/) from a while ago. Any similar suite
of tools such as ktap, systemtap would be something I would be open to looking
into.

Take the below code snippet for instance. All it is trying to do is overload the
operator ``-`` for obtaining the difference between two sets.

```
unordered_set& operator- (std::unordered_set& a, std::unordered_set& b)
{
    for (auto b_ele : b) {
		a.erase(b_ele);
    }
    return a;
}

```
Again, this would be very interesting to see how distant it is from a hand
written implementation of the set difference if implemented by an average
(above) C programmer.

Before I go boil the ocean, any suggestions are welcome.
