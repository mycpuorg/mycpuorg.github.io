---
layout: post
title: "Detecting a USB disconnection in a Hurry"
excerpt: "Detect a dead USB conn"
tags: [USB, Linux, OS]
comments: true
---
```
This post works for a quick hack but it is inefficient. An efficient way to do
the same is written in 
```
I used have laptop which was my favorite for the longest time. It survived
through my grad school abuse and many years before and after. However, I had
some useful scripts and other files that I wanted to occasionally access
remotely from my other machines. Now these files were not all organized nicely
in a central location on the storage or anything. In fact, most of the time all
I knew was that there was a script on the laptop that I needed to access
remotely.

So a lot of the times, I would simply login and run a find on candidate path
(typically in my home directory under a dir very, unhelpfully, named
`research`. Soon age caught up with it and the WiFi started to act flaky. There
were options to fix this separately, but I did not have the mental energy nor
the inclination to take this laptop apart. Besides, it did not "feel" right to
dismember anything from a compactly built laptop which had faithfully served for
many years.

So, I attached a WiFi-USB dongle to it and installed some drivers on Linux Mint
that it was running and eventually things got back to normal. Not for long. The
WiFi was extremely flaky because it was connected over the SDIO bus to the
chipset which I read is not the best choice for supporting a robust WiFi
bandwidth in any case.

Whenever the WiFi dongle and it's connected SDIO bus was asked to support a
bandwidth over a particular watermark level, the dongle would simply disconnect
and refuse to connect back until someone physically pulled the dongle out and
plugged it back in thus defeating the whole remote accessing of the files.

I wanted to be notified each time such a disconnection occurred so I wrote a
simple script to detect this. This is not sophisticated or impressive by any
means, but I wanted to log it here for posterity. At least to remind me of those
expensive external storage days.

Usage is simple:
```
-------Usage---------
monitor_usb arg1 arg2
arg1 - USB_VENDOR_ID:USB_DEVICE_ID
arg2 - FREQ in seconds
---------------------
```

USB_VENDOR_ID and USB_DEVICE_ID can be found in the output of lsusb.
```
╰─ lsusb
Bus 004 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 003 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 001 Device 017: ID 045e:07fd Microsoft Corp. Nano Transceiver 1.1
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
```

Demo ``045e:07fd`` is the USB ID pair for my mouse which I disconnected after a few seconds as shown below.

```
-----------------------------------
╭─ /tmp
╰─ . ./monitor_usb 045e:07fd 1
Checking USB ...
OK
Checking USB ...
OK
Checking USB ...
OK
Checking USB ...
OK
Checking USB ...
OK
Checking USB ...
OK
Checking USB ...
OK
Checking USB ...
USB CONN BROKEN - Sun Jan  5 18:30:09 PST 2020

```

The script itself was hardly anything but parsing the right fields from the
output of ``lsusb``

```bash

#!/bin/bash

check_usage()
{
    echo "-------Usage---------"
    echo "monitor_usb arg1 arg2"
    echo "arg1 - USB_VENDOR_ID:USB_DEVICE_ID"
    echo "arg2 - FREQ in seconds"
    echo "---------------------"
}

if [ "$#" -ne 2 ];
then
    check_usage
    return 1
fi

USB_ID="$1"
FREQ_SEC=$2
while [ true ]; do
    echo "Checking USB ... "
    res=`lsusb | cut -f6 -d' ' | grep "$1"`
    if [ "$USB_ID" != "$res" ];
    then
	echo "USB CONN BROKEN - `date`"
	break
    fi
    sleep $FREQ_SEC
    echo "OK"
done

```
