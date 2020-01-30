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
For now, let's assume they have some sort of advantage over [Multi-Layer
Perceptrons](http://www.mycpu.org/debugging-deep-learning-flamegraph/) that make them worth understanding.

```
For authoritative content written by real researchers and scientists, please
visit http://d2l.ai
I'm trying to make the key intuition "ELI5" level simple for myself.
```

Let's say, the input to an MLP is a matrix of `` h x w `` dimensions and hidden
representations are also `` h x w `` matrices. Let `` x[i,j] `` represent a pixel in the
image and `` h[i,j] `` represent a pixel in hidden representation. In an MLP, all
the $h.w$ hidden nodes receive inputs from `` h.w `` then the weights alone need to
be represented as a 4-D tensor. This would quickly make the usage of it
infeasible even for relatively small input sizes.

```
h[i,j] = u[i,j] + ∑sub(k,l) W[i,j,k,l] ⋅ x[k,l] = u[i,j] + ∑sub(a,b) W[i,j,a,b] ⋅ x[i+a, j+b]
```

In the RHS, let ``k=i+a and l=j+b.``. Indices, ``a, b`` can be positive or
negative.

In effect, for any given pixel (i,j) in the hidden layer ``h[i,j]``, we compute its value by summing over pixels in x, centered around (i,j) and weighted by ``W[i,j,a,b]``.

Now, if you refer to the earlier post with verbiage about intuition, we
mentioned two important points about Convolutions. The first one said, that our
vision generates a similar response to the same object irrespective of where the
object appears in the image. This means that a shift in inputs ``x`` should
simply lead to a shift in ``h``.
But from the previous equation stated above, that is possible only if ``W`` and
``u`` don't depend on ``(i.j)`` i.e., they don't change based on an offset from
the central pixel. ("you can move around"). Therefore, we have ``V[i,j,a,b] =
V[a,b]`` and ``u`` is a constant. Then, ``h[i,j]=u+∑sub(a,b) V[a,b] ⋅ x[i+a,
j+b].``

From d2l.ai:
```
This is a convolution! We are effectively weighting pixels (i+a,j+b) in the
vicinity of (i,j) with coefficients V[a,b] to obtain the value h[i,j]. Note that
V[a,b] needs many fewer coefficients than V[i,j,a,b].
```

This means, we have reduced the number of parameters used in the calculations
significantly.

Now let’s invoke the second principle - ``locality`` which implies that we don't
have to look very far away from centered pixel ``(i,j)`` in order to obtain the relevant information to assess what is going on at ``h[i,j]``. This means that outside some range |a|,|b|>Δ, we should set V[a,b]=0. Therefore, we can rewrite ``h[i,j]``

```
h[i,j = u + ∑sub(a=-Δ), super(Δ)∑sub(b=-Δ), super(Δ) V[a,b] ⋅ x[i+a, j+b].
```

Phew, That's it. That's a Convolution Layer! Let's take it on a faith basis for
now, that implementing the above somehow allows us to practically scale from the
earlier discussed [Fully Connected MLP](http://www.mycpu.org/more-on-mlp/)

From d2l.ai:
```
While previously, we might have required billions of parameters to represent just a single layer in an image-processing network, we now typically need just a few hundred. The price that we pay for this drastic modification is that our features will be translation invariant and that our layer can only take local information into account. 
```

In a future post, we will try and see how all of this is actually used in Image
Based Models.
