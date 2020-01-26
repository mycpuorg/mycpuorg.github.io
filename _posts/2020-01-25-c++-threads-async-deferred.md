---
layout: post
title: "C++: Handling Asynchronous Requests using Futures"
excerpt: "Spawn C++ Threads on the Fly or Deferred"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM, Threads, Concurrency, async]
comments: true
---
In an [earlier post](http://www.mycpu.org/c++-promise-and-future/), we have
looked at using modern C++ semantics to synchronize threads. I mentioned that
the ``future`` objects are instantiated in one of the following ways:
```cpp
    async
    promise::get_future
    packaged_task::get_future
```

Today, let's look into the first way a little more. If you are familiar with
``future`` and ``async()`` method, I recommend that you jump below to the
``Run`` section where we capture from
[``ftrace``](http://www.mycpu.org/ftrace-function-graph/) events and
[``perf``](http://www.mycpu.org/perf-events/) events to see if we can catch anything.

## ``async()`` from ``future``
Basically, the ``async()`` method provides a mechanism to call the provided
 function asynchronously. It does in two flavors:

### **Asynchronous**
Launches a new thread to call the given function and returns a ``future``
object. Accessing the _shared state_ of this ``future`` object joins the
thread.

```cpp
    // *try* and flush the caches
    my_mem_flush(stack_start, flush_size);

    // ...

    for ([[maybe_unused]] auto i : {1, 2, 3, 4, 5}) {
        // policy for async() is unspecified
        std::future<bool> fut = std::async(prime_lambda, prime_candidate);
        auto f =
            [&ret, &fut]() -> bool {
			    // gets the result from the future
				// if this accesses the shared state here
				// then this is the equivalent of calling "deferred"
                ret = fut.get();
			    return ret;
            };
        total += timed_lambda(f, "unspecified");
    }
    // print avg of times here
```

### **Deferred**
Calls the provided function at some point returning without waiting for the
execution of the function to complete. The value returned by the function can be
accessed through the future object retuned by the ``async()`` call by calling
it's member ``get()``.

The function temporarily stores in the shared state either the threading handler
used or decay copies of provided function and its args (as a deferred function)
without making it ready. Once the execution of provided function is completed,
the shared state contains the value returned by provided function and is made
ready.

```cpp
    std::future<bool> def_fut = std::async(launch::deferred,
                       prime_lambda, prime_candidate);

    // ... Lot of things can happen here with no concurrent computation on going

    // *try* and flush the cache to get unbiased perf numbers
    auto foo_cache_flush = [&stack_start, &flush_size]() {
                my_mem_flush(stack_start, flush_size);
                };

    //
    timed_lambda(foo_cache_flush, "cacheflush");

    auto foo_deferred_fut_get = [&ret, &def_fut]() {
                    ret = def_fut.get();
                    };
    auto tt = timed_lambda(foo_deferred_fut_get, "deferred");
    std::cout << "deferred took " << tt << "us on avg" << std::endl;
```


### What is the ``task`` function?
The obvious question is what is the actual workload function of the created
thread? In the above code, to appease the impatient reader with the semantics of
the core of this article I waited until now.  it so the
meaning is obvious. The function is to determine the primality of some number.
We select a large prime number and purposely write the prime checker to be
inefficient.

```cpp
    auto prime_candidate = 313222313;

    // purposely slow primality lambda
    auto prime_lambda = [](auto x) {
                std::cerr << "Wait ..." << x << std::endl;
                for (int i = 2; i < x; ++i) if ((x % i) == 0) return false;
                return true;
            };
```

### Lambda to measure time taken
If not for anything else, I want to record my lambda to simply measure the time
taken by a given piece of execution (this could be a Lambda too!).
```cpp
    auto timed_lambda = [](auto lam, auto mod_name) -> long {
                auto before = std::chrono::steady_clock::now();
                lam();
                auto after = std::chrono::steady_clock::now();

				// pretty prints the time take and the module name
                auto t = print_ts(string(mod_name), before, after);
                return t;
            };

```

### Flush Cache
I tried to flush cache for unbiased perf counter measurements and my (poor)
attempt at doing so is:
```cpp
void my_mem_flush(const void *p, unsigned int allocation_size){
    const size_t cache_line = 64;
    const char *cp = (const char *)p;
    size_t i = 0;

    if (p == NULL || allocation_size <= 0)
        return;

    for (i = 0; i < allocation_size; i += cache_line) {
        asm volatile("clflush (%0)\n\t"
                :
                : "r"(&cp[i])
                : "memory");
    }

    asm volatile("sfence\n\t"
                 :
                 :
                 : "memory");
}
```



## Build
The commands used for building:
```bash
clang++ -fno-omit-frame-pointer -o mfa ./my_future_async.cpp -lpthread
```

## Run
There's some magic here showing the nuances of how to efficiently measure the
performance of your program. Here, I'm capturing

```bash
sudo /home/manoj/bin/perf stat -e "sched:sched_process*,task:*,L1-dcache-loads,L1-dcache-load-misses,cycles" -d -d -d ./mfa 2
```

### Perf Results for ``Deferred`` ``async()`` Call
```bash
===========================
unspecified: ran for 1057395us
===========================
cacheflush: ran for 0us
===========================
deferred: ran for 1072166us
===========================
It is prime!

 Performance counter stats for './mfa 2' (5 runs):

                 2      sched:sched_process_free                                      ( +- 36.85% )
                 6      sched:sched_process_exit
                 0      sched:sched_process_wait
                 5      sched:sched_process_fork
                 1      sched:sched_process_exec
                 0      sched:sched_process_hang
                 5      task:task_newtask
                 1      task:task_rename
    11,330,199,345      L1-dcache-loads                                               ( +-  0.06% )  (49.92%)
           833,099      L1-dcache-load-misses     #    0.01% of all L1-dcache hits    ( +-  4.43% )  (50.09%)
    27,403,478,438      cycles                                                        ( +-  0.06% )  (50.15%)
    11,268,197,415      L1-dcache-loads                                               ( +-  0.06% )  (50.16%)
           839,110      L1-dcache-load-misses     #    0.01% of all L1-dcache hits    ( +-  4.64% )  (50.17%)
   <not supported>      LLC-loads
   <not supported>      LLC-load-misses
         8,183,188      L1-icache-loads                                               ( +-  5.49% )  (50.19%)
           489,139      L1-icache-load-misses     #    5.98% of all L1-icache hits    ( +-  6.70% )  (50.08%)
            37,963      dTLB-loads                                                    ( +- 23.22% )  (49.91%)
             5,762      dTLB-load-misses          #   15.18% of all dTLB cache hits   ( +- 17.93% )  (49.85%)
             1,483      iTLB-loads                                                    ( +- 49.34% )  (49.84%)
               520      iTLB-load-misses          #   35.05% of all iTLB cache hits   ( +- 26.77% )  (49.83%)
           130,655      L1-dcache-prefetches                                          ( +- 13.49% )  (49.81%)
   <not supported>      L1-dcache-prefetch-misses

           6.36034 +- 0.00639 seconds time elapsed  ( +-  0.10% )


```

### Perf Results for ``Unspecified`` ``async()`` Call
```bash
===========================
unspecified: ran for 1054048us
===========================
It is prime!

 Performance counter stats for './mfa' (5 runs):

                 1      sched:sched_process_free                                      ( +- 48.59% )
                 6      sched:sched_process_exit
                 0      sched:sched_process_wait
                 5      sched:sched_process_fork
                 1      sched:sched_process_exec
                 0      sched:sched_process_hang
                 5      task:task_newtask
                 1      task:task_rename
     9,435,133,920      L1-dcache-loads                                               ( +-  0.07% )  (49.88%)
           730,119      L1-dcache-load-misses     #    0.01% of all L1-dcache hits    ( +-  1.61% )  (50.07%)
    22,815,220,431      cycles                                                        ( +-  0.08% )  (50.15%)
     9,384,105,495      L1-dcache-loads                                               ( +-  0.08% )  (50.19%)
           736,150      L1-dcache-load-misses     #    0.01% of all L1-dcache hits    ( +-  1.59% )  (50.21%)
   <not supported>      LLC-loads
   <not supported>      LLC-load-misses
         6,803,493      L1-icache-loads                                               ( +-  0.98% )  (50.23%)
           383,948      L1-icache-load-misses     #    5.64% of all L1-icache hits    ( +-  3.07% )  (50.12%)
            26,848      dTLB-loads                                                    ( +- 11.26% )  (49.93%)
             4,331      dTLB-load-misses          #   16.13% of all dTLB cache hits   ( +- 10.54% )  (49.85%)
               842      iTLB-loads                                                    ( +- 40.64% )  (49.81%)
               205      iTLB-load-misses          #   24.34% of all iTLB cache hits   ( +- 20.22% )  (49.79%)
            90,556      L1-dcache-prefetches                                          ( +-  5.35% )  (49.77%)
   <not supported>      L1-dcache-prefetch-misses

           5.30487 +- 0.00630 seconds time elapsed  ( +-  0.12% )


```

[Full Code Here:](/images/my_future_async.cpp)
