---
layout: post
title: "Ubuntu Installation on Android"
excerpt: "Install Ubuntu on Android system"
tags: [Linux, Ubuntu, Android ]
comments: true
---

### Android related setup

#### adb, fastboot and a rooted device with SD card slot

### SDCARD or DATA partition preparation

#### Create partitions

Use a 32-GB SD Card and partition into two:
One with 4GB of VFAT (FAT32) type with initramfs/boot partition.
The other with remaining storage as an ext4 partition. I used gparted to create partitions.
After creating partitions mount the card on Android device.
Now we need to bring Ubuntu filesystem on SDCARD.

    Output of mount shows this:
    # mount
    ...
    none /data/ubuntu/dev/pts devpts rw,seclabel,relatime,mode=600 0 0
    none /dev/pts devpts rw,seclabel,relatime,mode=600 0 0
    /dev/block/vold/179:129 /mnt/media_rw/sdcard1 vfat rw,dirsync,nosuid,nodev,noexec,relatime,uid=1023,gid=1023,fmask=0007,dmask=0007,allow_utime=0020,codepage=437,iocharset=iso8859-1,shortname=mixed,utf8,errors=remount-ro 0 0
    /dev/block/vold/179:129 /mnt/secure/asec vfat rw,dirsync,nosuid,nodev,noexec,relatime,uid=1023,gid=1023,fmask=0007,dmask=0007,allow_utime=0020,codepage=437,iocharset=iso8859-1,shortname=mixed,utf8,errors=remount-ro 0 0
    /dev/fuse /storage/sdcard1 fuse rw,nosuid,nodev,noexec,relatime,user_id=1023,group_id=1023,default_permissions,allow_other 0 0
    root@nexus:/ # ls -l /dev/block/

    Notice the 179:129 index for /dev/block/vold..

Now we have to find out the corresponding block device name from /dev/block

    ls -l /dev/block
    brw-rw---- root     system   179,  64 2017-05-02 10:18 mmcblk0boot1
    brw------- root     root     179,   1 2017-05-02 10:18 mmcblk0p1
    brw------- root     root     179,  10 2017-05-02 10:18 mmcblk0p10
    brw------- root     root     179,  11 2017-05-02 10:18 mmcblk0p11
    brw------- root     root     179,  12 2017-05-02 10:18 mmcblk0p12
    brw------- root     root     179,  13 2017-05-02 10:18 mmcblk0p13
    brw------- root     root     179,  14 2017-05-02 10:18 mmcblk0p14
    brw------- root     root     179,  15 2017-05-02 10:18 mmcblk0p15
    brw------- root     root     179,  16 2017-05-02 10:18 mmcblk0p16
    brw------- root     root     179,  17 2017-05-02 10:18 mmcblk0p17
    brw------- root     root     179,  18 2017-05-02 10:18 mmcblk0p18
    brw------- root     root     179,   2 2017-05-02 10:18 mmcblk0p2
    brw------- root     root     179,   3 2017-05-02 10:18 mmcblk0p3
    brw------- root     root     179,   4 2017-05-02 10:18 mmcblk0p4
    brw------- root     root     179,   5 2017-05-02 10:18 mmcblk0p5
    brw------- root     root     179,   6 2017-05-02 10:18 mmcblk0p6
    brw------- root     root     179,   7 2017-05-02 10:18 mmcblk0p7
    brw------- root     root     179,   8 2017-05-02 10:18 mmcblk0p8
    brw------- root     root     179,   9 2017-05-02 10:18 mmcblk0p9
    brw-rw---- root     system   179,  96 2017-05-02 10:18 mmcblk0rpmb
    brw------- root     root     179, 128 2017-05-02 12:25 mmcblk1
    brw------- root     root     179, 129 2017-05-02 12:25 mmcblk1p1
    brw------- root     root     179, 130 2017-05-02 12:25 mmcblk1p2
    drwxr-xr-x root     root              2017-05-02 10:18 platform
    drwx------ root     root              2017-05-02 12:25 vold
    brw------- root     root     254,   0 2017-05-02 10:18 zram0

As we can see the 4GB partition on SDCARD corresponds to mmcblk1p1,
therefore the other block device under the same major device 28GB
EXT4 partition enumerated as mmcblk1p2 device (179:130).

### Debootstrap steps

#### Commands for debootstrap and installing the debian/ubuntu file system on the SDCARD

#### First, you will need to install debootstrap on your desktop

    sudo apt-get install debootstrap

#### After this, you can run debootstrap to download the first stage images of xenial (or other Ubuntu release of your choice)

