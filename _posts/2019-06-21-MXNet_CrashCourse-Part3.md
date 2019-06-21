---
layout: post
title: "Apache MXNet - Crash Course Part 3"
excerpt: "Automatic Differentiation in MXNet - super convenient"
tags: [Deep Learning, Machine Learning, ML, AI, Neurons, NDArray, MXNet]
comments: true
---

## Automatic Differentiation with autograd
Here we show how Automatic Differentiation can be set up using MXNet. This is super convenient way to set up backpropagation.
Follow along and have fun!

### Basic Usage:



```python
from mxnet import nd
from mxnet import autograd
```

+ Differentiate $f(x) = 2x^2$


```python
x = nd.array([[1,2], [3, 4]])
x
```




    
    [[1. 2.]
     [3. 4.]]
    <NDArray 2x2 @cpu(0)>



+ MXNet we can tell an NDArray that we plan to store a gradient by invoking it's attach_grad() method.


```python
x.attach_grad()
```

+ Define the function $y = f(x)$ to let MXNet store $y$, so that we can computer gradients later.
+ Put the definition inside a autograd.record() scope.


```python
with autograd.record():
    y = 2 * x * x

```

+ Invoke backpropagation by calling y.backward(). When y has more than one entry y.backward() is equivalent to y.sum().backward()


```python
y.backward()
```

+ If $y = 2x^2$ then $\frac{dy}{dx} = 4x$


```python
x.grad
```




    
    [[ 4.  8.]
     [12. 16.]]
    <NDArray 2x2 @cpu(0)>




```python
4 * x
```




    
    [[ 4.  8.]
     [12. 16.]]
    <NDArray 2x2 @cpu(0)>



## Using Python control flows
+ 


```python

```
