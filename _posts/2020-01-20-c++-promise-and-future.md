---
layout: post
title: "C++ Promise and Future"
excerpt: "Usage of Promise in C++"
tags: [Linux, Code, C++, Promise, Future, STL, algorithms]
comments: true
---
This post is not about staring into a distant sky ruminating about the future of
``C++``. This is about the namesake features in the language.

If you are a ``C++`` developer I am guessing you are familiar with ``Promises
and Futures`` in C++, supported from C++11.

A ``future`` is an object that provides methods to access the data that needs to be
communicated with a consumer. The semantics of the ``future`` are such that the
access to the object can be done in different threads. The synchronization is
provided by the language/compiler across multiple threads.

In it's common form of usage, ``future`` objects are backed by a shared state
object or an object that is to be shared between producer and consumer.
``future`` is the object that is typically associated with the shared object on
the consumer side.

These ``future`` objects are instantiated in one of the following ways:
```cpp
    async
    promise::get_future
    packaged_task::get_future
```

A ``promise`` is an object that can store the object which is to be shared by
the producer. This shared object is typically retrieved into a ``future``
object. ``promise`` offers a synchronization mechanism on the producer side.
The producer thread stores the shared object into a ``promise`` via the
``::set_value()`` method.

```cpp
	    // instantiate a promise object to share
		// an object of string type
        auto prom = std::promise<std::string>();
        
		// a sample producer thread which sets
		// the shared object via set_value()
        auto producer = std::thread([&]
        {
            prom.set_value("Hello World");
        });
        
		// associate the shared object with a future
		// that can retrieve a value from some provider object,
		// in this case, a promise
        auto future = prom.get_future();
        
		// retrieve the shared object from the future
        auto consumer = std::thread([&]
        {
            std::cout << future.get();
        });
        
		// join threads
        producer.join();
        consumer.join();

```
