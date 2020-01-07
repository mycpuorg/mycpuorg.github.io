---
layout: post
title: "Preliminary Maths for Deep Learning"
excerpt: "A simple intro to the Math in Deep Learning"
tags: [Deep Learning, MXNet, TVM, AI, Compilers, Linux, OS]
comments: true
---
In this series of posts, I will pick a source for Deep Learning and try to share
my understanding and learnings on the topic. I want to treat this as my notes
from these lessons. My notes tend to be very rough, in an attempt to keep a
semblance of formality to them I am putting them out in public.

Machine Learning is primarily all about extracting information from
data. Unlike simple situations in life data in Machine Learning comes in larger
quantities. The most common form of this "large quantity" is a list of
numbers. Mathematically, we would like to apply some abstraction and perform
operations on them. _Vectors_ are a good abstraction for such cases, a
collection of such vectors can be represented by _Matrices_. Now _Tensors_ are a
generalized form of matrices allowing data to be represented along several axes.

Linear Algebra helps us understand the rules of simple operations on such a
collection of numbers. In this post, I will only list the items or topics from
Linear Algebra that we will need to apply Deep Learning in a practical manner.

Here's the list, each of which we will cover in detail in future posts.
+ Scalars
+ Vectors
+ Matrices
+ Tensors
+ Reduction of Tensors
+ Non-Reduction Operations: Sum
+ Dot Products
+ Matrix-Vector Products
+ Multiplication of Matrices
+ Norms - L2, Frobenius

If you think you need any further topics to apply Deep Learning it is likely to
be answered
[here](https://d2l.ai/chapter_appendix-mathematics-for-deep-learning/index.html).

As mentioned earlier, we will cover some or all of these topics in a bit more
detail in separate tiny bite sized posts. Until then, happy whatever!
