---
layout: post
title: "C++-17 New Attributes"
excerpt: "A few C++-17 attributes"
tags: [Linux, Code, C++, Attributes, C++-17]
comments: false
---
Remember the annoying warnings that you feel helpless about? Well you shouldn't
because C++-11 introduced ``Attributes`` which are indicators to the compiler to
either disable a warning or to flag a warning.

C++-17 introduced three main ``Attributes`` which are helpful.

## ``[[nodiscard]]``
As the name suggests, by annotating the code with ``[[nodiscard]]`` you are
telling the compiler that the values returned should not be discarded. This is
usefull when ignoring the returned value can cause errors in the program.

## ``[[maybe_unused]]``
In many cases, some of the variables or values allocated may not be
used. Example for this are function parameters that are not used, or data
members of a structure or a Class that are not used.

## ``[[fallthrough]]``
You have a ``switch`` statement and certain cases may not have an explicit
``break`` statement. In such cases, the compiler usually throws a warning. To
avoid these warnings we should explicitly add ``[[fallthrough]]`` statements.


#### Source
[C++17 - The Complete Guide](https://www.amazon.com/dp/396730017X/ref=cm_sw_r_tw_dp_U_x_tFqpEbWY7WBS3)
