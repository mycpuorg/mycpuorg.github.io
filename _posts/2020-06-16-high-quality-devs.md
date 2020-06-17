---
layout: post
title: "Downsides of Starting in Modern C++"
excerpt: "Disadvantages of Using Modern C++"
tags: [C++, Modern, Lambdas, C++17]
comments: true
---
This post is not about the downsides of the language itself but those of
proposing to your colleagues to implement in `Modern C++`. When I say, modern
C++ I actually mean `C++17` but for the most part `C++14` specific features also
count.

```c++
class my_class_t {
    int blah;
	// other blah blah
};
```
For a simple class like the above, more people expect the following as how the
class is instantiated.
```c++
my_class_t *m = new my_class_t();
```
instead of:
```c++
auto m = std::make_unique<my_class_t>();
```
Once the scope of `m` expires, unless the pointer was moved to another scope the
default destructor is called avoiding delete.

### Everything's too convenient
As I mentioned in an [earlier post](http://www.mycpu.org/c++-smart_pointers/),
this requires a different mindset from using the old school `C` approach to
memory management. In some strange ways, relying on compilers, static checkers
and analyzers for various facilities reduces the amount of attention paid to the
code quality during code reviews.

For all it's powers and beauty Modern C++ unlocks, I'd urge you to pay more
attention to the code you write than ever before. Be paranoid, use all possible
`clang-tools` as possible but also be thorough with code reviews.
