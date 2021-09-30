---
layout: post
title: "Simple Debug"
excerpt: "A Simple Example To Avoid Refactoring Log Statements"
tags: [Linux, Code, C++, variant, STL, algorithms, macros, helpers]
comments: true
---
This is not an earth shattering revelation but a simple trick which should work
in most cases.

Consider this:

```cpp
int a = 19;
auto some_var_name_here = foo();

// typical log statements look like this
SPDLOG(DEBUG_LOG, " a = {}", a);
SPDLOG(DEBUG_LOG, " some_var_name_here = {}", some_var_name_here);
```

In the right mind you do a code review and find that you must refactor the code
to rename `some_var_name_here` to `var_name`. So you do some clever find and
replace which only replaces variable names and leaves string within `""`
untouched.

So now you have
```cpp
// typical log statements look like this
SPDLOG(DEBUG_LOG, " a = {}", a);
SPDLOG(DEBUG_LOG, " some_var_name_here = {}", var_name);
```

Of course, reality is a bit more messy. So you can use preprocessor to help. You
can use a global macro to print var names and values.

```cpp
#define VAR_DEBUG(var) SPDLOG(DEBUG_LOG, #var " {}", (var))
```

As with all uses of macros you can shoot yourself in the feet and a lot of other
body parts pretty easily. So use judgement!
