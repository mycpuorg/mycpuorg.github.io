---
layout: post
title: "Android Death Trap Fixed"
excerpt: "How I Fixed Boot Loop on my Pixel 3"
tags: [Linux, Ubuntu, Android]
comments: false
---
I'm not a newbie to Android. In an [earlier
post](http://www.mycpu.org/ubuntu-on-android/), I have written about how to
install Ubuntu on an Android. However, this morning my Pixel 3 would not boot up
even when I pressed the power button. When I plugged into a charger I realized
that the phone was busy boot looping (boots up to initial bootloader / 'Google'
screen and reboots again, over and over).
I am lucky enough to have a friend who works in the Android Team on Pixel. So I
contacted him and he pointed me to a [web
tool](http://www.mycpu.org/ubuntu-on-android/) to try and flash a different
version of Android (specifically, the latest stable corresponding to my
phone). However, my phone was not in any sort of mood to let ``adb`` protocol
discover it on my computer. What I noticed was that the phone was rebooted even
when I put it in fastboot and did not make any selections, just let it sit
there.

Then I realized that trigger to restart the device has to come from within the
device and not from any software.

### The Fix?
I realized that for the phone to reboot even when in ``fastboot`` mode, only
power button can do that. So Itook the external casing that  I usse and
disconnected played around with the powere button to ensure there are no
erroneous button presses.
After this the phone now works fine.
