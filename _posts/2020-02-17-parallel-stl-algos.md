---
layout: post
title: "C++-17 Execution Policy in STL"
excerpt: "A Rude Shock"
tags: [Linux, Code, C++, Parallel, C++-17]
comments: true 
---
I came across this
[blog](https://www.bfilipek.com/2018/11/parallel-alg-perf.html) which talks
about about how you could parallelize all the STL algos with a simple argument
passed into them. So I was excited to try this but it disappointed me to such a
large extent that I am scared to go back to trusting anything in the newer C++
versions.

I read through the blog, also went through the chapter dedicated in the
book. Everything about this feature sounded great, even with its minor
caveats. Little did I know about the higher order bit that was not mentioned
clearly.

The feature is *Parallel STL Algorithms* but it is not supported anywhere
outside MSVC. I think that means, it's a Windows only feature?! Then why the
heck is it even mentioned about in a C++-17 book? Is it just because
``Microsoft`` arm twisted the C++ Community to standardize it? I don't even know
who is at fault here? All I know is that the ones losing out are the devs and
the language.
