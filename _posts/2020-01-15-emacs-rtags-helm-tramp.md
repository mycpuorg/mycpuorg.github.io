---
layout: post
title: "Emacs as C++ IDE - Next Step: rtags on tramp"
excerpt: "Emacs"
tags: [Emacs, Linux, Code, IDE, source code navigation, tramp, remote]
comments: true
---
In an [earlier post on Emacs as C++ IDE], I had shown how to get up and running
very quickly out of the box with rtags. While that setup is fantastic, a lot of
my code is running remote machines too. This is specially true at work and it
frequently cheats me off using my Emacs on the same level of satisfaction that I
use it otherwise.

```
This is a WIP, which needs to be verified on my setup that I cannot get to until
tomorrow.
If you see an error please leave a comment else, wait for the verification results.
```

### Wild Emacs + Tags Goose Chase Again??
Sigh! These are the things why some people feel frustrated and give up.
*Give up? Give ... up? NO* we are the brethren who suffer through untold pains
to stay away from phrases like "simply works".

So I did what any ~~in~~sane Emacs user would do. Waste several hours searching
for solutions on SO, Github etc and Voila! I found enough people with the same
problems but alas none with a solution that works out of the box:

+ [How can I use tramp mode with
rtags](https://github.com/Andersbakken/rtags/issues/477)
+ [rtags-socket-address support for remote code browsing](https://github.com/Andersbakken/rtags/pull/1303)
+ [Let rtags handle the project that sits remotely](https://github.com/Andersbakken/rtags/pull/546)
+ [How can I use rtags in TRAMP mode?](https://github.com/Andersbakken/rtags/issues/1206)
+ [Using rtags with Tramp](https://github.com/Andersbakken/rtags/issues/1240)

### The Steps (to be verified) that you need to get rtags working on TRAMP
Copy the ``rtags`` binaries from your machine over to the remote machine and set
it in an accessible path to Emacs. I put it in ``$HOME/.emacs.d/rtags/bin/``
Although, you can install rtags seperately over in the destination separately
you need to be very careful to install the same versions of the package down to
the last bit. Without this ``rtags`` starts crying about this mismatch.
```lisp
(setq rtags-autostart-diagnostics t)
(setq rtags-tramp-enabled t)

;;;; This is similar to setting $PATH env var, but for TRAMP
(add-to-list 'tramp-remote-path "/home/ubuntu/.emacs.d/rtags/bin")

;;;; This is the same as --socket-address to rc
(setq rtags-socket-address "my-remote-machine:8998")

;;;; Gives you an unfair advantage
(setq rtags-rc-log-enabled t)
```

#### Breaking it down
Here are the items you must do to setup remote rtags and I will then give you
the steps to verify if your installation is successful.
+ Set the variable ``rtags-tramp-enabled`` to ``t``, the reason should be obvious.
+ Set the variable ``tramp-remote-path`` to point to the location where you
  copied over the ``rc`` and ``rdm`` binaries in the remote machine. This is how
  Emacs can access these binaries remotely.
+ Set the variable ``rtags-socket-address`` to a val in ``host_name:port``
  format. This is how Emacs apparently reaches the remote location.
+ And finally, the most important and crucial step of all. The one thing that
  gives you an unfair advantage in this struggle, the thing that's your "gun in
  a knife fight" is setting ``rtags-rc-log-enabled`` to ``t`` which clearly
  tells you about the error.

### Demo!
