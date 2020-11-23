---
layout: post
title: "Modern C++ Inheritance"
excerpt: "Inheritance is the Base Class of Evil - Sean Parent"
tags: [function, variant, visit]
comments: true
---
## Topics:
### Inheritance Is Not Always Good
Watch Sean Parent's ["Inheritance is the Root of
Evil"](https://www.youtube.com/watch?v=QGcVXgEVMJg&t=1238s) where Sean
eloquently convinces the listener that inheritance can be avoided where
possible. It's also oddly satisfying to watch Sean Parent's code snippets.

<iframe width="560" height="315" src="https://www.youtube.com/embed/QGcVXgEVMJg" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### A modern callback
There are mulitutde of options to mess up the implementation, usually done by,
adding layers upon layers of abstraction disguised as `abstract` classes with
pure `virtual` methods. You can do away with it.

### `std::variant` to the rescue
Diverse unrelated classes/objects can be bunched together for a single purpose
after which these objects are free to go on their own.

### TODO: Needs expansion
Really these are selfishly put up as notes for myself. I will fill these out shortly.
+ Use `std::variant` as frequently as possible.
+ Hide behind `std::any` when possible

