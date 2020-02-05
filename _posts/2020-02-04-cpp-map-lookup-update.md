---
layout: post
title: "Beware C++ Map's Lookup Operator"
excerpt: "[] updates too"
tags: [Linux, Code, C++, Maps, Lookup, Ambiguous]
comments: true
---
Imagine you are walking in the park on a beautiful day and suddenly ... !

## ``map:operator[]`` - Lookup or Insert
This is one of those quirks of the language ``C++``:
```cpp
map<int, ValueType> my_map;
if (my_map[42])
    // do something when key exists
else
    // do something when key does not exist
```

The above works counter intuitively since ``C++`` creates a value object for the
key ``42`` even when nothing is assigned since the object is created with a
default constructor. So in, effect the predicate always evaluates to ``true``
even when clearly there is no such key in the map.

The right way to check for membership in map is to use ``map::find()`` or ``map::at()``
So the above code looks like:
```cpp
if (my_map.find(42) != my_map.end())
    // do something when key exists
else
    // do something when key does not exist
```
Passing a key that's not already mapped to ``map::at()`` throws an exception.

This might seem like a quirk of C++'s that needs fixing, but it is the same
feature that allows the more intuitive functionality to exist in ``C++`` in the
first place:
```cpp
map<int, ValueType> my_map;
my_map[42] = my_val_obj;
```

I'm making this a separate post as a note to self.
