---
layout: post
title: "Unpacking the nVIDIA AGX"
excerpt: "The massive GPU Beast"
tags: [Linux, GPU, CUDA, nVIDIA, Xavier, Jetson, AGX]
comments: true
---

So I ordered the
[https://developer.nvidia.com/embedded/jetson-agx-xavier-developer-kit](nVidia's
Jetson Xavier AGX Developer Kit). It arrived today in mail and I unpacked the
beast. I am very excited to get me some Deep Learning with CUDNN library.

**Note:** Please have a dedicated HDMI screen connected to the Xavier kit while
installing, without this you will have a lot of headache installing.

### Installation Hiccups
In the past, I have spent a lot of time playing with boards and Linux Kernel
bringups, [installing full Linux distro on things I shouldn't
be](http://www.mycpu.org/ubuntu-on-android/) without which this could be a bit
of a problem mainly because AGX was supposed to come with Linux and all the
related packages pre installed, but mine simply refused to boot up!

NVidia has a massive website dedicated for discussions and guidance on
installation. Since I bought the fancy Jetson Xavier AGX kit (which is the
pricier high end kit) I assumed it came installed with the latest OS too. Turns
out, all the instructions websites asked me to stop whatever I was doing and go
straight through the OS installation steps, sigh!

In general, I despise installation through GUI. Sure, I have to install
everything through the sdk manager software which has an added "joy" since it invents
new excuses to keep balking. I quickly figured it's just a shell script with a
GTK widget window in the front. Then I was able to find my way into getting it
to **start** installing.

### Android Filesystem
The user partition on the eMMC is nothing but Android-esque `bootloader`, `kernel`, `system` partition.

### Thermal!
Yes, the Xavier AGX is a massive power packed box but the thermal dissipation is
substantial, so much that I could feel the warmth physically while sitting a good
3 feet away from it.

### Install Ubuntu
Finally, after installing all the components through NVidia's SDKManager, I
installed Ubuntu on the board.

### Coming up ... Task Based CUDA work
