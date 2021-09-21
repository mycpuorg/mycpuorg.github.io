---
layout: post
title: Using `std::unordered_map` with custom types
excerpt: "A Common Pitfall"
tags: [Linux, Code, C++, STL, map, unordered_map]
comments: true
---
Using `std::unordered_map` instead of `std::map` gives us the freedom with
choice for the type of keys we can use. While `std::map` expects that there be
an inherent order to the keys which allows for sorting,
`std::unordered_map` allows us to choose our own key types as long as we know
how to hash them.

For example: you can choose any type so long as you know how to hash it.

```cpp
using meeting_time_t = std::pair<int, int>;
```

### Compare Keys
The effect of defining your own hash function to the stdlib is that you also
need to tell how to compare two keys.

```cpp
bool operator== (const meeting_time_t& f, const meeting_time_t& s) {
    return (f.first == s.first and f.second == s.second);
}
```

### Define `hash`
We must sneak our own hash iimplementation into the `std::` namespace while
carefully using the same type names as hash implementation of other/standard key
types.

Note that you must implement the operator `()` where we compute the hash. This
is completely up to the implementation - you can go crazy with crypto secure
hash or keep it lightweight and dummy.

```cpp
namespace std {
    template <>
	struct hash<meeting_time_t> {
	    using argument_type = meeting_time_t;
		using result_type = size_t;
		
		result_type operator() (const argument_type& m) const { return m.first * m.second; }
	};
}
```

Of course, you must implement the operator to reduce the number of hash
collisions.

Hopefully, this gave you an idea that you can do the following:

```cpp
std::unordered_map<meeting_time_t, size_t> meet_map { 
	{{5, 7}, 5}, 
	{{2, 3}, 7} 
	};
for (const auto& [k, v] : meet_map) { std::cout << k.first << k.second << v << std::endl; }
```
