---
layout: post
title: "That's a rotate!"
excerpt: "C++ Seasoning Talk"
tags: [Linux, Code, C++, rotate, STL, algorithms]
comments: true
---
Sean Parent is a well known C++ expert who has given several talks. One of his
most famous talks is called C++ Seasoning.
I urge you to watch it yet, but not just yet! First I want to mention a couple
of things that were important for me. This talk has succinct, invaluable
lessons that almost sound too good to be true.

Primarily, he provides three goals you must have as a programmer to improve the
quality of your code:
+ Now Raw Loops
+ Now Raw Synchronization Primitives
+ Now Raw Pointers

<iframe width="560" height="315" src="https://www.youtube.com/embed/qH6sSOr-yk8" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


#### Meme:

``That's a rotate``

- Sean Parent

Sean's frequent reference to his favorite algorithm during the talk gave rise to
this meme. Oddly, it makes think of trying to randomly apply ``rotate()``. It's
like I'm looking to apply ``rotate()`` only to have the satisfaction of somehow
attaching myself to this talk. I enjoyed this talk a lot!

### Problem:
The gist of the problem is while trying to avoid using Raw Loops in your
program, look for alternatives in the standard library since it typically has an
optimized implementation without risk of introducing bugs.

So a problem that came to mind is with the following input:

![](https://media.geeksforgeeks.org/wp-content/uploads/simplearray.png)

Required Output is:

![](https://media.geeksforgeeks.org/wp-content/uploads/arrayRotation.png)

### Bad Solution:
```cpp
template <typename Container>
auto move_n_elements_to_back(Container& c, size_t n) {
    // Copy the first n elements to the end of the container
    for(auto it = c.begin(); it != std::next(c.begin(), n); ++it) {
      c.emplace_back(std::move(*it));
    }
    // Erase the copied elements from front of container
    c.erase(c.begin(), std::next(c.begin(), n));
}
```
### Clever Solution (didn't know ``rotate()``):
Even if you did not know about the existence of ``rotate`` algorithm
implementation you could solve the problem with
```cpp
template <typename Container>
auto move_n_elements_to_back(Container& c, size_t n) {
    auto split_point = std::next(c.begin(), n);
    std::reverse(c.begin(), split_point);
	split_point++;
	std::reverse(split_point, c.end());	
	std::reverse(c.begin(), c.end());	
}
```

### Good Solution:
```cpp
template <typename Container>
auto move_n_elements_to_back(Container& c, size_t n) {
     auto new_begin = std::next(c.begin(), n);
     std::rotate(c.begin(), new_begin, c.end());
}
```
