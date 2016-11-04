---
layout: post
title: "Build and Run your own kernel in 60 mins"
excerpt: "Run your own kernel in 60 mins or less. Kernel n00b to n00b++ in 60mins"
tags: [Linux, kernel, buildroot, kernel hacking, qemu, kvm, qemu-kvm, hacking]
comments: true
---

I will teach you how to make a lot of mon..er.. No, in this we will have our own running kernel within the next 60 mins (likely lesser).
If you do it right, most of these 60mins will be spent like this:

![img](//imgs.xkcd.com/comics/compiling.png)

Recently, I was asked by a friend about how to compile Linux Kernel. Shortly after, I realized that he assumed this needed a really complex system with a very sophisticated configuration. Although he was running a fairly new system he felt it needed a lot more "setup time" to get his kernel compiled and running. I found that most people are under the impression that you had to have this complex setup to be able to start "hacking the kernel".
I challenged him that we could find a way to be able to boot and run a kernel in under 60 mins!
For the impatient, I have kept this entry short enough but if you are feeling extra-impatient follow the instructions inside<a id="sec-1" name="sec-1"></a>

    this block

Steps:

### Install QEMU and KVM<a id="sec-2" name="sec-2"></a>

    # Ex: Debian systems
    sudo apt-get install qemu-kvm

### Get Linux kernel:<a id="sec-3" name="sec-3"></a>

    $ git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

### Build kernel config<a id="sec-4" name="sec-4"></a>

I have to mention that in my experience as a kernel n00b I used to get stuck in this step more often than I should have. I'd either end up double guessing myself about whether I'd need this new feature simply based on how well the one-line description looked in the menu or getting drowned in the kernel documentation rabbit-hole. But the kernel ships with a default config that is good enough to compile all the necessary features for booting a kernel while leaving the extra-luggage out by the door. Simply: <a id="sec-4-1" name="sec-4-1"></a>

    cd KERNEL_ROOT/arch/<YOUR_MACHINE_ARCH_TYPE>/configs/

You should make sure you know what your machine's "YOUR\_MACHINE\_ARCH\_TYPE" is.<a id="sec-4-2" name="sec-4-2"></a>
Not sure?<a id="sec-4-3" name="sec-4-3"></a>

    uname -m

Even if you are running x86\_64 the config files are placed under<a id="sec-4-4" name="sec-4-4"></a>

    ls KERNEL_ROOT/arch/x86/configs/
    # no suffix '_64' in dirname

In my case, I'm running a x86\_64 machine for desktop. So,<a id="sec-4-5" name="sec-4-5"></a>

    # I will choose x86_64_defconfig
    make x86_64_defconfig

Typically, I add a list of my favorite options to the kernel that I leave turned on by default in my kernels. Ex: CONFIG\_FUNCTION\_GRAPH\_TRACER, CONFIG\_PREEMPT\_TRACER, CONFIG\_IRQSOFF\_TRACER<a id="sec-4-6" name="sec-4-6"></a>

### Compile
This should only take a few seconds. It's already time to actually compile our shiny new kernel:<a id="sec-4-7" name="sec-4-7"></a>

    make -jNN
    # NN = number of cores. Ex: make -j4 on a 4-core machine

### Compile Success? Verify.
Ensure that the kernel compilation succeeded by looking for bzImage under KERNEL\_ROOT/arch/x86/boot/. I enourage you to have this in your shell to save some typing xlater.<a id="sec-4-8" name="sec-4-8"></a>

    export MY_BZIMG=$KERNEL_ROOT/arch/x86/boot/bzImage

This will take several minutes for the first time, say about 6-7 mins in a normal case for an idle 4-core machine (for assumptions within reason)<a id="sec-4-9" name="sec-4-9"></a>

### Buildroot
Now we have the kernel already compiled. However, remember kernel doesn't all by itself. We will need a shell running in user space for which you will need a sandbox userspace. One of the quickest options is to build an initial ramdisk, which basically provides a temporary root fs in memory.<a id="sec-4-10" name="sec-4-10"></a>
The best option here is [buildroot](https://buildroot.org/), you don't have to click and get lost in the ocean of details (yet!). Here's an intro from it's official documentation: "Buildroot is a tool that simplifies and automates the process of building a complete Linux system for an embedded system, using cross-compilation.". OK! so you know enough to follow up on the details later. For now we have got to start compiling.<a id="sec-4-11" name="sec-4-11"></a>

    git clone git://git.buildroot.net/buildroot
    cd buildroot
    make menuconfig
    
    # Select these two options
    
    Target options -> Target Architecture -> ex: x86_64
    
    Filesystem Images ->  cpio the root filesystem (for use as an initial RAM filesystem)

In the menu config for buildroot, there are a ton of config options that might seem interesting and you would want to explore them one by one, because it's magic! But we still are quite a way away from being able to run our kernel, yet we are close!<a id="sec-4-12" name="sec-4-12"></a>

    make

To ensure buildroot compiled successfully, look for rootfs.cpio under $BUIDLROOT\_ROOT/output/images/. I encourage you to do this<a id="sec-4-13" name="sec-4-13"></a>

    export BUIDLROOT_PKG=$BUILDROOT_ROOT/output/images/rootfs.cpio

If you're missing any dependencies you should be able to quickly figure out by searching stackoverflow or by installing packages directly. Once buildroot has started compiling then you can take a break for several minutes. This is, easily, the most time-consuming step of the 60 mins we allocated for this activiity.<a id="sec-4-14" name="sec-4-14"></a>
If everything went well, you should be very close to having your first kernel compiled on your machine.<a id="sec-4-15" name="sec-4-15"></a>
Now we have a compressed image of the kernel binaries and a filesystem package for running a basic ramdisk image with a minimal userspace binaries to talk to your kernel!<a id="sec-4-16" name="sec-4-16"></a>
The following part is borrowed from [LKP test script](https://git.kernel.org/cgit/linux/kernel/git/wfg/lkp-tests.git/tree/rootfs/kexec/run-in-kvm.sh) and [Laura's blog](http://www.labbott.name/blog/2016/04/22/quick-kernel-hacking-with-qemu-+-buildroot/) (You should check it out, it's awesome!):<a id="sec-4-17" name="sec-4-17"></a>
Copy this script into, say, **start-qemu.sh**

    #!/bin/bash
    
    kernel=$1
    initrd=$2
    
    if [ -z $kernel ]; then
        echo "pass the kernel argument"
        exit 1
    fi
    
    if [ -z $initrd ]; then
        echo "pass the initrd argument"
        exit 1
    fi
    
    kvm=(
        qemu-system-x86_64
        -enable-kvm
        -cpu kvm64,+rdtscp
        -kernel $kernel
        -m 300
        -device e1000,netdev=net0
        -netdev user,id=net0
        -boot order=nc
        -no-reboot
        -watchdog i6300esb
        -rtc base=localtime
        -serial stdio
        -vga qxl
        -initrd $initrd
        -spice port=5930,disable-ticketing
        -s
    )
    
    append=(
        hung_task_panic=1
        earlyprintk=ttyS0,115200
        systemd.log_level=err
        debug
        apic=debug
        sysrq_always_enabled
        rcupdate.rcu_cpu_stall_timeout=100
        panic=-1
        softlockup_panic=1
        nmi_watchdog=panic
        oops=panic
        load_ramdisk=2
        prompt_ramdisk=0
        console=tty0
        console=ttyS0,115200
        vga=normal
        root=/dev/ram0
        rw
        drbd.minor_count=8
    )
    
    "${kvm[@]}" --append "${append[*]}"

## Finally, It's time to run!

After our 60mins, I encourage you to play with these options. But first let's try to get our system running:<a id="sec-4-18" name="sec-4-18"></a>

    ./start-qemu.sh $MY_BZIMG $BUILDROOT_PKG

This should boot your kernel with a print out of intial kernel logs on your screen and end at a login prompt (user: root, no passwd)<a id="sec-4-19" name="sec-4-19"></a>

That's it! you have successfully downloaded the absolute latest and greatest kernel from the source tree of Linus Torvalds and compiled it. This is really the basic building block of kernel hacking. Save this setup, play around with it until you have exactly what you are looking for, tweak kernel config options, add your own prints in the kernel for fun!<a id="sec-4-20" name="sec-4-20"></a>

Caveat: Are you screaming "Woohoo! I never this powerful before"? or "Nothing works! This entry sucks"? Either way, this is just the start of something that can repay rich dividends for a programmer. Play around with everything mentioned above until it works for you. There's no shorter route. There's no substitute for your own work. Above steps assume that you are using a reasonably modern machine running Linux to compile. Also, if you don't make it in 60 mins don't sue me please.


*Time to start growing a beard on your necks! Wait, what?!*
