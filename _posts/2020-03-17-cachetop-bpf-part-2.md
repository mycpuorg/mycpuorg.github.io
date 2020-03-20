---
layout: post
title: "Writing Linux File Buffer Cache top in C++ with BPF - Part II"
excerpt: "Use BPF to Write Linux File Buffer Util"
tags: [Linux, Code, Kernel, BPF, C++]
comments: true
---
In an [earlier post](http://www.mycpu.org/cachetop-bpf/) we looked at writing a
utiity to collect Linux File Buffer Cache Statistics in C++.
This is a continuation of the post since the last time we left off at a point
when we attach all the relavant probes.

In this post, we will actually use the statistics obtained from BPF. If we are
displaying the statistics each time in a loop we will need to fetch the
statistics for each iteration of the loop. It is done as shown below:

### Primary Loop
```cpp
do {
	ch = get_user_cmd();
	if (ch == 'q')
	    quit();

    // does all the fetching from BPF
	// access it offline, clear etc.
	auto lines = fetch_bpf_stats(bpf);
	
	// currently I'm using ncurses
	// you know ...
	print_pretty(auto_lines);
} while (again);
```
There's nothing too complex here. Hopefully, this is self explanatory with the comments.

### BPF Util

#### BPF Hash Table
The below code reads from the BPF Hash Table, makes a copy for offline access.
```cpp
lines_t fetch_bpf_stats(ebpf::BPF& b)
{
    lines_t lines;

    auto counts = b.get_hash_table<my_key_t, uint64_t>("counts");
    auto counts_tbl_offline = counts.get_table_offline();
    auto mpa = 0, mbd = 0, apcl = 0, apd = 0;
	
	// ...
```

#### Super Powers of BPF!
Here's where things like the famous "Super Powers" are possible. The BPF
function attached in the kernel returns ``struct pt_regs`` which only provides
the value of register ``IP`` as a part of the returned structure. Now, we need
to somehow convert this to the kernel function name and check which one of the
four kernel functions we have attached the kprobe for. The ``BCC`` library
provides a nice little utility to do just that. It is called ``KSyms`` and it
provides a nice API where you can pass it some valid kernel address and it will
convert this to the corresponding kernel function name and return based on the
name mangling it perfoms from vmlinux/kallsyms. Equivalently, you can turn off
``kptr_restrict`` and read ``/proc/kallsyms`` to see how the sausage is made.

```cpp
    for (auto it : counts_tbl_offline) {
        KSyms ks;
        // need a helper function
        // to resolve an addr to function name
        // this is available in Python
        // needs to be added in C++
        struct bcc_symbol sym;
        ks.resolve_addr(it.first.ip, &sym, true);
        // ...
```

The above piece of code fills up the ``bcc_symbol`` structure and returns with
its function name (and other useful info).

We store the count (of the number of times our attached ``kprobe`` was hit
based on the kernel function name.

```cpp
        if (string{"mark_page_accessed"} == sym.name)
            mpa = it.second;
        if (string{"mark_buffer_dirty"} == sym.name)
            mbd = it.second;
        if (string{"add_to_page_cache_lru"} == sym.name)
            apcl = it.second;
        if (string{"account_page_dirtied"} == sym.name)
            apd = it.second;
        else
            continue;

        // ...
```

#### Cache Stats Calculations
The total hits are the sum of ``mark_page_accessed`` and
``mark_buffer_dirty``. Correspondingly, the total misses are the total from
``add_to_page_cache_lru`` and ``account_page_dirtied``

```cpp
        auto access = mpa + mbd;
        auto misses = apcl + apd;
        auto rtaccess = mpa / (access + misses);
        auto wtaccess = apcl / (access + misses);

        // ...
    }
```

#### Clear the Tables
It is important to clear the BPF table after each iteration so we can
record/collect fresh stats only and keep the BPF monitoring footprint low.

```cpp
    // ...
    counts.clear_table_non_atomic();
    counts_tbl_offline.clear();
    return lines;
}
```

### Demo
```
sudo ./cachetop
=======================================
Chrome_IOThread(13956): access=214       wr_hit%=0       rd_hit%=0       miss=16
jbd2/sda1-8(411): access=11703   wr_hit%=0       rd_hit%=0       miss=126
TaskSchedulerFo(1116): access=550        wr_hit%=0       rd_hit%=0       miss=10
systemd-journal(460): access=127         wr_hit%=0       rd_hit%=0       miss=45
Xorg(10652): access=165          wr_hit%=0       rd_hit%=0       miss=2
TaskSchedulerSi(13973): access=455       wr_hit%=0       rd_hit%=0       miss=2
rp(30853): access=11624          wr_hit%=0       rd_hit%=0       miss=134
rp(30945): access=159    wr_hit%=0       rd_hit%=0       miss=96
rp(30873): access=264    wr_hit%=0       rd_hit%=0       miss=55
sshd(30711): access=1262         wr_hit%=0       rd_hit%=0       miss=2
rs:main Q:Reg(1135): access=264          wr_hit%=0       rd_hit%=0       miss=2
rp(30915): access=1826   wr_hit%=0       rd_hit%=0       miss=103
rp(30843): access=263    wr_hit%=0       rd_hit%=0       miss=125
chrome(13937): access=148        wr_hit%=0       rd_hit%=0       miss=9
rp(30918): access=164    wr_hit%=0       rd_hit%=0       miss=269
CacheThread_Blo(13976): access=11840     wr_hit%=0       rd_hit%=0       miss=4
rp(30861): access=1448   wr_hit%=0       rd_hit%=0       miss=194
=======================================
```
