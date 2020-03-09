---
layout: post
title: "CMake - How To Setup A C++ Project"
excerpt: "Steps To Setting Up a Dummy Project in CMake"
tags: [C++, Code, Software, Design, cmake, clang]
comments: true
---
In this post, we will walk through a dummy ``cmake`` setup for a ``C++``
application. For the experienced folks this is not going to be very informative,
however, can serve as a reference to start building complex projects with.

### A Simple Application
I want to create a project called ``my_app`` with the following directory
structure.
```
├── CMakeLists.txt
├── src
│   └── main.cc
├── test
│   ├── CMakeLists.txt
│   └── my_app_test.cc
└── third-party
    └── googletest -> /home/manoj/software/googletest
```

### CMakeLists.txt
``cmake`` is not really a replacement for ``make`` instead it is a generator of
``Makefile`` or other alternatives such as ``Ninja`` files.

#### Basic Config
``cmake`` makes frequent releases, so this will require you to set a minimum
required version.
```cmake
cmake_minimum_required(VERSION 3.16)
```

Set the compiler version and the language standard version.
```cmake
# Compiler and Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER clang-8)
set(CMAKE_CXX_COMPILER clang++-8)
```

I prefer to set the global project name in a variable and use that all over the
rest of the file so that file is readable.
```cmake
# set the project name
set(this_project my_app)
```

It's nice to use variables for other project directories too.
```cmake
# third party software
set(thirdparty third-party)
```

Here you are telling which languages are used in the project.
```cmake
# RUST, Erlang, what else
project(${this_project} C CXX)
```

Here you are telling that you want to enable testing for this dir and it's subdirs
```cmake
enable_testing()
```

This is used to add [``googletest``](https://github.com/google/googletest) as
your testing software.
```cmake
# add google test
add_subdirectory(${thirdparty}/googletest)
```

Specify the source files and headers
```cmake
# header files
set(headers
  include/my_app.h
  )

# src files
set(sources
  src/main.cc
  )
```

Finally, build the executable for this project
```cmake
# add the executable
add_executable(${this_project} ${sources} ${headers})
```

I have not tested these, but they are useful if you are building library
```cmake
target_include_directories(${this_project} PUBLIC include)
target_include_directories(${this_project} PRIVATE src)
```

The directory consisting tests and it's own ``CMakeLists.txt``
```cmake
add_subdirectory(test)
```

Nice options, especially useful if you are using them with [Emacs rtags](http://www.mycpu.org/emacs-rtags-helm/)
```cmake
set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```
