---
layout: post
title: "So... You wanna measure Branch Prediction Hit Rate with BPF and C++?"
excerpt: "A walk-through of my understanding of the BCC"
tags: [Linux, Code, C++, BPF, eBPF, BCC, tracing, LLVM]
comments: true
---
```
Disclaimer: [Brendan Gregg](http://www.brendangregg.com) is a leading expert on
this topic. This post is an attempt to try and simplify things for a mortal like
myself. If you want authoritative content on anything BCC, BPF, or tracing
please visit Brendan's site.
```
### Why is BCC?
Writing BPF Programs can be hard, so we needed a toolkit that makes writing them
easier, at least, to a certain extent.

### What is BCC?
[BCC](https://github.com/iovisor/bcc) is a toolkit for creating frontend
programs that can efficiently perform Kernel and User Level tracing. It comes
with several useful tools and examples. BCC makes writing BPF programs
less painful (and includes a C wrapper around LLVM), and front-ends in Python, Lua,
and C++

#### But ... Perf?
With ``perf`` you can [Debug
Applications](http://www.mycpu.org/flamegraphs-on-c++/) or [capture events for
the fun of it](http://www.mycpu.org/perf-events/). However, it is a sampling
profiler which collects events periodically. It then "estimates" the system
performance statistics based on collected samples and Hardware Based Performance
Monitoring Counters. Perf (and any sampling profiler) can add non-trivial
amounts of overhead, and we are not even talking about compute spent
post-processing the captured samples.

### Pre-requisites
For you to walk through the examples, you will need to [install BCC
tools](https://github.com/iovisor/bcc/blob/master/INSTALL.md). To find out the
list of Hardware Events supported in your machine you could run ``perf list``
and you should see something like the output at the end of this page
copied verbatim from my machine (search for 'COLLAPSE')

To be able to successfully run the Branch Prediction example illustrated here
make sure you see the support for below two hardware events:
```
  branch-instructions OR branches                    [Hardware event]
  branch-misses                                      [Hardware event]
```

I am running a spanking new Linux Kernel I built and [you can too, if you have an
hour](http://www.mycpu.org/kernel-n00b-howto/).
```bash
╭─ ~/s/bcc/bcc/build   master ● ? ⍟1                                                                                      ✔  26.99G RAM  0.20 L
╰─ cat /proc/version 
Linux version 5.5.0-rc4+ (manoj@manoj-desktop) (gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1)) #1 SMP Fri Jan 3 00:01:03 PST 2020
```

I also compiled the perf binary from the same Kernel Tree since perf requires
you to install the Kernel headers for the same version.

### Show Me The Code!
A BCC program tends to have a common structure which is mostly along the lines
of:
+ Define the BPF Program which is going to be run by eBPF in the Kernel
+ Attach the Perf Event or Probe in the Kernel
+ Run whatever code you wanted to profile
+ Detach the Perf Event or Probe
+ Process the Result

The Program we are going to walk through is similar to the ``LLCStat`` which is
present in the BCC Project but does not seem to run on any hardware that I tried
on. However, I thought it would still be interesting enough to profile the
Branch Prediction (or MisPrediction) Rate on all the cores and all PIDs running
in a specified period of time.

#### Define BPF Program
```c
const std::string BPF_PROGRAM = R"(
#include <linux/ptrace.h>
#include <uapi/linux/bpf_perf_event.h>

struct event_t {
    int cpu;
    int pid;
    char name[16];
};

BPF_HASH(ref_count, struct event_t);
BPF_HASH(miss_count, struct event_t);

static inline __attribute__((always_inline)) void get_key(struct event_t* key) {
    key->cpu = bpf_get_smp_processor_id();
    key->pid = bpf_get_current_pid_tgid();
    bpf_get_current_comm(&(key->name), sizeof(key->name));
}

int on_branch_miss(struct bpf_perf_event_data *ctx) {
    struct event_t key = {};
    get_key(&key);

    u64 zero = 0, *val;
    val = miss_count.lookup_or_try_init(&key, &zero);
    if (val) {
        (*val) += ctx->sample_period;
    }

    return 0;
}

int on_branch_ref(struct bpf_perf_event_data *ctx) {
    struct event_t key = {};
    get_key(&key);

    u64 zero = 0, *val;
    val = ref_count.lookup_or_try_init(&key, &zero);
    if (val) {
        (*val) += ctx->sample_period;
    }

    return 0;
}
)";
```
The gist of the above code is that we define two data structures - both BPF Hash
Tables. They are Hash Tables in the Kernel, however, it is not important to know
exactly how they are used. We just need to know that they basically function
like Hash Tables. Their Key is of type ``struct event_t`` and they store a value
of ``uint64_t`` type. In addition, there are a couple of functions defined which
seem to look up some sort of ``key`` which is of type ``struct event_t``. You
have probably connected the dots here :)

But who's calling these functions? And why are they enclosed in double quotes
and stored like ``String`` objects?

#### Init and Attach Events:
```cpp
ebpf::BPF bpf;
auto init_res = bpf.init(BPF_PROGRAM);
// ...

auto attach_ref_res =
bpf.attach_perf_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
		"on_branch_ref", 100, 0);

// ...

auto attach_miss_res = bpf.attach_perf_event(PERF_TYPE_HARDWARE,
PERF_COUNT_HW_BRANCH_MISSES, "on_branch_miss", 100, 0);
// ...

```
``BPF`` is the Class defined by the BCC toolkit which provides a lot of
utilities for completing BPF related tasks effectively and efficiently. This is
the biggest value of BCC framework in my opinion. We initialize this object
``bpf`` with the BPF Program which we wrote above, this code is going to be run
in the Kernel's BPF JIT VM and it is injected by the ``BPF`` object. __"How
exactly?"__ you ask, through the ``attach_perf_event()`` API. If you notice, the
string literals ``on_branch_ref`` and ``on_branch_miss`` passed as an arg to
this API are the same as the two functions within the BPF Program. So, in
effect, we are attaching two perf events and passing the callbacks as the
functions defined within the BPF program (again, which runs in the Kernel on
each event. What are these *events* that we talk about? They are
``PERF_COUNT_HW_BRANCH_INSTRUCTIONS`` and ``PERF_COUNT_HW_BRANCH_MISSES`` both
are definied in the ``linux/perf_events.h`` header file included in this
program. Note that we are specifying that these two events are of the type
``PERF_TYPE_HARDWARE`` so the Kernel knows to fetch the values for these
requests from the PMC Registers just like it does when ``perf`` utlitiy requests
for these numbers. So we registered our handlers in the BPF program for the
events we are interested in.

#### Run the code to profile
In this case, we are simply capturing all the branch instructions and branch
misses throughout a specified duration (note the ``sleep()`` for an amount of
time.

#### Detach our handlers
After the specified amout of time is finished, we detach our handlers by
calling:
```cpp
bpf.detach_perf_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
bpf.detach_perf_event(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES);
```

#### Process the output
If our program has reached until this point then most hurdles associated with
running BPF programs have been crossed. We only need to make sure the output is
meaningful or even there. In this example, we don't automate the processing of
this output, instead we simply pretty-print the stats on stdout.

```cpp
auto instrns = bpf.get_hash_table<event_t, uint64_t>("ref_count");
auto misses = bpf.get_hash_table<event_t, uint64_t>("miss_count");
for (auto it : instrns.get_table_offline()) {
    uint64_t hit;
    try {
    	auto miss = misses[it.first];
    	hit = miss <= it.second ? it.second - miss : 0;
    } catch (...) {
    	hit = it.second;
    }
    double ratio = (double(hit) / double(it.second)) * 100.0;

// ...

// pretty-print the results
```

In the above code, we are retrieving the hash tables via BPF's
``get_hash_table()`` API. Note that the arg passed to this API contains the
string literal which were the names of the hash tables in our BPF program which
was attached earlier. Now we iterate through the hash tables and calculate the
Branch Hit Ratio and Branch Miss Ratio from the given counters.

The complete code can be downloaded from [here](/images/BranchPrediction.cc)

The output from the above program on my machine as-is is:
```bash
╭─ ~/s/bcc/bcc/build   master ● ? ⍟1                                                                                      ✔  26.89G RAM  0.26 L 
╰─ sudo ./examples/cpp/BranchPrediction 1
Probing for 1 seconds
PID    27835 (tmux: server)     on CPU  4 Hit Rate 58.82% (12000/20400)
PID        0 (swapper/6)        on CPU  6 Hit Rate 96.15% (274600/285600)
PID     4935 (tmux: server)     on CPU  4 Hit Rate 0% (0/20400)
PID     4935 (tmux: server)     on CPU 15 Hit Rate 0% (0/20400)
PID    27840 (python3)          on CPU  2 Hit Rate 0.4902% (400/81600)
PID        0 (swapper/2)        on CPU  2 Hit Rate 95.93% (450100/469200)
PID    27980 (kworker/4:5)      on CPU  4 Hit Rate 88.73% (18100/20400)
PID        0 (swapper/15)       on CPU 15 Hit Rate 96.28% (432100/448800)
PID     4711 (gnome-terminal-)  on CPU 15 Hit Rate 2.451% (500/20400)
PID        0 (swapper/13)       on CPU 13 Hit Rate 95.68% (527000/550800)
PID    20674 (firefox)          on CPU  8 Hit Rate 83.82% (17100/20400)
PID    20856 (WebExtensions)    on CPU  4 Hit Rate 55.39% (11300/20400)
PID        0 (swapper/4)        on CPU  4 Hit Rate 95.48% (448000/469200)
PID    27836 (byobu-status)     on CPU  6 Hit Rate 0% (0/20400)
PID    27850 (mv)               on CPU  0 Hit Rate 5.392% (1100/20400)
PID        0 (swapper/8)        on CPU  8 Hit Rate 89.12% (90900/102000)
PID    20680 (Gecko_IOThread)   on CPU  1 Hit Rate 14.71% (3000/20400)
PID    28632 (kworker/5:1)      on CPU  5 Hit Rate 88.97% (36300/40800)
PID    14423 (Web Content)      on CPU  8 Hit Rate 0% (0/20400)
PID        0 (swapper/3)        on CPU  3 Hit Rate 89.57% (127900/142800)
PID    27817 (BranchPredictio)  on CPU 12 Hit Rate 94.12% (38400/40800)
PID        0 (swapper/14)       on CPU 14 Hit Rate 91.93% (806400/877200)
PID        0 (swapper/1)        on CPU  1 Hit Rate 93.34% (457000/489600)
PID        0 (swapper/7)        on CPU  7 Hit Rate 95.59% (468000/489600)
PID    27836 (byobu-status)     on CPU  1 Hit Rate 0% (0/20400)

#...

```

Happy Whatever!

#### Source:
+ http://www.brendangregg.com
+ https://github.com/iovisor/bcc
+ http://www.mycpu.org/kernel-n00b-howto/
+ http://www.mycpu.org/flamegraphs-on-c++/
+ http://www.mycpu.org/perf-events/

<details><summary><b>CLICK COLLAPSE/UNCOLLAPSE FULL PERF LIST</b></summary>
<p>

branch-instructions OR branches                    [Hardware event]
branch-misses                                      [Hardware event]
cache-references                                   [Hardware event]
cpu-cycles OR cycles                               [Hardware event]
instructions                                       [Hardware event]
stalled-cycles-backend OR idle-cycles-backend      [Hardware event]
stalled-cycles-frontend OR idle-cycles-frontend    [Hardware event]
alignment-faults                                   [Software event]
bpf-output                                         [Software event]
context-switches OR cs                             [Software event]
cpu-clock                                          [Software event]
cpu-migrations OR migrations                       [Software event]
dummy                                              [Software event]
emulation-faults                                   [Software event]
major-faults                                       [Software event]
minor-faults                                       [Software event]
page-faults OR faults                              [Software event]
task-clock                                         [Software event]
duration_time                                      [Tool event]
L1-dcache-load-misses                              [Hardware cache event]
L1-dcache-loads                                    [Hardware cache event]
L1-dcache-prefetches                               [Hardware cache event]
L1-icache-load-misses                              [Hardware cache event]
L1-icache-loads                                    [Hardware cache event]
branch-load-misses                                 [Hardware cache event]
branch-loads                                       [Hardware cache event]
dTLB-load-misses                                   [Hardware cache event]
dTLB-loads                                         [Hardware cache event]
iTLB-load-misses                                   [Hardware cache event]
iTLB-loads                                         [Hardware cache event]
amd_iommu_0/cmd_processed/                         [Kernel PMU event]
amd_iommu_0/cmd_processed_inv/                     [Kernel PMU event]
amd_iommu_0/ign_rd_wr_mmio_1ff8h/                  [Kernel PMU event]
amd_iommu_0/int_dte_hit/                           [Kernel PMU event]
amd_iommu_0/int_dte_mis/                           [Kernel PMU event]
amd_iommu_0/mem_dte_hit/                           [Kernel PMU event]
amd_iommu_0/mem_dte_mis/                           [Kernel PMU event]
amd_iommu_0/mem_iommu_tlb_pde_hit/                 [Kernel PMU event]
amd_iommu_0/mem_iommu_tlb_pde_mis/                 [Kernel PMU event]
amd_iommu_0/mem_iommu_tlb_pte_hit/                 [Kernel PMU event]
amd_iommu_0/mem_iommu_tlb_pte_mis/                 [Kernel PMU event]
amd_iommu_0/mem_pass_excl/                         [Kernel PMU event]
amd_iommu_0/mem_pass_pretrans/                     [Kernel PMU event]
amd_iommu_0/mem_pass_untrans/                      [Kernel PMU event]
amd_iommu_0/mem_target_abort/                      [Kernel PMU event]
amd_iommu_0/mem_trans_total/                       [Kernel PMU event]
amd_iommu_0/page_tbl_read_gst/                     [Kernel PMU event]
amd_iommu_0/page_tbl_read_nst/                     [Kernel PMU event]
amd_iommu_0/page_tbl_read_tot/                     [Kernel PMU event]
amd_iommu_0/smi_blk/                               [Kernel PMU event]
amd_iommu_0/smi_recv/                              [Kernel PMU event]
amd_iommu_0/tlb_inv/                               [Kernel PMU event]
amd_iommu_0/vapic_int_guest/                       [Kernel PMU event]
amd_iommu_0/vapic_int_non_guest/                   [Kernel PMU event]
branch-instructions OR cpu/branch-instructions/    [Kernel PMU event]
branch-misses OR cpu/branch-misses/                [Kernel PMU event]
cache-references OR cpu/cache-references/          [Kernel PMU event]
cpu-cycles OR cpu/cpu-cycles/                      [Kernel PMU event]
instructions OR cpu/instructions/                  [Kernel PMU event]
msr/aperf/                                         [Kernel PMU event]
msr/irperf/                                        [Kernel PMU event]
msr/mperf/                                         [Kernel PMU event]
msr/tsc/                                           [Kernel PMU event]
stalled-cycles-backend OR cpu/stalled-cycles-backend/ [Kernel PMU event]
stalled-cycles-frontend OR cpu/stalled-cycles-frontend/ [Kernel PMU event]

branch:
bp_l1_btb_correct                                 
	[L1 BTB Correction]
bp_l2_btb_correct                                 
	[L2 BTB Correction]

cache:
bp_l1_tlb_miss_l2_hit                             
	[The number of instruction fetches that miss in the L1 ITLB but hit in
	the L2 ITLB]
bp_l1_tlb_miss_l2_miss                            
	[The number of instruction fetches that miss in both the L1 and L2 TLBs]
bp_snp_re_sync                                    
	[The number of pipeline restarts caused by invalidating probes that hit
	on the instruction stream currently being executed. This would happen
	if the active instruction stream was being modified by another
	processor in an MP system - typically a highly unlikely event]
bp_tlb_rel                                        
	[The number of ITLB reload requests]
ic_cache_fill_l2                                  
	[The number of 64 byte instruction cache line was fulfilled from the L2
	cache]
ic_cache_fill_sys                                 
	[The number of 64 byte instruction cache line fulfilled from system
	memory or another cache]
ic_cache_inval.fill_invalidated                   
	[IC line invalidated due to overwriting fill response]
ic_cache_inval.l2_invalidating_probe              
	[IC line invalidated due to L2 invalidating probe (external or LS)]
ic_fetch_stall.ic_stall_any                       
	[IC pipe was stalled during this clock cycle for any reason (nothing
	valid in pipe ICM1)]
ic_fetch_stall.ic_stall_back_pressure             
	[IC pipe was stalled during this clock cycle (including IC to OC
	fetches) due to back-pressure]
ic_fetch_stall.ic_stall_dq_empty                  
	[IC pipe was stalled during this clock cycle (including IC to OC
	fetches) due to DQ empty]
ic_fw32                                           
	[The number of 32B fetch windows transferred from IC pipe to DE
	instruction decoder (includes non-cacheable and cacheable fill
	responses)]
ic_fw32_miss                                      
	[The number of 32B fetch windows tried to read the L1 IC and missed in
	the full tag]
l2_cache_req_stat.ic_fill_hit_s                   
	[IC Fill Hit Shared]
l2_cache_req_stat.ic_fill_hit_x                   
	[IC Fill Hit Exclusive Stale]
l2_cache_req_stat.ic_fill_miss                    
	[IC Fill Miss]
l2_cache_req_stat.ls_rd_blk_c                     
	[LS Read Block C S L X Change to X Miss]
l2_cache_req_stat.ls_rd_blk_cs                    
	[LS ReadBlock C/S Hit]
l2_cache_req_stat.ls_rd_blk_l_hit_s               
	[LsRdBlkL Hit Shared]
l2_cache_req_stat.ls_rd_blk_l_hit_x               
	[LS Read Block L Hit X]
l2_cache_req_stat.ls_rd_blk_x                     
	[LsRdBlkX/ChgToX Hit X. Count RdBlkX finding Shared as a Miss]
l2_fill_pending.l2_fill_busy                      
	[Total cycles spent with one or more fill requests in flight from L2]
l2_latency.l2_cycles_waiting_on_fills             
	[Total cycles spent waiting for L2 fills to complete from L3 or memory,
	divided by four. Event counts are for both threads. To calculate
	average latency, the number of fills from both threads must be used]
l2_request_g1.cacheable_ic_read                   
	[Requests to L2 Group1]
l2_request_g1.change_to_x                         
	[Requests to L2 Group1]
l2_request_g1.l2_hw_pf                            
	[Requests to L2 Group1]
l2_request_g1.ls_rd_blk_c_s                       
	[Requests to L2 Group1]
l2_request_g1.other_requests                      
	[Events covered by l2_request_g2]
l2_request_g1.prefetch_l2                         
	[Requests to L2 Group1]
l2_request_g1.rd_blk_l                            
	[Requests to L2 Group1]
l2_request_g1.rd_blk_x                            
	[Requests to L2 Group1]
l2_request_g2.bus_locks_originator                
	[Multi-events in that LS and IF requests can be received simultaneous]
l2_request_g2.bus_locks_responses                 
	[Multi-events in that LS and IF requests can be received simultaneous]
l2_request_g2.group1                              
	[All Group 1 commands not in unit0]
l2_request_g2.ic_rd_sized                         
	[Multi-events in that LS and IF requests can be received simultaneous]
l2_request_g2.ic_rd_sized_nc                      
	[Multi-events in that LS and IF requests can be received simultaneous]
l2_request_g2.ls_rd_sized                         
	[RdSized, RdSized32, RdSized64]
l2_request_g2.ls_rd_sized_nc                      
	[RdSizedNC, RdSized32NC, RdSized64NC]
l2_request_g2.smc_inval                           
	[Multi-events in that LS and IF requests can be received simultaneous]
l2_wcb_req.cl_zero                                
	[LS (Load/Store unit) to L2 WCB (Write Combining Buffer) cache line
	zeroing requests]
l2_wcb_req.wcb_close                              
	[LS to L2 WCB close requests]
l2_wcb_req.wcb_write                              
	[LS to L2 WCB write requests]
l2_wcb_req.zero_byte_store                        
	[LS to L2 WCB zero byte store requests]
l3_comb_clstr_state.other_l3_miss_typs            
	[Other L3 Miss Request Types. Unit: amd_l3]
l3_comb_clstr_state.request_miss                  
	[L3 cache misses. Unit: amd_l3]
l3_lookup_state.all_l3_req_typs                   
	[All L3 Request Types. Unit: amd_l3]
l3_request_g1.caching_l3_cache_accesses           
	[Caching: L3 cache accesses. Unit: amd_l3]
xi_ccx_sdp_req1.all_l3_miss_req_typs              
	[All L3 Miss Request Types. Ignores SliceMask and ThreadMask. Unit:
	amd_l3]
xi_sys_fill_latency                               
	[L3 Cache Miss Latency. Total cycles for all transactions divided by
	16. Ignores SliceMask and ThreadMask. Unit: amd_l3]

core:
ex_div_busy                                       
	[Div Cycles Busy count]
ex_div_count                                      
	[Div Op Count]
ex_ret_brn                                        
	[Retired Branch Instructions]
ex_ret_brn_far                                    
	[Retired Far Control Transfers]
ex_ret_brn_ind_misp                               
	[Retired Indirect Branch Instructions Mispredicted]
ex_ret_brn_misp                                   
	[Retired Branch Instructions Mispredicted]
ex_ret_brn_resync                                 
	[Retired Branch Resyncs]
ex_ret_brn_tkn                                    
	[Retired Taken Branch Instructions]
ex_ret_brn_tkn_misp                               
	[Retired Taken Branch Instructions Mispredicted]
ex_ret_cond                                       
	[Retired Conditional Branch Instructions]
ex_ret_cond_misp                                  
	[Retired Conditional Branch Instructions Mispredicted]
ex_ret_cops                                       
	[Retired Uops]
ex_ret_fus_brnch_inst                             
	[The number of fused retired branch instructions retired per cycle. The
	number of events logged per cycle can vary from 0 to 3]
ex_ret_instr                                      
	[Retired Instructions]
ex_ret_mmx_fp_instr.mmx_instr                     
	[MMX instructions]
ex_ret_mmx_fp_instr.sse_instr                     
	[SSE instructions (SSE, SSE2, SSE3, SSSE3, SSE4A, SSE41, SSE42, AVX)]
ex_ret_mmx_fp_instr.x87_instr                     
	[x87 instructions]
ex_ret_near_ret                                   
	[Retired Near Returns]
ex_ret_near_ret_mispred                           
	[Retired Near Returns Mispredicted]
ex_tagged_ibs_ops.ibs_count_rollover              
	[Number of times an op could not be tagged by IBS because of a previous
	tagged op that has not retired]
ex_tagged_ibs_ops.ibs_tagged_ops                  
	[Number of Ops tagged by IBS]
ex_tagged_ibs_ops.ibs_tagged_ops_ret              
	[Number of Ops tagged by IBS that retired]

floating point:
fp_num_mov_elim_scal_op.opt_potential             
	[Number of Ops that are candidates for optimization (have Z-bit either
	set or pass)]
fp_num_mov_elim_scal_op.optimized                 
	[Number of Scalar Ops optimized]
fp_num_mov_elim_scal_op.sse_mov_ops               
	[Number of SSE Move Ops]
fp_num_mov_elim_scal_op.sse_mov_ops_elim          
	[Number of SSE Move Ops eliminated]
fp_ret_sse_avx_ops.all                            
	[All FLOPS]
fp_ret_sse_avx_ops.dp_add_sub_flops               
	[Double precision add/subtract FLOPS]
fp_ret_sse_avx_ops.dp_div_flops                   
	[Double precision divide/square root FLOPS]
fp_ret_sse_avx_ops.dp_mult_add_flops              
	[Double precision multiply-add FLOPS. Multiply-add counts as 2 FLOPS]
fp_ret_sse_avx_ops.dp_mult_flops                  
	[Double precision multiply FLOPS]
fp_ret_sse_avx_ops.sp_add_sub_flops               
	[Single-precision add/subtract FLOPS]
fp_ret_sse_avx_ops.sp_div_flops                   
	[Single-precision divide/square root FLOPS]
fp_ret_sse_avx_ops.sp_mult_add_flops              
	[Single precision multiply-add FLOPS. Multiply-add counts as 2 FLOPS]
fp_ret_sse_avx_ops.sp_mult_flops                  
	[Single-precision multiply FLOPS]
fp_retired_ser_ops.sse_bot_ret                    
	[SSE bottom-executing uOps retired]
fp_retired_ser_ops.sse_ctrl_ret                   
	[SSE control word mispredict traps due to mispredictions in RC, FTZ or
	DAZ, or changes in mask bits]
fp_retired_ser_ops.x87_bot_ret                    
	[x87 bottom-executing uOps retired]
fp_retired_ser_ops.x87_ctrl_ret                   
	[x87 control word mispredict traps due to mispredictions in RC or PC,
	or changes in mask bits]
fp_retx87_fp_ops.add_sub_ops                      
	[Add/subtract Ops]
fp_retx87_fp_ops.all                              
	[All Ops]
fp_retx87_fp_ops.div_sqr_r_ops                    
	[Divide and square root Ops]
fp_retx87_fp_ops.mul_ops                          
	[Multiply Ops]
fp_sched_empty                                    
	[This is a speculative event. The number of cycles in which the FPU
	scheduler is empty. Note that some Ops like FP loads bypass the
	scheduler]
fpu_pipe_assignment.dual                          
	[Total number multi-pipe uOps]
fpu_pipe_assignment.total                         
	[Total number uOps]

memory:
ls_dc_accesses                                    
	[The number of accesses to the data cache for load and store
	references. This may include certain microcode scratchpad accesses,
	although these are generally rare. Each increment represents an
	eight-byte access, although the instruction may only be accessing a
	portion of that. This event is a speculative event]
ls_dispatch.ld_dispatch                           
	[Counts the number of operations dispatched to the LS unit. Unit Masks
	ADDed]
ls_dispatch.ld_st_dispatch                        
	[Load-op-Stores]
ls_dispatch.store_dispatch                        
	[Counts the number of operations dispatched to the LS unit. Unit Masks
	ADDed]
ls_inef_sw_pref.data_pipe_sw_pf_dc_hit            
	[The number of software prefetches that did not fetch data outside of
	the processor core]
ls_inef_sw_pref.mab_mch_cnt                       
	[The number of software prefetches that did not fetch data outside of
	the processor core]
ls_l1_d_tlb_miss.all                              
	[L1 DTLB Miss or Reload off all sizes]
ls_l1_d_tlb_miss.tlb_reload_1g_l2_hit             
	[L1 DTLB Reload of a page of 1G size]
ls_l1_d_tlb_miss.tlb_reload_1g_l2_miss            
	[L1 DTLB Miss of a page of 1G size]
ls_l1_d_tlb_miss.tlb_reload_2m_l2_hit             
	[L1 DTLB Reload of a page of 2M size]
ls_l1_d_tlb_miss.tlb_reload_2m_l2_miss            
	[L1 DTLB Miss of a page of 2M size]
ls_l1_d_tlb_miss.tlb_reload_32k_l2_hit            
	[L1 DTLB Reload of a page of 32K size]
ls_l1_d_tlb_miss.tlb_reload_32k_l2_miss           
	[L1 DTLB Miss of a page of 32K size]
ls_l1_d_tlb_miss.tlb_reload_4k_l2_hit             
	[L1 DTLB Reload of a page of 4K size]
ls_l1_d_tlb_miss.tlb_reload_4k_l2_miss            
	[L1 DTLB Miss of a page of 4K size]
ls_locks.bus_lock                                 
	[Bus lock when a locked operations crosses a cache boundary or is done
	on an uncacheable memory type]
ls_misal_accesses                                 
	[Misaligned loads]
ls_not_halted_cyc                                 
	[Cycles not in Halt]
ls_pref_instr_disp.load_prefetch_w                
	[Prefetch, Prefetch_T0_T1_T2]
ls_pref_instr_disp.prefetch_nta                   
	[Software Prefetch Instructions (PREFETCHNTA instruction) Dispatched]
ls_pref_instr_disp.store_prefetch_w               
	[Software Prefetch Instructions (3DNow PREFETCHW instruction)
	Dispatched]
ls_stlf                                           
	[Number of STLF hits]
ls_tablewalker.perf_mon_tablewalk_alloc_dside     
	[Tablewalker allocation]
ls_tablewalker.perf_mon_tablewalk_alloc_iside     
	[Tablewalker allocation]

other:
de_dis_dispatch_token_stalls0.agsq_token_stall    
	[AGSQ Tokens unavailable]
de_dis_dispatch_token_stalls0.alsq1_token_stall   
	[ALSQ 1 Tokens unavailable]
de_dis_dispatch_token_stalls0.alsq2_token_stall   
	[ALSQ 2 Tokens unavailable]
de_dis_dispatch_token_stalls0.alsq3_0_token_stall 
	[Cycles where a dispatch group is valid but does not get dispatched due
	to a token stall]
de_dis_dispatch_token_stalls0.alsq3_token_stall   
	[ALSQ 3 Tokens unavailable]
de_dis_dispatch_token_stalls0.alu_token_stall     
	[ALU tokens total unavailable]
de_dis_dispatch_token_stalls0.retire_token_stall  
	[RETIRE Tokens unavailable]
ic_oc_mode_switch.ic_oc_mode_switch               
	[IC to OC mode switch]
ic_oc_mode_switch.oc_ic_mode_switch               
	[OC to IC mode switch]
rNNN                                               [Raw hardware event descriptor]
cpu/t1=v1[,t2=v2,t3 ...]/modifier                  [Raw hardware event descriptor]
mem:<addr>[/len][:access]                          [Hardware breakpoint]

Metric Groups:


</p>
</details>
