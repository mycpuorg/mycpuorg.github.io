---
layout: post
title: "Apache MXNet - Crash Course Part 1"
excerpt: "MXNet Crash Course"
tags: [Deep Learning, Machine Learning, ML, AI, Neurons, NDArray, MXNet]
comments: true
---

# MxNet crash course - Part 1
+ This is an attempt to follow the crash course listed here [MxNet Crash Course](https://beta.mxnet.io/guide/crash-course/1-ndarray.html)
+ There are some gaps in the crash course page of the MXNet website while this is a tested/verified set of steps.
+ Jupyter notebooks make it convenient to share.
+ Ensure you have installed the latest version of mxnet (>=1.4.x) at the time of this post.


```python
# !pip install -U mxnet
from mxnet import nd

```
+ Create a 2D array with 1,2,3 and 4,5,6


```python
nd.array(((1,2,3), (5,6,7)))
```




    
    [[1. 2. 3.]
     [5. 6. 7.]]
    <NDArray 2x3 @cpu(0)>



+ create a 2x3 matrix with 1's


```python
x = nd.ones((2,3))
x
```




    
    [[1. 1. 1.]
     [1. 1. 1.]]
    <NDArray 2x3 @cpu(0)>



+ Create arrays with random values in a range.
+ Ex: values between -1 and 1 in the shape of 2x3


```python
y = nd.random_uniform(-1, 1, (2, 3))
y
```




    
    [[0.09762704 0.18568921 0.43037868]
     [0.6885315  0.20552671 0.71589124]]
    <NDArray 2x3 @cpu(0)>



+ you can also fill an array of a given shape with a give value such as 2.0


```python
x = nd.full((2, 3), 2.0)
x
```




    
    [[2. 2. 2.]
     [2. 2. 2.]]
    <NDArray 2x3 @cpu(0)>



+ As with numpy, the dimensions of each ND array are accessible by accessing the .shape attribute. We can also query its size, which is equal to the product of the components of the shape. In addition, .dtype tells the data type of the stored values.


```python
(x.shape, x.size, x.dtype)
```




    ((2, 3), 6, numpy.float32)



## Operations


```python
x * y
```




    
    [[0.19525409 0.37137842 0.86075735]
     [1.377063   0.41105342 1.4317825 ]]
    <NDArray 2x3 @cpu(0)>




```python
y.exp()
```




    
    [[1.1025515 1.204048  1.5378398]
     [1.9907899 1.2281718 2.0460093]]
    <NDArray 2x3 @cpu(0)>



+ matrix's transpose to compute a proper matrix-matrix product


```python
nd.dot(x, y.T)
```




    
    [[1.4273899 3.219899 ]
     [1.4273899 3.219899 ]]
    <NDArray 2x2 @cpu(0)>



## Indexing


```python
y[1,2]
```




    
    [0.71589124]
    <NDArray 1 @cpu(0)>



+ Reading the second and third columns from y


```python
y[:, 1:3]
```




    
    [[0.18568921 0.43037868]
     [0.20552671 0.71589124]]
    <NDArray 2x2 @cpu(0)>



+ and setting them to a specific element


```python
y[:, 1:3] = 2
y
```




    
    [[0.09762704 2.         2.        ]
     [0.6885315  2.         2.        ]]
    <NDArray 2x3 @cpu(0)>



+ Multi-dimensional slicing 


```python
y[1:2, 0:2] = 4
y
```




    
    [[0.09762704 2.         2.        ]
     [4.         4.         2.        ]]
    <NDArray 2x3 @cpu(0)>



## Converting between MXNet NDArray and NumPy


```python
a = x.asnumpy()
(type(a), a)
```




    (numpy.ndarray, array([[2., 2., 2.],
            [2., 2., 2.]], dtype=float32))




```python
nd.array(a)
```




    
    [[2. 2. 2.]
     [2. 2. 2.]]
    <NDArray 2x3 @cpu(0)>




```python

```
