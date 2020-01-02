---
layout: post
title: "Reading C++ as a Kernel Developer"
excerpt: "C++"
tags: [Linux, Code, C++]
comments: true
---
As noted in the yesterday's [blog post](http://www.mycpu.org/emacs-rtags-helm/)
I have started staring at a large C++ codebase.

I am, largely, used to doing so at the _beautiful_ code written in the Linux
Kernel. The reason the codebase in the Linux Kernel is beautiful is at multiple
layers.

## Linux Kernel Code
If you haven't done so already, I urge you to go through the following guide
for [The Linux Kernel Coding
Style](https://www.kernel.org/doc/html/v4.10/process/coding-style.html) it will
make too much sense.

Without delving into the details of the specific style guidelines themselves I
want to bring up a few points.

### First - the syntax
For a majority of the case, The syntax of C is exceedingly simple. If you are a
programmer, getting past "the pointer wall" is going to take a couple of days
but beyond that everything else is pretty straight forward. The 8 space tab
width might seem excessive but it makes reading code easier on the eye and the
brain since the brain can automatically go to deeper levels of nesting without
getting lost.

### Second - consistency
The Linux Kernel is one of the most readily reviewed codebases of all the open
source projects. Multiple people go through your patch in an organic fashion and
anything out of sorts is going to be underlined, possibly by multiple
developers. This is particularly true if the patch is touching any parts of the
Kernel that is crucial to other components. A sure way to get your patch
rejected is by being incosistent with the rest of the codebase, either in naming
variables or operating on the data structures.

## But the title said 'C++'
You are thinking that the title says *something something* **C++**. Yes, my
round about point is as I was looking at the codebase I wanted to make a simple
change. The situation in the code is that the code keeps track of certain
weights. It stores this in a map where the ``resource type`` is the key and
``weight`` is the value. Here the ``weight`` is equivalent of the ``popularity``
of the resources in question. I wanted to add a change which required to
implement an API that requests for the resources ordered by their
``popularity``.

Here's the code. Remember that I have changed the following snippet
significantly from the original source.

There's a lot going on with the code here, let's try to unravel bit by bit.

```C++
// pop_map is the map containing 
// resources and their popularities
using map_pair_type = decltype(pop_map)::value_type;
std::sort(
	std::begin(pop_map), std::end(pop_map),
	[] (const map_pair_type & p1, const map_pair_type & p2) {
		return p1.second < p2.second;
	}
);

for (auto d : pop_map) {
	pop_resources.push_back(d);
}

return pop_resources;
```

### ``decltype``
``decltype()`` takes an ``expr`` and predicts the type of the ``expr``. All the
dynamically typed languages provide this as a necessity to help the developer
during the not-so-occassional mishap. For example, Python provides ``type()``
and Ruby ``.class()``. There is also a very nice command utility called
``cdecl`` that you should download and play with for limitless fun.

Case in point.
```bash
cdecl> explain int *(*foo)(int *[][][])[]
declare foo as pointer to function (array of array of array of pointer to int) returning array of pointer to int
cdecl> 
```

### ``map::value_type``
``map::value_type`` returns ``pair<const key_type, mapped_type>``. This always
results in a ``it.first`` and ``it.second`` pair containing the key and value respectively.

### Powerful ``sort()`` method
Here we are trying to sort a container which is not single dimensional (since
it's a ``map``, it has a hierarchy that cannot be simply iterated over like an
array or a vector). The condition or the predicate to be applied to this sorting
can be non-trivial. For illustration, I have modified this situation to keep it
simple here where the values are simply the weights or the ``popularity`` of the
resources. However, the compare function is passed as a ``lambda`` function.
Although simple to understand, the snippet above achieves a sizeable amount of
code logic in a relatively small piece of code which could have spanned multiple
functions and multiple fragmented data structures if implemented in vanilla
``C``.

To be respectful to all the C-wizards out there (and there are many!), it might
be possible to write succinct looking C code to achieve the same. But what is
possible is not always feasible. With this, I will make a shocking inference,
choose the right tool for the job.

Happy New Year 2020!
