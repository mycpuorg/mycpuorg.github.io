---
layout: post
title: "std:visit ing a std::variant"
excerpt: "Visiting a std::variant"
tags: [Linux, Code, C++, variant, STL, algorithms]
comments: true
---
As discussed in [earlier post](http://www.mycpu.org/std-variant/) a
`std::variant` can hold a number of different types of data much like a `union`
can. However, what gives it an edge in C++ is the ability to run a polymorphic
function on the `std::variant`. C++'s `stdlib` provides a function called
`std::visit` which does just that.

`std::visit` is a global function which can run a polymorphic function/lambda on
the `std::variant` object.

```cpp
using my_var_type = std::variant <int, std::string, bool, char>;
auto val = my_var_type {};

// idea is that do_something() is a polymorphic functions function
// which applies for all types contained in the variant var
// ex:
//     void do_something(const T& v) { std:: cout << v << std::endl; }
std::visit([](const auto& var) mutable { do_something(var); }, val);
```


### What happens under the hood here?
The compiler generates C++ of the lambda for each type contained in the
variant. Somewhat similar to how a templatized code is expanded at compile
time. The above example gets loosely translated to the following code:

```cpp
struct lambda_var {
    auto operator() (const int& v) { do_something(v); }
    auto operator() (const std::string& v) { do_something(v); }
    auto operator() (const bool& v) { do_something(v); }
    auto operator() (const char& v) { do_something(v); }
};
```

### `std::holds_alternative<T>()` to the rescue
The `std::visit()` function is expanded to checking for types of each
alternative in the `std::variant`:

```cpp

// Compiler's code generated
auto visit_impl(lambda_var f, const my_var_type& v) {
    if (std::holds_alternative<int>(v)) { return f(std::get<int>(v)); }
    if (std::holds_alternative<std::string>(v)) { return f(std::get<std::string>(v)); }
    if (std::holds_alternative<bool>(v)) { return f(std::get<bool>(v)); }
    if (std::holds_alternative<char>(v)) { return f(std::get<char>(v)); }
}


// Actually calls the above generated function
visit_impl(lambda_var(), my_var_type);
```

The size allocated for the variant is equal to the largest object type among its
members. In the case of `my_var_type` it is `std::string`
