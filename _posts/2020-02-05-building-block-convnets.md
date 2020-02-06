---
layout: post
title: "Baby Steps towards Convolution Nets - III"
excerpt: "Idea Behind ConvNets - CrossCorr2D"
tags: [Linux, Code, C++, MXNet, Deep Learning]
comments: true
---
In [earlier](http://www.mycpu.org/dummys-intro-to-convnets/)
[posts](http://www.mycpu.org/simple-math-convnets/), we tried to get an
intuition for the maths behind the key ideas of Convolution Networks.

Assume our inputs are images for this post. In general, Convolutional Networks
are pretty versatile but they are most famous for their application in Computer
Vision which primarily deals with inputs in the form of images. These images are
typically converted from RGB to their monochromatic representation [^1]. For
this discussion, the input is going to be a 2-D array.

[^1]: There are other transformations performed on the input, we will not
    discuss for simplicity.

## Cross Correlation Operator
Remember the ``Translation Invariance`` thing from
[here](http://www.mycpu.org/dummys-intro-to-convnets/)? This operator in terms
of the source code that actually helps us implement it. The idea behind it is
simple, imagine passing a filter or a lens over the input (a 2-D array). We will
be interested in what we "see" through this lens. How we use this info is
application dependent but we will use to arrive at our final goal via this
method of moving this lens over the input matrix in full (left-to-right, then top-to-bottom).

![A Thousand Words](https://d2l.ai/_images/correlation.svg)

In the fig, you can see the ``Kernel`` is the filter. For example: in this
figure we use the shaded numbers as follows.
```
0×0+1×1+3×2+4×3=19.
```

Similarly, the rest of the output is computed:
```
1×0+2×1+4×2+5×3=25
3×0+4×1+6×2+7×3=37
4×0+5×1+7×2+8×3=43
```

If the input size is ``H x W`` and filter/lens size is ``h x w`` then the output
is given by ``(H - h + 1) x (W - w + 1)``. This is because we apply the lens
filter only on locations where the filter wholly fits into the image i.e., the
Kernel filter does not spill over the edges of the image.

Below is the code snippet from the actual implementation of the MXNet library.
```cpp
for (index_t top_channel = 0 ; top_channel < top_channels_unsigned_ ; top_channel++) {
    int s2o = (top_channel % neighborhood_grid_width_ -\
            neighborhood_grid_radius_) * stride2_;
    int s2p = (top_channel / neighborhood_grid_width_ -\
            neighborhood_grid_radius_) * stride2_;
    int x2 = x1 + s2o;
    int y2 = y1 + s2p;
    for (index_t h = 0; h < static_cast<index_t>(kernel_size_); h++)
        for (index_t w = 0; w < static_cast<index_t>(kernel_size_); w++)
        for (index_t channel = 0; channel < static_cast<index_t>(bchannels); channel++) {
            if (is_multiply == true)
                out[nbatch][top_channel][i][j] += \
                    tmp1[nbatch][y1+h][x1+w][channel]*tmp2[nbatch][y2+h][x2+w][channel];
            else
                out[nbatch][top_channel][i][j] += std::abs(\
                    tmp1[nbatch][y1+h][x1+w][channel]-tmp2[nbatch][y2+h][x2+w][channel]);
        }
    out[nbatch][top_channel][i][j] /= sumelems;
}
```
*Oh My!*

Well, if you simplify it and remove all the real world considerations from the
implementation like tensor dimensions, data shapes in ``NHWC`` (N-Batches,
Height, Width, Channels) etc. then here's what you are really looking at:

```cpp
for (index_t h = 0; h < static_cast<index_t>(kernel_size_); h++)
    for (index_t w = 0; w < static_cast<index_t>(kernel_size_); w++)
        out[i][j] +=  input[i + h][j + w] * lens[i + h][j + w];
```

Now stare at it a few more times until you have digested it and convinced
yourself that they are indeed equal. If not, you can go through the [original
implementation
here](https://github.com/apache/incubator-mxnet/blob/master/src/operator/correlation.cc)

If you are happy, then hold it there and we will use this for greater things in
the next few "Baby Steps".
