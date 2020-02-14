---
layout: post
title: "Git - Productivity Tricks/Hacks"
excerpt: "Git"
tags: [Git, Linux, Code, IDE, source code navigation]
comments: false
---
In [earlier](http://www.mycpu.org/simple-git-commit-hack/)
[posts](http://www.mycpu.org/simple-git-commit-hack/), I have written about a
couple of git tricks that I have found very useful.
Today, I'm dumping on you the entire list of git aliases. Out of all the aliases
shown below ``ri`` and ``gg`` are the most frequently used.
Although ``standup`` is useful I have not used it enough yet.
```
[alias]
		co = checkout
		ci = commit
		ca = commit --amend
		lol = log --oneline --decorate --graph
		ri = rebase -i
		rc = rebase --continue
		st = status
		ap = add -p
		standup = log --since yesterday --author `git config user.email` --pretty=short
		lg = log --pretty=format:"%Cred%h%Creset -%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset"
		unpushed = log --stat @{u}..HEAD
		gg = grep --break --heading -n
		cl = log --graph --pretty=format:"%an: %s%C(yellow)%d%Creset %Cgreen(%cr)%Creset" --date=relative
		cls = log --pretty=format:"%an (%ae): %C(yellow)%s%Creset" --shortstat
		diff = diff --word-diff
		wdiff = diff -w --word-diff-regex=. --color-words -U0
		dag = log --graph --format='format:%C(yellow)%h%C(reset) %C(blue)\"%an\" <%ae>%C(reset) %C(magenta)%cr%C(reset)%C(auto)%d%C(reset)%n%s' --date-order
```
