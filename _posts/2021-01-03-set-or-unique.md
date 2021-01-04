---
layout: post
title: "C++: How a simple question helped me for a New Year's Resolution"
excerpt: "Finding unique elements"
tags: [C++, STD, lambda]
comments: true
---
Firstly, this post is a short walk through in ``C++`` code, but feel free to
jump to the conclusion for my New Year's Resolution (Spoiler: It's not an "Aha!"
moment)

## Finding Unique elements in you vector
So,  here I was minding my business on a nice weekend. I just wanted to find the
unique elements in my vector of integers. So I started reaching for the nearest
``std::set`` data structure. Turns out, this is likely implemented using [``RB
Trees``](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) according to [CPP
Reference](https://en.cppreference.com/w/cpp/container/set)

## What If ... ?
I was wondering if this is in critical path of my code, then should I blindly
rely on it? So I just looked up the sample impl on wikipedia linked above. My
head spun for a few seconds to see that the impl invokes every family member for
each node including ``GetUncle(Node* n)``. This is usually a bad sign, it
smells.

So, instead of looking at the gargantuan implementation details in the STDLIB
codebase, I decided to simply measure.

## Setup Benchmark
So I setup a quick Google Benchmark harness:
```cmake
cmake_minimum_required(VERSION 3.16)

# Compiler and Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER clang-8)
set(CMAKE_CXX_COMPILER clang++-8)

# set the project name
set(this_project project_name)

# RUST, Erlang, what else
project(${this_project} C CXX)

add_subdirectory(src)

find_package(benchmark REQUIRED)
target_link_libraries(${this_project} benchmark::benchmark)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

## Measure the Truth
So I got the following code cranked up in no time.
### Setup:
+ Get a few (1000) random numbers
+ Insert them into a set (RB Tree)
+ 
```cpp
std::vector<int> nums_;
std::vector<int> nums_cp_;
auto helper_(int n) -> std::vector<int> {
    auto gen_node_ = [=]() -> int { return rand() % 100; };
    while (n--) { nums_.emplace_back(gen_node_()); }
    std::copy(nums_.begin(), nums_.end(), std::back_inserter(nums_cp_));
    std::sort(nums_cp_.begin(), nums_cp_.end());
    return nums_;
}

static void BM_InitializeNums(benchmark::State& state) {
    for (auto _ : state) { helper_(1000); }
}
BENCHMARK(BM_InitializeNums);

static void BM_Set(benchmark::State& state) {
    std::set<int> s;
    for (auto _ : state) {
	for (const auto& e_ : nums_) { s.insert(e_); }
    }
}
BENCHMARK(BM_Set);

BENCHMARK_MAIN();
```

### Result:
```
2021-01-03 21:50:24
Running ./src/project_name
Run on (16 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 64 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 8192 KiB (x2)
Load Average: 0.74, 0.97, 0.81
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
BM_StringCreation       32.2 ns         32.2 ns     21667369
BM_StringCopy           74.8 ns         74.8 ns      9387404


BM_InitializeNums 1780847955 ns   1780435494 ns          136
BM_Set              38689124 ns     38681053 ns           18
```

If a string copy takes `75ns` a RB Tree insertion of 1000 numbers takes millions
of times longer? My intuition was that something was not right. But it's the
hallowed ``stdlib`` itself! You know, it comes with all that claim of:
```
"It's been
optimized and tested over decades, so you dummies cannot outdo it in reasonable
time"
```

To be fair, I also remember the famous:
```
"It's not magic, you could muck it up as easily as another language"
```

If there's one skill I have built over time, it's running into situations that
could only be described as: "you are
using it akk wrong, it won't work well in this case"

I'm somewhat of an expert in this myself :D

## Am I doing something wrong?
There's only one way to find out, what if I could swap all the unique elements
to the front side of the vector and leave all the duplicates to the back?
There's an algorithm just for this in the standard library - it's called
`std::unique()`

Allegedly, [one such impl](https://www.cplusplus.com/reference/algorithm/unique/) of this algo is:
```cpp
template <class ForwardIterator>
  ForwardIterator unique (ForwardIterator first, ForwardIterator last)
{
  if (first==last) return last;

  ForwardIterator result = first;
  while (++first != last)
  {
    if (!(*result == *first))  // or: if (!pred(*result,*first)) for version (2)
      *(++result)=*first;
  }
  return ++result;
}
```

Now, further if the array is already sorted then it must be very easy.
```cpp
std::vector<int> nums_;
std::vector<int> nums_cp_;
auto helper_(int n) -> std::vector<int> {
    auto gen_node_ = [=]() -> int { return rand() % 100; };
    while (n--) { nums_.emplace_back(gen_node_()); }
    std::copy(nums_.begin(), nums_.end(), std::back_inserter(nums_cp_));
    std::sort(nums_cp_.begin(), nums_cp_.end());
    return nums_;
}

static void BM_InitializeNums(benchmark::State& state) {
    for (auto _ : state) { helper_(1000); }
}
BENCHMARK(BM_InitializeNums);

static void BM_Set(benchmark::State& state) {
    std::set<int> s;
    for (auto _ : state) {
	for (const auto& e_ : nums_) { s.insert(e_); }
    }
}
BENCHMARK(BM_Set);

static void BM_UniqueUnSort(benchmark::State& state) {
    for (auto _ : state) {
	auto uniq_iter = std::unique(nums_.begin(), nums_.end());
	std::vector<int> uniqs_;
	nums_.erase(uniq_iter, nums_.end());
    }
}
BENCHMARK(BM_UniqueUnSort);

static void BM_UniqueSort(benchmark::State& state) {
    for (auto _ : state) {
	auto uniq_sort_iter = std::unique(nums_cp_.begin(), nums_cp_.end());
	nums_cp_.erase(uniq_sort_iter, nums_cp_.end());
    }
}
BENCHMARK(BM_UniqueSort);

BENCHMARK_MAIN();
```

### Results:
So the simple stupid algo is like **30x** faster. Further, if the array is already
sorted then it's **30000x** faster! To be fair, the sorting is a significant
overhead so unless your data is already sorted simply sticking with the swapping
algo gives a significant speedup.

```
2021-01-03 21:50:24
Running ./src/project_name
Run on (16 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 64 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 8192 KiB (x2)
Load Average: 0.74, 0.97, 0.81
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
BM_StringCreation       32.2 ns         32.2 ns     21667369
BM_StringCopy           74.8 ns         74.8 ns      9387404


BM_InitializeNums 1780847955 ns   1780435494 ns          136
BM_Set              38689124 ns     38681053 ns           18
BM_UniqueUnSort      1427650 ns      1427362 ns          514
BM_UniqueSort           1031 ns         1031 ns       658493
```

## Conclusions:
Question everything in your workload, experiment for your usecase, have fun.
Sounds like a good New Year's Resolution anyway!
