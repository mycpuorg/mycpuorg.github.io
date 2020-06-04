---
layout: post
title: "Emacs as Email Client - Part I"
excerpt: "Email with mu4e and offlineimap"
tags: [Emacs, Linux, Email]
comments: true
---
In [earlier](http://www.mycpu.org/emacs-productivity-setup/)
[posts](http://www.mycpu.org/emacs-rtags-helm/) [I
have](http://www.mycpu.org/emacs-rtags-helm-tramp/)
[shared](http://www.mycpu.org/emacs-relative-linum/)
[my](http://www.mycpu.org/emacs-24-magit-magic/)
[Emacs](http://www.mycpu.org/activity-stats/) configuration.

Emailing in Emacs is a super power that I have been grateful for over the past
several years. Below I will describe a simple setup that works for me and more
importantly for me, it's something I like. This setup makes me almost want to
write descriptive emails simply because it moves the pain of writing emails into
the same ecosystem that I feel comfortable writing long form articles, programs,
design documents and other artifacts that involve "putting my thoughts down". I
cringe everytime I see an email written in rich text with broad lines going well
over 150 characters.

A lot of the times it makes me think that people with very detailed email
responses tend to almost face no resistance between thinking and putting their
thoughts in a detailed email response. Consider the following:

![](/images/emacs-mail-read.gif)

Well it's not the best demo I guess, but what if you want to capture something
from the email for future reference? Maybe, you want to stash an important TODO
item from the email and keep notes for yourself. Furthermore, won't it nice to
automatically capture all the meta fields of the email, such as `from:`, `to:`,
`sent_time:` 

Take this for example, I am composing this post, obviously, in Emacs but I will
read LKML and capture some important notes that I can later access. In order to
not overwhelm an new Emacs user I won't go into the weeds with the specific
packages used and their configurations etc in this post. This is just to
motivate the right reader to think of using Emacs as their email client to boost
your productivity, well at least to get to a point where you don't hate
reading/writing emails.

**LET US START BY READING EMAIL**
![](/images/emacs-mail-org-capture.gif)

If you use [org-mode](https://orgmode.org/) on Emacs then you are
in luck, you simply select the right keyword from [org-capture template](https://www.gnu.org/software/emacs/manual/html_node/org/Template-expansion.html#fn-3).

OK, I will admit that I have spent unhealthy amounts of time tweaking my Emacs
config over the years. If you like what you see then you can simply start by
using my config as-is, by copying pieces of config in to your `.emacs` and
giving it a whirr.

Hope that whets your appetite to start using Emacs as an email client (among
other things) Config details coming up in a separate post...
