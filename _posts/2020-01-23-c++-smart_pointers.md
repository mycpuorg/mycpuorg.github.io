---
layout: post
title: "Modern C++ Pointers - Basics"
excerpt: "C++ Pointers"
tags: [Linux, Code, C++, Pointers]
comments: true
---
``C++11`` introduced four different ``Smart Pointers`` because when it came to
pointers, everybody was sick of you whining about one of the following
+ We couldn't figure out if the pointer was pointing to a single object or a
  whole lot of them
+ We didn't use ``const`` while passing these pointers around, there was no clear
  policy implicit in the code, so we could not tell if we could delete an
  object pointed to by the pointer.
+ We kept deleting it multiple times and you wouldn't set it to ``NULL`` after
  deleting it once.
  
### Four Smart Pointers
That's right! to help us from getting out feet stuck in the ceiling fans the
``C++`` language introduced *four* smart pointers:
+ ``std::unique_ptr``
+ ``std::shared_ptr``
+ ``std::weak_ptr``
+ ``std::auto_ptr``

#### ``std::unique_ptr``
They are exactly what they sound like. Such pointers are meant to show that the
memory pointed to by them are exclusively owned by the pointer. Therefore, the
software deleting this memory must do so via this ``unique`` pointer. This is
the most intuitive of the four smart pointers since it is more or less same as a
raw pointer.

#### ``std::shared_ptr``
This is ``C++``'s answer to ``Java``'s long sporadic blocking runs that take up
CPU, usually, when you most need it since that's also the time when the system
is under a resource crunch. Sorry, that was a diss but an accurate one. Platform
Developers and Low Level System Developers on Android Developers will share
their scars about this in their chapter of ``AA`` (Androiders Anonymous?). 

Before Smart Pointing, ``C++`` used to leave the responsibility to the developer
to delete objects. But developers are not adults, we just want our outputs, we
are not deleting stuff! even if we did we would delete it awkwardly enough to
cause troubles for 20 years. Or at least, that seemed to be the
attitude. Basically, ``C++``'s efficiency came from the fact that there were no
training wheels. The developer was always expected to keep track of resources
allocated in their programs. This took sufficient time away from making the
core logic more robust.

But what is ``C++``'s response? It's basically same as what was done earlier
except with basic ref counting implemented in the shared pointer it is possible
to keep track of all active referrers to the object and simply call the supplied
or default constructor. Actually, that's pretty clever! You are getting the best
of both worlds in this case, right? The trade-off is that this smart pointer
requires additional memory for keeping ref count and requires updates to the ref
counter be via atomic operations.

#### ``std::weak_ptr``
If your ``std::shared_ptr`` can dangle after deletion, use
``std::weak_ptr``. Doing supports APIs that can explicitly tell you whether this
is a bad idea or not.

When possible, it is recommended to use ``std::make_unique`` and
``std::make_shared`` instead of ``new``

### Sources:
+ Effective Modern C++
