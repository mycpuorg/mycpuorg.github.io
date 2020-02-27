---
layout: post
title: "Emacs as C++ IDE - Next Step: rtags on tramp still fails"
excerpt: "Emacs"
tags: [Emacs, Linux, Code, IDE, source code navigation, tramp, remote]
comments: false
---
This is a continuation of my [earlier
post](http://www.mycpu.org/emacs-rtags-helm-tramp/) trying to get rtags to work
over TRAMP.

```
tl;dr: rtags over tramp still fails
```

### Failure to run rtags on remote files
One of the authors of remote rtags suggested that I ensure the ``rc`` binary is
actually triggered from within Emacs. So I tried a few things.

Here are the steps:
+ ssh to remote from a terminal outside Emacs, run rdm
+ open remote file via tramp (cmake succeeds in this project, outside Emacs)
+ MiniBuffer says *cmake* failed.
+ Doesn't respond to cmake-ide commands (which work fine when attempted on a local project file)
+ M-x shell-command: rc --help shows help menu errors out with "rc not found"
+ Interestingly, if I provide the full *remote* path to M-x shell-command for rc like /home/REMOTE/PATH/TO/rc -J . then I see the following error messages.

```
FileSystemWatcher::watch() '/tmp/cmakeMqZWRU/compile_commands.json' doesn't seem to be watchable
FileSystemWatcher::watch() '/tmp/cmakelkQV72/compile_commands.json' doesn't seem to be watchable
FileSystemWatcher::watch() '/tmp/cmakeBr0aly/compile_commands.json' doesn't seem to be watchable

```

The funny thing is ``cmake-ide`` + ``rtags`` works and works very well for local codebase so much so
that I am desperate to get it working on my remote code over TRAMP.
