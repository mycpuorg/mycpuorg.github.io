---
layout: post
title: "Emacs Relative Line Numbering"
excerpt: "Emacs setup for vim's :se relativenumber"
tags: [Emacs, Linux, Code, IDE, source code navigation]
comments: true
---
I use both ``vim`` and ``Emacs`` because I grew up using a lot of ``vim`` then
decided to switch over to ``Emacs`` one day. The obvious choice was to switch to
using ``Evil Mode``. This post is not about that, at least not entirely. One of
the things I missed dearly about ``vim`` was the ease with which you could use
line numbers to move around and edit. I always found ``C-u`` preface very
crufty. The implicit numeric argument that precedes a command is what makes you
super fast with navigation in ``vim``. However, I had come to ~~like~~ *love*
``Emacs`` to go back. So I ~~copied~~ some ``ELisp`` code from the Internet and
modified it to achieve the exact result I wanted. This is something that works
out of the box on my ``Emacs - 26.1``

### Demo
![Relative Line Numbers](/images/emacs_screencast_relative_num.gif)

```lisp
(defvar my-linum-current-line-number 0)

(setq linum-format 'my-linum-relative-line-numbers)

(defun my-linum-relative-line-numbers (line-number)
  (let ((test2 (- line-number my-linum-current-line-number)))
    (propertize
     (number-to-string (cond ((<= test2 0) (* -1 test2))
                             ((> test2 0) test2)))
     'face 'linum)))

(defadvice linum-update (around my-linum-update)
  (let ((my-linum-current-line-number (line-number-at-pos)))
    ad-do-it))
(ad-activate 'linum-update)

(global-linum-mode t)
```
