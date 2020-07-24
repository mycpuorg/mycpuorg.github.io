---
layout: post
title: "C++ Interface Principle"
excerpt: "Learning a few more quirks of the language"
tags: [C++, Interface, API, Design]
comments: true
---
Interface Principle roughly states that a free non-member function is closely
tied to a class, therefore, part of the class interface if
+ it mentions the class type, for ex: it accepts the class in it's parameter
  list
+ It's shipped along with the class, i.e., it is in the same namespace and the
  header file as the class definition
  
[Keonig's Lookup](https://en.wikipedia.org/wiki/Argument-dependent_name_lookup)
is closely related to this principle.

Source:  [Modern C++ Design](https://www.amazon.com/dp/0201704315/ref=cm_sw_r_tw_dp_x_vVsgFbDY3STFV)