### apt conf modification

#### Configuring parameters such as environment variables etc to get Ubuntu up and running..

### Bonus points: Re-route X11 display to the screen

#### A nice-to-have feature

### UBUNTU on SDCARD: Writing the initramfs on /init

    #!/sbin/busybox sh
    
    # initamfs pre-boot init script
    
    # Mount the /proc and /sys filesytems
    /sbin/busybox mount -t proc none /proc
    /sbin/busybox mount -t sysfs none /sys
    /sbin/busybox mount -t tmpfs none /dev
    
    # Allow them to rest a little 
    /sbin/busybox sleep 1
    
    # Populate /dev
    /sbin/busybox mdev -s
    
    # Mount the root filesystem, second partition on micro SDCARD
    /sbin/busybox mount -t ext4 -o noatime,nodiratime,errors=panic /dev/mmcblk1p2 /mnt/root
    
    # Clean up
    /sbin/busybox umount /proc
    /sbin/busybox umount /sys
    /sbin/busybox umount /dev
    
    #Transfer root to SDCARD
    exec /sbin/busybox switch_root /mnt/root /etc/init

The file system of this initramfs is very minimalistic and only
contains the /sbin/busybox and the mount points /proc, /sys, /dev and
/mnt/root. Or to be on the safe side, use the original initamfs and
just add /sbin/busybox, a mount point /mnt/root and replace init with
the script above.

We will need the system's base address i.e., where the RAM begins. To
get it from your original kernel zImage, check for /proc/config.gz in
your running kernel or use the extrace-ikconfig script on the kernel
binary (??). This script is included in the kernel source under
kernel/scriptsextract-ikconfig

    extrace-ikconfig zImage | grep PHYS_OFFSET
    CONFIG_PHYS_OFFSET=0x44400000

or

    Look for "System RAM" under /proc/iomem 
    This will only give a hint, not the actual address. Ex: 0x40000000

In the Android code base:

    # At your desktop computer
    
    # mkdir my-initramfs
    # cd my-initramfs
    # mkdir -p proc sys dev mnt/root sbin
    # cp /my/arm/busybox sbin/busybox
    # cp /above/init init
    # chmod a+x init sbin/busybox
    # find . | cpio --quiet -H newc -o | gzip > ../initramfs.cpio.gz
    # cd ..
    
    # mkbootimg --base 0x00200000 --kernel zImage --ramdisk initramfs.cpio.gz -o my-boot.img 
    
    # Do not flash this image yet..

Creating an Ubunut root file system

Mount your SDcard, if not already mounted. I assume you've mounted it as /mnt/debian. If you prefer Ubuntu or some other Debian based distribution, the steps are the same. Replace the mirrors accordingly.

Chose a Debian mirror close to you, and begin to create the Debian root filesystem.

    At your desktop computer
    
    apt-get install debootstrap
    mkdir /mnt/debian
    mount -t ext4 /dev/sdf2 /mnt/debian
    debootstrap --verbose --arch armel --foreign squeeze /mnt/debian http://ftp.se.debian.org/debian
    umount /mnt/debian

On your Android device

    mount /dev/block/mmcblk1p2 /root
    export PATH=/sbin:/usr/sbin:/bin:/usr/bin:/system/bin
    busybox chroot /root /debootstrap/debootstrap --second-stage
    echo 'deb http://ports.ubuntu.com/ubuntu-ports xenial main' >/root/etc/apt/sources.list
    mount -t proc none /root/proc
    mount -t sysfs none /root/sys
    mount -o bind /dev /root/dev
    mount -t devpts none /root/dev/pts
    export TMPDIR=/tmp
    busybox chroot /root /bin/bash
    apt-get update
    exit
    sync
    
    # Now shutdown your device and remove the SDcard.

### UBUNTU on SDCARD:: Problems faced with apt-get update

I looked a little further into this and it's really baffling to me. I
enabled the debug flags for apt commands and looked at the output,
turns out if you provide a hostname in the sources list the command
fails during getaddinfo() and if I provide an IP address instead the
command fails with the following:
Err:3 <http://91.189.88.150/ubuntu-ports> xenial Release
  Could not create a socket for 91.189.88.150 (f=2 t=1 p=6) - socket
(13: Permission denied)

### UBUNTU on SDCARD:

