---
layout: post
title: "Profiling User Space Code with BPFTrace"
excerpt: "Profiling BPF Trace"
tags: [Linux, Code, Perf, Profiling]
comments: true
---
I am excited about ``bpf``. I have been using bcc to profile some fantastic
traces in the Kernel without much of post processing. Consider the list of
examples in the ``bcc`` project under ``tools``. Take the profiler for example:
From the source code comment it is clear what they are achieving.

```
This is an efficient profiler, as stack traces are frequency counted in
kernel context, rather than passing every stack to user space for frequency
counting there. Only the unique stacks and counts are passed to user space
at the end of the profile, greatly reducing the kernel<->user transfer.
```

Using EBPF can be several orders of magnitude, especially when profiling for
rare or infrequent events or occurences. This is primarily because you are
completely bypassing all the post-processing of the captured data in user space
afterwards. In some cases, the quantity of data captured and required to
be pre-processed was high enough to consume significant CPU time.

This is specially bad in embedded environments where the processing capability
is not as high as desktops and servers.

Among other things, I was trying to go through a Java Codebase and instead of
using my Emacs and constructing tags etc. I decided to simply run a couple of
simple usecases/tests and capture the Control Flow Graphs for these runs to
understand the codebase.

```
Tracing method calls in java process 20071... Ctrl-C to quit.
CPU PID    TID    TIME(us) METHOD
...
0   20071  23291  1.047      -> io/netty/util/concurrent/SingleThreadEventExecutorit;)Lio/netty/U.fetchFromScheduledTaskQueue
0   20071  23291  1.047        -> io/netty/util/concurrent/AbstractScheduledEventExecutor.nanoTime
0   20071  23291  1.047          -> io/netty/util/concurrent/ScheduledFutureTaskt;^A.nanoTime
0   20071  23291  1.047            -> java/lang/System.nanoTime
0   20071  23291  1.047            <- java/lang/System.nanoTime
0   20071  23291  1.047          <- io/netty/util/concurrent/ScheduledFutureTaskt;^A.nanoTime
0   20071  23291  1.047        <- io/netty/util/concurrent/AbstractScheduledEventExecutor.nanoTime
0   20071  23291  1.047        -> io/netty/util/concurrent/AbstractScheduledEventExecutor.pollScheduledTask
0   20071  23291  1.047        <- io/netty/util/concurrent/AbstractScheduledEventExecutor.pollScheduledTask
0   20071  23291  1.047      <- io/netty/util/concurrent/SingleThreadEventExecutorit;)Lio/netty/U.fetchFromScheduledTaskQueue

```

The above ``Java`` Call Flow shows from start to finish a function under the
``netty`` module in Java. I have no clue about the function (or even the module
or what it does) From the above flow however, I can tell that the function
``fetchFromScheduledTaskQueue()`` is roughly waiting for a certain amount of
time (or it could simply be checking the current time) and polling for any
scheduled tasks via ``pollScheduledTask()`` and then simply fetch the next
scheduled task from the Task Queue. The price of minor errors in guess is not
high enough to stop and dig in further. The above flow was pretty easy to
capture and here is the single line of command. Now, of course, not all tools
that are possible to be written/expressed in BPF are available in this repo, but
it is a great starting point.

```bash
cd bcc/tools
sudo ./javaflow.sh -l java 20071 | tee /tmp/jf_20071.txt
```

### BPFTrace
Now taking this convenience a step further, you can simply turn this into a CLI
based command. Something like typing ``awk`` commands. This can be achieved with
the powerful ``bpftrace``.
The following command allows us to profile all the userstacks and count them.
```
sudo bpftrace -e 'profile:hz:99 /pid == 20071/ { @[ustack] = count(); }' > /tmp/fg_bp1.txt
```

If you are using this to profile Java Code, you will have to pass the following
arguments to the VM:
```bash
-XX:+PreserveFramePointer -XX:+ExtendedDTraceProbes 
```

#### Sources:
+ [Brendan Gregg's Blog](http://www.breandangregg.com)
+ [IOVISOR Blog](https://github.com/iovisor/bcc#tracing)
+ [Async Java Profiler](https://github.com/jvm-profiling-tools/async-profiler)
