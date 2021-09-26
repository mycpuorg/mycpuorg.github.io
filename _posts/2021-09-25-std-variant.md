---
layout: post
title: "std::variant - Why not just any?"
excerpt: "What is std::variant - discriminated union"
tags: [Linux, Code, C++, variant, STL, algorithms]
comments: true
---
## What is std::variant:

The class template std::variant represents a type-safe union. An instance of
std::variant at any given time holds a value of one of its possible/alternative
types.

As with unions, if a variant holds a value of some object type T, the object
representation of T is allocated directly within the object representation of
the variant itself. Variant is not allowed to allocate additional memory on
heap.

A variant cannot hold references, arrays, or `void`.

Just like unions, the contained type and value within a `std::variant`is the
that of the last value assigned to `std::variant`.

## Why not just `std::any`?
If you want to store a limited set of types in a container then `std::variant`
is a good choice. A simple alternative would be to use `std::any`. After all,
`std::any` can be used to store *any* type of value in it. But using
`std::variant` over `std::any` has a few advantages. Every a value in `std::any`
is accessed, the type is tested for at runtime. Which means there's no type
information stored at compile time. So, this relies on runtime type checks for
information (`RTTI`). `std::any` stores this type info on the heap since it must
be dynamically allocated.

However, a `std::variant` does not store its contained type on heap. To invoke
`std::variant` we don't have to know is currently contained type. It works
similar to a tuple, except that it only stores one object at a time.

```cpp
// the set of types are known ahead of time
using my_var_type = std::variant <int, std::string, bool, char>;
auto val = my_var_type {};
val = 7; // at this time holds an int

val = std::string { "This is a String" }; // now, the int is replaced with std::string

val = true; // std::string is now replaced with bool type
```

As we can see, the usage is pretty simple. Next post, we will look at visiting
variants.

