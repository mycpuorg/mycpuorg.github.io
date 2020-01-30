---
layout: post
title: "Baby Steps towards Convolution Nets - II"
excerpt: "Idea Behind ConvNets"
tags: [Linux, Code, C++, MXNet, Deep Learning]
comments: true
---
In an [earlier post](http://www.mycpu.org/dummys-intro-to-convnets/), we tried
to get an intuition for what ConvNets are. In this post, we will try to look at
the mathematical representation of the two key ideas for the intuition.

```
For authoritative content written by real researchers and scientists, please
visit http://d2l.ai
I'm trying to make the key intuition "ELI5" level simple for myself.
```

Let's say, the input to an MLP is a matrix of $$h x w $$ dimensions and hidden
representations are also $$ h x w $$ matrices. Let $x[i,j]$ represent a pixel in the
image and $$ h[i,j] $$ represent a pixel in hidden representation. In an MLP, all
the $h.w$ hidden nodes receive inputs from $$ h.w $$ then the weights alone need to
be represented as a 4-D tensor. This would quickly make the usage of it
infeasible even for relatively small input sizes.

$$ h[i,j]=u[i,j]+∑k,lW[i,j,k,l]⋅x[k,l]=u[i,j]+∑a,bV[i,j,a,b]⋅x[i+a,j+b] $$