The difference, as far as I can tell thus far, seems to be in that the '_apt' user cannot read the 'pubring.gpg' file that is being created in a temporary directory, which means that gpgv cannot access it when it runs;

    ==
    [pid 10149] stat("/etc/apt/trusted.gpg", {st_mode=S_IFREG|0644, st_size=12255, &#x2026;}) = 0
    [pid 10149] faccessat(AT_FDCWD, "/etc/apt/trusted.gpg", R_OK) = 0
    [pid 10149] open("*tmp/tmp.OcaWlGuT32/pubring.gpg", O_WRONLY|O_CREAT|O_APPEND, 0666) = -1 EACCES (Permission denied)
    [pid 10149] write(2, "/usr/bin/apt-key: 309: /usr/bin*"&#x2026;, 41) = 41
    [pid 10149] write(2, "cannot create /tmp/tmp.OcaWlGuT3"&#x2026;, 64) = 64
    ==

This problem does not occur when root is the sandbox user, set via 'APT::Sandbox::User "root";' in '/etc/apt/apt.conf'. It's the only setting present. Disable that setting and the problem returns, while running the same thing interactively works without any issues.

I'm a bit stumped, at this point, pausing my investigation for now, but logging it here in case someone else runs into this.

The warning we're seeing looks as follows;

==
W: An error occurred during the signature verification. The repository is not updated and the previous index files will be used. GPG error: <https://apt-cache.domain.example/cache/us-east-1.ec2.archive.ubuntu.com/ubuntu> xenial InRelease: Unknown error executing apt-key
`=

### UBUNTU on SDCARD: Add details to the following solutions before putting them up on the wiki

I also installed the debian package called android-permissions.deb
Here's my new apt.conf

    // Options for the downloading routines
    Acquire
    {
      Queue-Mode "host";       // host|access
      ForceIPv4 "true";
      Retries "0";
      Source-Symlinks "true";
      ForceHash "sha256"; // hashmethod used for expected hash: sha256, sha1 or md5ssum
      PDiffs "true";     // try to get the IndexFile diffs
      PDiffs::FileLimit "4"; // don't use diffs if we would need more than 4 diffs
      PDiffs::SizeLimit "50"; // don't use diffs if size of all patches excess
                              // 50% of the size of the original file
    
      Check-Valid-Until "true";
      Max-ValidTime "864000"; // 10 days
      Max-ValidTime::Debian-Security "604800"; // 7 days, label specific configuratii on
    
      // HTTP method configuration
      http
      {
        // Proxy "http://127.0.0.1:3128";
        // Proxy::http.us.debian.org "DIRECT";  // Specific per-host setting
        Timeout "120";
        Pipeline-Depth "5";
        AllowRedirect  "true";
    
        // Cache Control. Note these do not work with Squid 2.0.2
        No-Cache "false";
        Max-Age "86400";     // 1 Day age on index files
        No-Store "false";    // Prevent the cache from storing archives
        Dl-Limit "7";        // 7Kb/sec maximum download rate
        User-Agent "Debian APT-HTTP/1.3";
      };
    
    };
    Debug
    {
      pkgProblemResolver "true";
      pkgProblemResolver::ShowScores "true";
      pkgDepCache::AutoInstall "true"; // what packages apt install to satify depenn dencies
      pkgDepCache::Marker "true";
      pkgCacheGen "true";
      pkgAcquire "true";
      pkgAcquire::Worker "true";
      pkgAcquire::Auth "true";
      pkgDPkgPM "true";
      pkgDPkgProgressReporting "true";
      pkgOrderList "true";
      pkgPackageManager "true"; // OrderList/Configure debugging
      pkgAutoRemove "true";   // show information about automatic removes
      BuildDeps "true";
      pkgInitialize "true";   // This one will dump the configuration space
      NoLocking "true";
      Acquire::Ftp "true";    // Show ftp command traffic
      Acquire::Http "true";   // Show http command traffic
      Acquire::Https "true";   // Show https debug
      Acquire::gpgv "true";   // Show the gpgv traffic
      Acquire::cdrom "true";   // Show cdrom debug output
      aptcdrom "true";        // Show found package files
      IdentCdrom "true";
      acquire::netrc "true";  // netrc parser
      RunScripts "true";      // debug invocation of external scripts
    
    };
    Dir "/"
    {
      // Location of the state dir
      State "var/lib/apt/"
      {
         Lists "lists/";
         status "/var/lib/dpkg/status";
         extended_states "extended_states";
      };
    
    };
    
    Acquire::Connect::AddrConfig "true";
    
    // This line being the most important of them all in terms of getting
    // apt to work in chroot systems
    // This adds root to all the relevant groups that are important
    APT::Sandbox::User "root";