---
layout: post
title: "Detecting a USB Plug/Unplug Efficiently"
excerpt: "Let the Kernel Detect a dead USB conn for you"
tags: [USB, Linux, OS]
comments: true
---
In an [earlier post](http://www.mycpu.org/detect-usb-disconnect/), I had written
about detecting a dead USB with a simple script. The summary of that was you
have a shell script periodically polling the output of ``lsusb`` command to
monitor a USB device of your interest.

Unfortunately, the script is not efficient and it is embarrassing to be
monitoring constantly in this day and age. Thanks to Grant Vesely for bringing
this question up.

There are several ways to do the same, but the most efficient way to do this has
to be simply subscribing to a ``udev`` event or simply known as a ``uevent``
from the Kernel.

## What is ``udev``?
``udev`` provides a dynamic device directory containing only the files for actually
present devices. It creates or removes device node files usually located in the
/dev directory, or it renames network interfaces.

As part of the hotplug subsystem, ``udev`` is executed if a kernel device is
added or removed from the system. On device creation, ``udev`` reads the sysfs
directory of the given device to collect device attributes like label, serial
number or bus device number. These attributes may be used as keys to determine a
unique name for the device. ``udev`` maintains a database for devices present on
the system. On device removal, ``udev`` queries its database for the name of the
device file to be deleted. Refer to the man page for ``udev``

## How do you ``udev``?
The udev daemon, systemd-udevd (or systemd-udevd.service) communicates with the
kernel and receives device uevents directly from it each time you add or remove
a device from the system, or a device changes its state.

Udev is based on rules, they are flexible and powerful. Every
received device event is matched against the set of rules read from files
located in ``/lib/udev/rules.d`` and ``/run/udev/rules.d``.

You can write custom rules files in the ``/etc/udev/rules.d/`` directory (files
should end with the .rules extension) to process a device. Note that rules files
in this directory have the highest priority.

### ``udev`` Monitor
We can monitor all the ``udev`` events in the Kernel from user space using the following command.
```bash
udevadm monitor
```

### How to Detect a Dead USB with ``udev``?
#### Create ``Actions`` for Connection and Disconnection
For USB Connection, the action taken could simply be to log it
```bash
$ cat ~/usb_add_scream
#!/bin/bash

echo "USB inserted at $(date)" >> /tmp/usb_disc.log
```

On Disconnection event, the action taken could be to log the time of
disconnection.
```bash
$ cat ~/usb_remove_scream
#!/bin/bash

echo "USB disconnected at $(date)" >> /tmp/usb_disc.log
```

#### Rules to Trigger Actions
``udev`` allows you to create rules. These rules for my USB mouse look like
this:
```bash
SUBSYSTEM=="usb", ACTION=="add", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="045e", RUN="/home/manoj/usb_add_scream"
SUBSYSTEM=="usb", ACTION=="remove", ENV{DEVTYPE}=="usb_device", RUN="/home/manoj/usb_remove_scream"
```

Setting up these rules automatically trigger the action when the rules are
satisfied. In particular, the rule checks for ``usb`` subsystem's action for
``add`` or ``remove`` of a USB device type. If this rule is satisfied then it
``RUN``s the script for action (needs to have execute permissions)
```bash
╰─ lsusb| grep 045e
Bus 001 Device 036: ID 045e:07fd Microsoft Corp. Nano Transceiver 1.1
╰─ cat /tmp/usb_disc.log 
╰─ 
```
Then I disconnect the mouse connected over a USB hub.
```bash
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                        ✔  28.15G RAM  0.16 L
╰─ lsusb| grep 045e
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                    0|1 ↵  28.15G RAM  0.16 L
╰─ cat /tmp/usb_disc.log 
USB disconnected at Fri Jan 24 23:33:19 PST 2020
USB disconnected at Fri Jan 24 23:33:19 PST 2020
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                        ✔  28.15G RAM  0.15 L
╰─ 
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                        ✔  28.15G RAM  0.15 L
╰─ 
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                        ✔  28.15G RAM  0.15 L
╰─ lsusb| grep 045e     
Bus 001 Device 037: ID 045e:07fd Microsoft Corp. Nano Transceiver 1.1
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                        ✔  28.15G RAM  0.14 L
╰─ cat /tmp/usb_disc.log
USB disconnected at Fri Jan 24 23:33:19 PST 2020
USB disconnected at Fri Jan 24 23:33:19 PST 2020
USB inserted at Fri Jan 24 23:33:42 PST 2020
USB inserted at Fri Jan 24 23:33:42 PST 2020
╭─ ~/my/m/m/_posts   master ● ? ⍟2                                                                                        ✔  28.15G RAM  0.14 L
╰─ 
```

## Advantages
This method does not require any CPU for constantly monitoring, neither does it
take any other resource than to handle an in-memory notification from the kernel
to the user space. This is the basis for ``sysfs`` entries to show relevant
device information.
