---
layout: post
title: "Useful STL Algos - Cheat Sheet"
excerpt: "stdlib C++ Cheat Sheet"
tags: [C++, STL, Algos]
comments: true
---
Come On! `C++` STL contains stupefyingly useful algorithms.
I will just arbitrarily list them here for reference (not comprehensive by any
means).

## List of Useful `STL` Algorithms

+ ``std::find_if``:
```cpp
// find odd
auto iter = std::find::if(v.begin(), v.end(), [](const auto& e_) { return (e_ % 2); }
```

+ ``std::transform``:
```cpp
std::string s1 {"BLAH"};
std::string s2;
std::transform(s1.begin(), s1.end(), s2.begin(), std::tolower);
```

+ ``std::stable_partition``:
```cpp
// pivot_ is an iterator with prime nos. before and composites after
auto pivot_ = std::stable_partition(v.begin(), v.end(), [](const auto& e_) { return is_prime(e_); });
```

+ ``std::rotate``:
```cpp
auto mid_ = v.begin() + 5;
std::rotate(v.begin(), mid, v.end());
```

+ ``std::is_permutation``:
```cpp
std::is_permutation(v1.begin(), v.begin() + 5, v2.begin());
```

+ ``std::next_permutation``:
```cpp
bool next_possible_ = std::next_permutation(v1.begin(), v1.begin() + 11);
if (next_possible_) {
	// v1 through 11 eles are rearranged based on the next avail permutation
}
```

+ ``std::lower_bound``:
```cpp
auto iter = std::lower_bound(v.begin(), v.end(), 50);
```

+ ``std::move_backward``:
```cpp
std::move_backward(v.begin(), v.begin() + 4, v.begin() + 5);
// [1, 2, 3, 4] => [_, 1, 2, 3, 4]
```

+ ``std::any_of``:
```cpp
bool is_found = std::any_of(v.begin(), v.end(), [](const auto& e_) { return (is_condition(e_)); });
```

+ ``std::lexicographical_compare``:
```cpp
std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), [](const
    auto& ch1, const auto& ch2) { return (std::tolower(ch1) < std::tolower(ch2));});
```

+ ``std::adjacent_find``:
This snippet finds words that are out of dictionary order
```cpp
auto iter = std::adjacent_find(words.begin(), words.end(), [=](const string& w1,
const string& w2) {
    return std::lexicographical_compare(w1.begin(), w1.end(), w2.begin(),
	w2.end(), [](char c1, char c2) { return (!(c1 < c2)); });
});
```

+ ``std::is_permutation``:
```cpp
bool is_perm = std::is_permutation(str1.begin(), str1.end(), str2.begin());
```

+ ``std::count_if``:
```cpp
int count_odds_ = std::count_if(v.begin(), v.end(), 
	[](const int e_) { return ((e_ % 2) == 1); });
```

+ ``std::for_each``:
```cpp
std::for_each(v.begin(), v.end(), [](auto& e_) { do_foo(e_); });
```

+ ``std::equal``:
```cpp
std::equal(v.begin(), v.end(), [](const atuo& e1, const auto& e2) { return foo(e1) == foo(e2); });
```

+ ``std::set_intersection``:
```cpp
auto iter = std::set_intersection(v1.begin(), v2.end, v2.begin(), v2.end(), std::back_inserter(v_intersection));
```

+ ``std::binary_search``:
```cpp
bool found_ = std::binary_search(sorted_v.begin(), sorted_v.end(), e, 
                                  [](const auto& i, const auto& j) { return is_sorted_order(i, j); });
```

+ ``std::unique``:
```cpp
auto iter = std::unique(v.begin(), v.end(),
                    [&repeats](auto& i, auto& j) {
					    if (i == j) { repeats++; return false; }
						return true;
                    });
// useful in merging adjacent/consecutive equal entries
// like merging intervals, even.
```

+ Merging Vectors: ``std::move``:
```cpp
// v1 is the final vector
// v2 is the arr to be merged/deleted
std::move(v1.begin(), v1.end(), std::back_inserter(v2));
```

+ ``std::priority_queue``:
```cpp
// comp is a predicate to compare
// only it's type is passed here
// Note: A priority queue cannot be iterated over
std::priority_queue<T, std::vector<T>, decltype(comp)> pq_;
```
