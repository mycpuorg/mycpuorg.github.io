---
layout: post
title: "Apache MXNet - Crash Course Part 2"
excerpt: "Basic Neural Network creation"
tags: [Deep Learning, Machine Learning, ML, AI, Neurons, NDArray, MXNet]
comments: true
---

## Basic Neural Network
This is a follow-along from the MXNet crash course: [Create NN](https://beta.mxnet.io/guide/crash-course/2-nn.html)
+ We create a basic Neural Network in this post.
+ Follow along from [previous post](http://www.mycpu.org/MXNet_CrashCourse-Part1/)
+ Have fun!


```python
from mxnet import nd
from mxnet.gluon import nn
```

## Create your Neural Network's first layer
Start with a dense layer with 2 output units


```python
layer = nn.Dense(2)
layer
```




    Dense(None -> 2, linear)



+ Then initialize its weights with the default initialization method, which draws random values unformly from [-0.7, 0.7]


```python
layer.initialize()
```

+ Then do a forward pass with random data. We create (3,4) shape random input x and feed into the layer to compute the output.


```python
x = nd.random_uniform(-1, 1, (3, 4))
layer(x)
```




    
    [[ 0.01587485  0.03087313]
     [ 0.02257253 -0.02103142]
     [ 0.06961896  0.01239835]]
    <NDArray 3x2 @cpu(0)>




```python
layer.weight.data()
```




    
    [[-0.00873779 -0.02834515  0.05484822 -0.06206018]
     [ 0.06491279 -0.03182812 -0.01631819 -0.00312688]]
    <NDArray 2x4 @cpu(0)>



## Chain layers into a neural network
+ First, a simple case that a neural network is a chain of layers. During the forward pass, we run layers sequentially one-by-one. The following code implements a famous network called LeNet through nn.Sequential


```python
net = nn.Sequential()

# Add a sequence of layers.
net.add(# Similar to Dense, it is not necessary to specify the input channels
    # by the argument 'in_channels', which will be automatically inferred
    # in the first forward pass. Also, we apply a rely activation on the output.
    # In addition, we can use a tuple to specify a non-square kerne size, such as
    # 'kernel_size=(2,4)'
    nn.Conv2D(channels=6, kernel_size=5, activation='relu'),
    # One can also use a tuple to specify non-symmetric pool and stride sizes
    nn.MaxPool2D(pool_size=2, strides=2),
    nn.Conv2D(channels=16, kernel_size=3, activation='relu'),
    nn.MaxPool2D(pool_size=2, strides=2),
    # The dense layer will automatically reshape the 4-D output of last
    # max pooling layer into the 2-D shape: (x.shape[0], x.size/x.shape[0])
    nn.Dense(120, activation='relu'),
    nn.Dense(84, activation='relu'),
    nn.Dense(10)
)
net
```




    Sequential(
      (0): Conv2D(None -> 6, kernel_size=(5, 5), stride=(1, 1))
      (1): MaxPool2D(size=(2, 2), stride=(2, 2), padding=(0, 0), ceil_mode=False)
      (2): Conv2D(None -> 16, kernel_size=(3, 3), stride=(1, 1))
      (3): MaxPool2D(size=(2, 2), stride=(2, 2), padding=(0, 0), ceil_mode=False)
      (4): Dense(None -> 120, Activation(relu))
      (5): Dense(None -> 84, Activation(relu))
      (6): Dense(None -> 10, linear)
    )



+ The usage of nn.Sequential() is similar to nn.Dense. They are both subclasses of nn.Block.
+ Now initialize the weights and run the forward pass.


```python
net.initialize()

# Input shape is (batch_size, color_channels, height, width)
x = nd.random_uniform(shape=(4, 1, 28, 28))
y = net(x)
y.shape
```




    (4, 10)



+ We can use [] to index a particular layer. For example, the following accesses the 1st layer's weight and 6th layer's bias.


```python
(net[0].weight.data().shape, net[5].bias.data().shape)
```




    ((6, 1, 5, 5), (84,))



## Create a Neural Network flexibly
+ In nn.Sequential, MXNet will automatically construct the forward function that sequentially executes added layers.
+ Introducing another way to contruct a network with a flexible forward function.
    + Create a subclass of nn.Block and implement two methods:
         + \_\_init\_\_ create the layers
         + forward define the forward function

```python

class MixMLP(nn.Block):
    def __init__(self, **kwargs):
        # Run `nn.Block`'s init method
        super(MixMLP, self).__init__(**kwargs)
        self.blk = nn.Sequential()
        self.blk.add(nn.Dense(3, activation='relu'),
                     nn.Dense(4, activation='relu'))
        self.dense = nn.Dense(5)
    def forward(self, x):
        y = nd.relu(self.blk(x))
        print(y)
        return self.dense(y)

net = MixMLP()
net


```




    MixMLP(
      (blk): Sequential(
        (0): Dense(None -> 3, Activation(relu))
        (1): Dense(None -> 4, Activation(relu))
      )
      (dense): Dense(None -> 5, linear)
    )



+ Usage of net remains the same as before


```python
net.initialize()
x = nd.random_uniform(shape=(2,2))
net(x)
```

    
    [[0.         0.00072302 0.00043636 0.00045482]
     [0.         0.00081594 0.00049244 0.00051327]]
    <NDArray 2x4 @cpu(0)>





    
    [[ 3.9329490e-05  1.1599804e-05  4.5617679e-05 -1.8504743e-05
       2.4952336e-05]
     [ 4.4383840e-05  1.3090528e-05  5.1480143e-05 -2.0882841e-05
       2.8159035e-05]]
    <NDArray 2x5 @cpu(0)>



+ Accessing a particular layer's weights can also be done similarly:


```python
net.blk[1].weight.data()
```




    
    [[ 0.0521711  -0.02633957 -0.03170411]
     [-0.01043678  0.04172656  0.05394727]
     [-0.04401097  0.02518312  0.06339083]
     [-0.00614183  0.02624836 -0.00232279]]
    <NDArray 4x3 @cpu(0)>




```python

```
