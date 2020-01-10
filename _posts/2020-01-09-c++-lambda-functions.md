---
layout: post
title: "The unharnessed power of C++ - Lambda functions"
excerpt: "C++ Lambda functions"
tags: [Linux, Code, C++, Perf, Profiling]
comments: true
---
The ``C++`` Gurus all over the world have spoke on multiple occassions about how the
language has helped shape the world in many ways. OS, Google, Mobile, Wall
Street, it's literally everywhere. Yet, credit to the C++ standards committee,
they come up with new features to add every revision/sub-revision. In my
opinion, there are a few such features that were borrowed from other programming
paradigms that helps ``C++`` stay relevant and, in many case, outperforming the
newer languages.

One of the most important and powerful features added in recent times to C++ is
Lambda. Lambda function in itself is not a new invention, it has been around in
functional programming languages like Lisp for many decades and has been a part
of CS Theory discussions for longer.

### C++ Lambda:
The lambda function was introduced in ``C++-11`` and enhanced with polymorphic
capabilities in ``C++14``. As discussed in my earlier post [C++
syntax](http://www.mycpu.org/complicate-but-readable/), these functions can be
passed to STL algorithms in C++ to achieve powerful results. They can also be
used to perform loose versions of meta programming and store lambda function in
std::function.

In theory, a lot of these can be achieved with with defining Functors() or
Classes with operator ``()`` overloaded.

The below example is taken from [``C++ High Performance`` Book](https://www.amazon.com/dp/B01MZX1E3Q/ref=cm_sw_r_tw_dp_U_x_uz-fEb27C98R)

In a nutshell, the lambda function capability enables programmers to pass
functions to regular functions, just as easily as a variable is passed.

```cpp
// Prerequisite 
auto vals = std::vector<int>{1, 3, 2, 5, 4}; 
 
// Look for number three 
auto three = 3; 
auto num_threes = std::count(vals.begin(), vals.end(), three); 
// num_threes is 1 
 
// Look for a numbers which is larger than three 
auto is_above_3 = [](int v){ return v > 3; }; 
auto num_above_3 = std::count_if(vals.begin(), vals.end(), is_above_3);
// num_above_3 is 2 
```

We pass a variable to run with ``std::count()`` and a function called
``std::count_if()`` to search with.

We can put an argument or a variable into the lambda function just like an
argument in to an equation.
```cpp
auto num_3 = std::count(vals.begin(), vals.end(), 3); 
auto num_above_3 = std::count_if(vals.begin(), vals.end(), [](int v){ 
  return v > 3; 
}); 
```
