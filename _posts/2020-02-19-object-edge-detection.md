---
layout: post
title: "Baby Steps towards Convolution Nets - V"
excerpt: "Convolution for Images"
tags: [Linux, Code, C++, MXNet, Deep Learning]
comments: true
---
In [earlier posts](http://www.mycpu.org/convolution-layers/), we saw how
convolution layer cross correlates the input and kernels then adds a scalar bias
to produce outputs. In this post, we will apply it to perform a simple task of
detecting the edge of an object. We do so by identifying the location of the
pixel change.

Let's assume that our image is made up of ``6x8`` pixels. This image is only
made up of two colors ``Black (0)`` and ``White (1)``. 
```
array([[1., 1., 0., 0., 0., 0., 1., 1.],
       [1., 1., 0., 0., 0., 0., 1., 1.],
       [1., 1., 0., 0., 0., 0., 1., 1.],
       [1., 1., 0., 0., 0., 0., 1., 1.],
       [1., 1., 0., 0., 0., 0., 1., 1.],
       [1., 1., 0., 0., 0., 0., 1., 1.]])
```

Next if we construct a kernel with a height of 1 and width of 2. When we perform
the cross-correlation operation with the input, if the horizontally adjacent
elements are the same then the output is 0 else it is non-zero.

```
K = [[1, -1]]
```

If we apply this kernel over the above shown input then we can detect a change
along a column from one value (in this case, color) to another.

```
Y = crosscorr2d(X, K);
```
If we pass the kernel over our input then we can see that we will detect 1 for
an edge from white to black and -1 for edge from black to white.

```
array([[ 0.,  1.,  0.,  0.,  0., -1.,  0.],
       [ 0.,  1.,  0.,  0.,  0., -1.,  0.],
       [ 0.,  1.,  0.,  0.,  0., -1.,  0.],
       [ 0.,  1.,  0.,  0.,  0., -1.,  0.],
       [ 0.,  1.,  0.,  0.,  0., -1.,  0.],
       [ 0.,  1.,  0.,  0.,  0., -1.,  0.]])
```

That's our "Poor Man's Edge Detector``.
