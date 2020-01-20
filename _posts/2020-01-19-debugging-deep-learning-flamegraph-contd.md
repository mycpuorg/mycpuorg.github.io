---
layout: post
title: "Useful Marriage of Deep Learning and Systems Profilers"
excerpt: "Profiling with Perf and Flamegraphs (Contd.)"
tags: [Linux, Code, C++, Perf, Profiling, Deep Learning]
comments: true
---
In an [earlier post](http://www.mycpu.org/debugging-deep-learning-flamegraph/) I
explored if capturing profiling info from a low level systems engg perspective
is even remotely useful. The result was that if the tools are coperative then it
is worthwhile capturing profiling info from the computational device / element
perspective. Looking into the call graph from the framework implementation gives
a ton of context on how the framework implements Deep Learning
operators. Clearly, profiling as an aid to debugging is not a new idea (at
all). Most Deep Learning frameworks have their own rich set of profiling tools
and libraries. Refer below for sample outputs from ``MXNet``. However, the
biggest utility of looking into such info is what it provides in addition to the
framework specific features.

#### MXNet Profiler Output

![Profiler Tracing Output](https://raw.githubusercontent.com/dmlc/web-data/master/mxnet/tutorials/python/profiler/profiler_output_chrome.png)

![Profiler Stats Output](https://raw.githubusercontent.com/dmlc/web-data/master/mxnet/tutorials/python/profiler/profile_stats.png)

From the above output it should be clear that obtaining ``Operator`` level
information should be fairly easy if you followed steps to use the [MXNet
Profiler](https://mxnet.incubator.apache.org/api/python/docs/tutorials/performance/backend/profiler.html)
in your Deep Learning Model.

### Is it even useful?
So, is it even useful to capture profiling info from tools like ``perf`` to
debug bugs and bottlenecks in the model itself? I found the answer is it may not
be terribly useful in obtaining contextual information or it may not help you
tune hyper parameters to make the training more effective, it does help make the
training itself a bit more efficient.

If you are curious about learning regarding the implementation of
``FullyConnected`` Operator in ``MXNet`` it is useful to look the stack trace
and compare the run times between different implementations. ``MXNet`` offers a
bunch of options to run on ``ctx<gpu>()`` or ``ctx<cpu>()``. Even when run on
CPU, there are couple of options you build ``MXNet`` with. It could be Intel's
``MKLDNN`` library (separate installation) or OpenBLAS which is the more general
open source library of Linear Algebra Programs that are optimized in software
(not hardware target based).

![Fully Connected on CPU with MKLDNN library](/images/fc_exec_on_cpu.png)
Once you reach the top of the ![graph](http://www.mycpu.org/images/mlp_cpu.svg) you will start to see calls to
``jit_avx_gemm_f32`` which is a call to Intel's Hardware that provides
acceleration for ``MKL`` or ``Math Kernel Library``. The source code can be
found [here](https://github.com/intel/mkl-dnn/blob/master/src/cpu/gemm/f32/jit_avx_gemm_f32.cpp)

#### Bonus: Off-CPU Flamegraph for MLP

<div
style="float:right;padding-left:30px;padding-right:10px;padding-bottom:3px"><a
href="/images/mlp_off_cpu.svg"><img
src="/images/mlp_off_cpu_preview.png" width="600" height="500"
style="padding-bottom:3px"/></a><br><center><i>Off-CPU Flame Graphs for a Multi Layer Perceptron</i></center></div>



