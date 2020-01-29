---
layout: post
title: "Baby Steps towards Convolution Nets"
excerpt: "Idea Behind ConvNets"
tags: [Linux, Code, C++, MXNet, Deep Learning]
comments: true
---
## Where's Waldo?
Before you go finding Waldo, just think consciously about how your mind and eyes
are working to find Waldo in the image. In this image, Waldo is not the "odd man
out", therefore, moving your eyes somewhere outside the image and squinting
won't help here. You can try this if the object or the person you are spotting
has a stark contrast from every other object. But it's not the case in this
image, there are too many stripes.

![Where's Waldo](https://d2l.ai/_images/where-wally-walker-books.jpg)

If you found him, great! There are a couple of things that remained true to the
way you found him. You scanned the image in smaller chunks and looked for a
"pattern". When you recognize an object you respond to it the same way,
irrespective of where you find this object in the image. Of course, scientists
put a fancy label on this ``Translation Invariance``. We mentally divide up the
image into pieces/chunks. Our eyes focus on the smaller chunks in this
image. chunk your eyes/you must rrespective on what's showing in the neighboring
(or a distant chunk), and the fancy terms (or not so much in this case),
``Locality``.

This is the heart of the idea behind ConvNets. We will look at the Maths behind
it in the future post.

```
I was referring [Conv
Nets](https://d2l.ai/chapter_convolutional-neural-networks/why-conv.html), if you have the ability to keep the focus, this book is fantastic. I try to
simplify the material from this source, albeit poorly, here.
```
