---
layout: post
title: "Void Star Podcast - Interview with Greg Kroah-Hartman (GKH)"
excerpt: "Interview from the Void Star Podcast Series"
tags: [Interview, Podcast, VoidStar, Linux, GKH, Kernel]
comments: true
---
Welcome to the [Void Star Podcast Series](http://www.mycpu.org/about-interviews). This episode contains my conversation with Greg Kroah-Hartman. If you are not sure who GKH is, I'll save you the trouble of looking him up.
Here is the excerpt from GKH's Wikipedia page:

"Greg Kroah-Hartman (GKH) is a Linux kernel developer. He is the current Linux kernel maintainer for the -stable branch, the staging subsystem, USB, driver core, debugfs, kref, kobject, and the sysfs kernel subsystems, Userspace I/O (with Hans J. Koch), and TTY layer. He also created linux-hotplug, the udev project, and the Linux Driver Project. He worked for Novell in the SUSE Labs division and, as of 1 February 2012, works at the Linux Foundation.
He is a co-author of Linux Device Drivers (3rd Edition) and author of Linux Kernel in a Nutshell, and used to be a contributing editor for Linux Journal. He also contributes articles to LWN.net, the computing news site. 
Kroah-Hartman frequently helps in the documentation of the kernel and driver development through talks and tutorials. In 2006, he released a CD image of material to introduce a programmer to working on Linux device driver development.
Kroah-Hartman has been a strong advocate of a stable kernel–user space API."

### My Interview with Greg Kroah-Hartman (GKH)
#### Available on Google Play Music
<div>
<a href='https://playmusic.app.goo.gl/?ibi=com.google.PlayMusic&amp;isi=691797987&amp;ius=googleplaymusic&amp;apn=com.google.android.music&amp;link=https://play.google.com/music/m/I46jz5f2f345iw4a5kw3umtbela?t%3DVoidStar_Podcast%26pcampaignid%3DMKT-na-all-co-pr-mu-pod-16' rel='nofollow'><img width='125px' alt='Listen on Google Play Music' src='https://play.google.com/intl/en_us/badges-music/images/badges/en_badge_web_music.png'/></a>
</div>

#### Available on Stitcher
<div>
<a href="https://www.stitcher.com/s?fid=162759&refid=stpr"><img src="https://secureimg.stitcher.com/promo.assets/stitcher-banner-180x120.jpg" width="180" height="120" alt="Listen to Stitcher"></a>
</div>

![Towering-GKH](/images/GKH-Manoj.gif)

# Show Notes:<a id="sec-1" name="sec-1"></a>

## Greg's Git Config:<a id="sec-1-1" name="sec-1-1"></a>

    [alias]
            dc = describe --contains
            fp = format-patch -M
            b = branch -v
            s = status -sb

## Greg's VIM config:<a id="sec-1-2" name="sec-1-2"></a>

.vimrc lines for different things we talked about:

    " highlight trailing spaces
    highlight WhitespaceEOL ctermbg=red guibg=red
    match WhitespaceEOL /\s\+$/

## Greg's Kernel Coding Style Plugin:<a id="sec-1-3" name="sec-1-3"></a>
Greg uses the linux coding style addon for proper kernel formatting and to highlight when going over 80 columsn:

    http://github.com/vivien/vim-addon-linux-coding-style

## The plugin Greg uses for Editing files under git<a id="sec-1-4" name="sec-1-4"></a>

Greg also mentioned that he uses this vim plugin and likes it a lot for editing files under git control, to see what has changed easily:

    git://github.com/airblade/vim-gitgutter.git

## Other plugins Greg uses:<a id="sec-1-5" name="sec-1-5"></a>

As well as a few other common ones:

    closetag.vim
    matchit.vim
    minibufexpl.vim
    taglinst.vim
    vimaspell.vim