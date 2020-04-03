---
layout: post
title: "Bash - Productivity Tricks/Hacks"
excerpt: "Bash"
tags: [Bash, Linux, Terminal, Console]
comments: false
--
In [earlier](http://www.mycpu.org/emacs-productivity-setup)
[posts](http://www.mycpu.org/git-aliases/), I have shared my productivity tricks
with Emacs and Git. In this post, I will share some of the commonly used shell
tricks that have helped me be/stay productive.

## Command History
Search for a previously run command
```bash
history | grep <pattern>
```
OR
```bash
Ctrl-R <pattern>
```

## Repeat Last Command
Repeating the most recent command
```bash
UP arrow then ENTER
```
OR
```
!!
```
## Repeat Last Command Starting With ...
```bash
!<starting pattern>
```
Reopen the last file with ``vim`` that contains a long filename which you can't remember
```bash
!v
```

If you want to be sure that the last command starting with ``v`` is indeed
``vim`` and not ``vscode`` (Ugh) then you can examine to make sure the last
command was OK.

```bash
!v:p
```

## Repeat the Last Command With New Arguments
```bash
ls foo
ls bar
```
OR
```bash
ls foo
!!:0 bar
```

## Repeat Last Arguments With New Command
```bash
vim /tmp/foo.txt
emacs /tmp/foo.txt
```
OR
```bash
vim /tmp/foo.txt
^vim^emacs
```

## Hold Result Of Command
```bash
find . -name "blahblah.txt"
# found in here/there/everywhere/blahblah.txt
rm here/there/everywhere/blahblah.txt
```
OR
```bash
rm `find . -name "blahblah.txt"`

# same as

rm $(find . -name "blahblah.txt")
```

