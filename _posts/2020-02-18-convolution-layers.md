---
layout: post
title: "Baby Steps towards Convolution Nets - IV"
excerpt: "Convolution Layers"
tags: [Linux, Code, C++, MXNet, Deep Learning]
comments: true
---
In an [earlier post]() we saw what a ``crosscorr2d()`` operator is. In this we will
see how it is applied.

A convolution layer performs ``crosscorr2d()`` operation on the input and
kernels. It then adds a scalar bias to produce the output. A convolution layer
accepts the kernel and a scalar bias as inputs.

```cpp
class conv2D : public Block {
    conv2D(kernel_size, ...)
    {
	    Shape s{1};
        my_weight = get_params("weight", kernel_size);
        my_bias = get_params("bias", s);
    }

    void forward(x)
    {
        return crosscorr2d(x, my_weight.data(), my_bias.data());
    }
};
```

The forward computation function calls ``crosscorr2d`` function and adds the
bias.
