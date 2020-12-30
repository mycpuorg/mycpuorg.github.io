---
layout: post
title: "Find the *est Window"
excerpt: "A Simple Pattern for A Class of Puzzles/Problems"
tags: [C++, lambda]
comments: true
---
The title of this post should describe what I imply here. I came across a class
of puzzles which fit a mould of solution.

**Note:** This is only for contiguous subgroups not the same as interleaved
subsequences.

```cpp
std::vector<int> arr = {1, 1, 2, 3, 2, 0, 4, 0, 5, 0, 6};
std::vector<int> sub_ = {1, 2, 3}; // not {1, 2, 3, 4, 5, 6} which is non-contig
```

## Find the Longest/Shortest Sliding Window:
This problem occurs in real life far more often than expected. All the way from
network protocol implementation to pattern matching to generic book keeping
tasks.

## Basic Building Blocks:
The basic building blocks for solving such problems are that you need a left and
a right iterator which form the window. The size of this window is determined by
which direction the iterators move in.

## Window Resizing:
For most cases, the input array (or a string) already dictates an in-built
ordering `(L -> R)`. As a convention, we extend the window by moving the right
iterator and shrink it by moving the left.

## Goal: Consume input only once, O(N)
By going over the input once, we guarantee that the worst case is
`O(N)`. However, this does require us to carefully manage the boundaries of the
window.


### Code Snippet:
Assume the problem is to find the longest window of increasing numbers in a
given array (Common interview problem, perhaps).

```cpp
// shown once here for clarity
struct max_sub {
    size_t max_len;
    std::pair <int, int> sub_idx;
    max_sub() : max_len(0) {}
};

using sub_arr_t = struct max_sub;

template <typename T>
auto longest_inc_sub(const std::vector<T>& arr) -> sub_arr_t {
    // init stack vars here ...

    auto update_global_max_ = [&max_] (int l, int r) {
        auto local_ = r - l + 1;
        if (local_ > max_.max_len) { max_.max_len = local_; max_.sub_idx = std::make_pair(l, r); }
    };

    while (r < arr.size() && (l <= r)) {
	    while (arr[l] < arr[r]) { r++; }
	    update_global_max_(l, r);
	    l++;
    }

    return max_;
}

// ... Caller

auto incr_subarr = longest_inc_sub(input_arr);

```

### Core Logic:
The magic all happens in this bit of the code, which simply extends the window
to the right if the condition is satisfied. If the condition check fails, then
the left iterator is advanced (or the window is shrunk) for the new input to be
consumed.

```cpp
    while (r < arr.size() && (l <= r)) {
	    while (arr[l] < arr[r]) { r++; }
	    update_global_max_(l, r);
	    l++;
    }
```

### Generic Code:
Now let's try to make this code apply to a broader set of problems that require
a sliding window run over the input. The code becomes:

```cpp
    
template <typename T, typename Pred>
auto sliding_win_(const std::vector<T>& arr, Pred& p, MinMax& u) -> sub_arr_t {
    // init stack vars here

    while (r < arr.size() && (l <= r)) {
	    while (p(arr[l], arr[r])) { r++; }
	    u(max_, l, r);
	    l++;
    }

    return max_;
}

// ...
// Caller

sliding_win_(input_arr, 
        [=](const auto& l, const auto& r) -> bool { return (l < r); }, 
        [=](auto& book_keeper, const auto& l, const auto& r) -> bool {
			auto local_ = r - l + 1;
			book_keeper.update_min(l, r);
			// OR book_keeper.update_max(l, r);
		);

```

As you can see, the control for the logic now rests with the caller who can
invoke this generic sliding window algorithm in a powerful, succinct manner.
