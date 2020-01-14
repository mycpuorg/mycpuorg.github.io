---
layout: post
title: "The Generator Pattern in C++"
excerpt: "Implementation of Generator Pattern in C++"
tags: [Linux, Code, C++, rotate, STL, algorithms]
comments: true
---
[Generator](https://en.wikipedia.org/wiki/Generator_(computer_programming)) is
a routine that can be used to control the iteration behavior of the loop. It is
essential in implementing programs that deal with __large__ amounts of data that
need to be iterated.
Consider the following code

```cpp
vector<int> foo() {
    // initialize a very large vector
    vector<int> my_vec = {/* VERY VERY LARGE AMOUNTS OF DATA */};

    // ... do something

    return my_vec;
}

void foo_caller() {
    // I'm screwed!
    vector<int> ret_foo_vec = foo();
}
```

This example just illustrates the situation in which one would run into issues
while dealing with large data structures. This is not a canonical example but
sufficient to get the point across.

Now, there isn't much we can do to avoid the huge amounts of data copied over
other than "pass a reference instead of value" but it is not always posssible to
avoid a situation where it must be done. Even in cases where memory copies are
avoided, ``foo_caller()`` will have to first copy the entire structure into
address space before starting to process data.

With generator pattern the reader or ``foo_caller()`` in our case can start to
process each entry in the container bit by bit. Python famously provides a nice
interface for this:

```python
def foo() -> Generator[int]:
    # large list my_arr
    for i in my_arr:
        yield i

    # foo done
	
def foo_caller():
    # handles elements one by one
    for gen_item in foo():
        print(gen_item)
```

### Brave Generators in C++ Pre-processor:
To acknowledge the enterprising people who have ventured into implementing
Generator patterns in C++ before it was mainstream, there are many blogs that
illustrate this, but as far I could tell, this blog explained clearly a while ago.
[Old School Generators in C++ ](https://terrainformatica.com/2008/06/25/generators-in-c-revisited/)
However, I'm sure there is a better way, because, after all Generators are
nothing but the following: a big iterable structure of data can be iterated over
with a reasonable implementation of ``next()`` and a way to indicate termination
of the iteration. There is no out of the box feature in C++ that could manage
this sorcery. But first, let's look at how one would implement without standard
language or library support for this feature.

### Lambdas in C++ to the rescue:
As mentioned in [C++ Lambda Post](http://www.mycpu.org/c++-lambda-functions/),
Lambda functions are kinda underrated or at least under utilized. My goal is to
start employing where it makes sense and/or the code more elegant.

```cpp
std::function<int(vector<int>)> generator = [] (container) {
    auto iter = std::begin(container);
    return [=]() mutable {
        return (iter != std::end(container)) ? iter++ : nullptr;
    };
};

// the large structure - my_vec from earlier
while ((i = generator(my_vec)) != nullptr)
    std::cout << "generator: " << i << std::endl;
```

### That is quirky syntax
For folks unfamiliar with the syntactic sugar of C++, we basically defined a
[a general purpose function
wrapper](https://en.cppreference.com/w/cpp/utility/functional/function) and
assigned it a lambda with a capture.

Who knows, I might have even enjoyed uncovering this.


#### Source:
+ [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)
+ [The greatest Lambda coverage :P ](http://www.mycpu.org/c++-lambda-functions/)
+ [C++ Generators in Macro](https://terrainformatica.com/2008/06/25/generators-in-c-revisited/)
+ [Generator Pattern](https://en.wikipedia.org/wiki/Generator_(computer_programming))
+ [SO-1](https://stackoverflow.com/questions/9059187/equivalent-c-to-python-generator-pattern)
+ [SO-2](https://stackoverflow.com/questions/12639578/c11-lambda-returning-lambda/12639820#12639820)
+ [SO-3](https://stackoverflow.com/questions/7213839/equivalent-in-c-of-yield-in-c)
