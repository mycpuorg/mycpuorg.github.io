---
layout: post
title: "Using clang-format and clang-tidy"
excerpt: "One of the most useful tools ever!"
tags: [clang-format, llvm, C++]
comments: true
---
`clang-format` is easily one of the most useful tools for C++ developers. If you
still haven't used it or heard of it, just whip up your favorite editor, type a
`hello_world.cc`

Throw a couple of functions in with badly formatted code (of course, this is on
purpose we never write bad looking code, right?). Now all you need is:

```bash
clang-format -i hello_world.cc
```

And voila! your code gets automatically formatted to match the specifications
set by default. I usually, have a small file with my favorite set of formatting
specifications. I will share them here, but in general, I try to make my code
look closest to the `LLVM` coding style with a few major exceptions, I keep all
the function names, variable names everything else in lower case.

`clang-tidy` is one of the most versatile tools in the `llvm` tools package for
C++ developers. For the most part, a whole lot of static analysis and other
sanitizations can be run.

In most cases you use `clang-format` and `clang-tidy` together.
Below I will share a bare minimum config with `clang-format` and `clang-tidy` to
get started in a new project.

### `clang-format`
```
BasedOnStyle: LLVM

AccessModifierOffset: -4
AlwaysBreakTemplateDeclarations: true
ColumnLimit: 100
Cpp11BracedListStyle: true
IndentWidth: 4
MaxEmptyLinesToKeep: 2
PointerBindsToType: true
TabWidth: 4
```

### `clang-tidy`
```
Checks: '-*,readability-identifier-naming'
CheckOptions:
  - { key: readability-identifier-naming.NamespaceCase,          value: lower_case }
  - { key: readability-identifier-naming.ClassCase,              value: lower_case }
  - { key: readability-identifier-naming.ClassSuffix,            value: _t         }
  - { key: readability-identifier-naming.TypeAliasSuffix,        value: _t         }
  - { key: readability-identifier-naming.PrivateMemberSuffix,    value: _          }
  - { key: readability-identifier-naming.StructCase,             value: lower_case }
  - { key: readability-identifier-naming.FunctionCase,           value: lower_case }
  - { key: readability-identifier-naming.VariableCase,           value: lower_case }
  - { key: readability-identifier-naming.GlobalConstantCase,  	 value: UPPER_CASE }
  - { key: readability-identifier-naming.TemplateParameterCase,  value: UPPER_CASE }
```
