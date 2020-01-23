---
layout: post
title: "Emacs - Productivity Tricks/Hacks"
excerpt: "Emacs"
tags: [Emacs, Linux, Code, IDE, source code navigation]
comments: true
---
In the past, I have written about using [Emacs as a C++
IDE](http://www.mycpu.org/emacs-rtags-helm/),
[and](http://www.mycpu.org/emacs-rtags-helm-tramp/)
[some](http://www.mycpu.org/emacs-relative-linum/)
[other](http://www.mycpu.org/emacs-24-magit-magic/)
[stuff](http://www.mycpu.org/activity-stats/). However, I did not realize that I
have been using Emacs for many other things. To be brutally honest, I would not
have enjoyed using ``Emacs`` anywhere nearly as much if it wasn't for the
features listed here. So, I decided to cover some of them. I will provide the
configs that I have used to get them to work out of the box on my setup (as in,
by literally copying from ``.emacs``)

### First things
``tl;dr:`` If you are impatient to get started skip this section and proceed to
the first section with package ``Helm``

I am running ``Emacs - 26.1`` compiled from source. You don't need this 
All the packages were installed from ``Emacs`` package Manager. By running,
```
M-x list-packages
```
This should bring up a list of packages available in ``MELPA``. Now, don't worry
if you don't know what ``MELPA`` is, just think of it as a repository of all
packages, as in, analogous to the Debian Package Repo in Debian/Ubuntu
distros. In this case, we get a long list of available packages like below:

![List Packages](/images/emacs-list-pkgs.gif)

If you select a package then a new buffer with it's short description pops
up. This buffer usually contains a "Quick Start" instruction. You can simply
press ``i`` and then ``x`` to install the package. So if you like any of the
following packages you could do that.

### Helm
If you don't know what this is, just drop whatever you are doing and give this a
shot. Odds are, it is totally going to be worth your time.

I don't think I should try and describe this feature but here is the official
blurb from their [website](https://emacs-helm.github.io/helm/):
```
Helm is an Emacs framework for incremental completions and narrowing selections.
It helps to rapidly complete file names, buffer names, or any other Emacs
interactions requiring selecting an item from a list of possible choices. 
```
#### Demo
![Helm Demo](/images/helm-cmds.gif)

```lisp
(require 'helm)

(setq-default helm-M-x-fuzzy-match t)
(global-set-key "\C-x\C-m" 'helm-M-x)
(global-set-key "\C-c\C-m" 'helm-M-x)
(define-key evil-ex-map "x" 'helm-M-x)

(define-key evil-ex-map "b " 'helm-mini)
(define-key evil-ex-map "e" 'helm-find-files)
```

### Evil Mode
``Evil`` is an [Extensible VI Layer](https://www.emacswiki.org/emacs/Evil) for
Emacs.
This is, obviously, a big controversial topic to stray away from a ``purist``'s
Emacs experience. To be honest, there is no such thing. In my opinion, the raw
power of Emacs mainly comes from the ability to turn Emacs into whatever you
want. I grew up using machines that did not have anything apart from ``vi`` on
them so I ended up using it quite a bit and got quite good at it too. I'm no
guru, but I can use ``vim`` well enough to get some work done quickly or feel 
productive without knowing why. 

#### Approved By Your Orthopedician
Using Emacs, I missed the single key press commands a lot, mostly because I am
terribly slow at typing, at least, in comparison with
[real](http://www.mycpu.org/interview-with-Steve-Rostedt/)
[masters](http://www.mycpu.org/interview-with-GKH/) [I have
encountered](http://www.mycpu.org/interview-with-James-Gosling/)

Enable ``Evil`` Mode: 
```lisp
(require 'evil)
(evil-mode 1)

;;;; define shortcuts for powerful commands
;;;; these can be invoked vim-style 
;;;; Esc-:<single_key_from_below>
(define-key evil-ex-map "b " 'helm-mini)
(define-key evil-ex-map "e" 'helm-find-files)
(define-key evil-ex-map "g" 'helm-projectile-grep)
(define-key evil-ex-map "f" 'helm-projectile-find-file)

;;;; I wept with joy about this in:
;;;; http://www.mycpu.org/emacs-24-magit-magic/
(define-key evil-ex-map "m" 'magit-blame)
```

### Helm-Projectile
I cannot understand why people are not running on the streets just pinching
their scalps because they are stark raving mad with joy, because that's how
``helm-projectile`` makes me feel. [Github](https://github.com/bbatsov/helm-projectile)

#### Demo
![Helm Projectile Grep](/images/helm-projectile-git-grep.gif)

```lisp
(require 'helm-projectile)
(define-key evil-ex-map "g" 'helm-projectile-grep)
(define-key evil-ex-map "f" 'helm-projectile-find-file)
```

### Doom Themes
Since this is aesthetics based, it is very subjective. So skip this section if
you are happy but if you like what you see in the screenshots above, continue.

``Doom Themes`` helped me setup a "modern" looking Emacs. I get bored from time
to time about using the same looks on my ``Emacs`` (feel like there's some room
for psycho-analysis there). So I kept looking for "that one theme" on Emacs. I
used the ``zenburn`` theme for a long time. But I eventually realized that I
actually like contrasting font but not with colors that are too sharp. Enter
``Doom Themes``, in particular, ``doom-molokai`` which apparently mimics the
look and feel of the Modern ``Atom`` IDE. The bare minimum setup required for
the above setup is presented here. I use a modified version of this ~~stolen~~
from the Internet.

```lisp
(require 'doom-themes)

(require 'indent-guide)
(indent-guide-global-mode)
(set-face-background 'indent-guide-face "dimgray")

;; Global settings (defaults)
(setq doom-themes-enable-bold t    ; if nil, bold is universally disabled
      doom-themes-enable-italic t) ; if nil, italics is universally disabled

;; Load the theme (doom-one, doom-molokai, etc); keep in mind that each
;; theme may have their own settings.
(load-theme 'doom-molokai t)

;; Enable flashing mode-line on errors
(doom-themes-visual-bell-config)

;; Enable custom neotree theme
(doom-themes-neotree-config)  ; all-the-icons fonts must be installed!

(require 'doom-modeline)
(doom-modeline-mode 1)
```

### Rtags
I know I have written a couple of posts mainly about ``rtags``: [here](http://www.mycpu.org/emacs-rtags-helm/) and [there](http://www.mycpu.org/emacs-rtags-helm-tramp/)

### Reading Email in Emacs with MU4E
This really deserves a complete post to itself. The configuration this requires
is sort of non-trivial (my case at least). Lack of good Email Clients on Emacs
~~has~~ had been one of my long time peeves, sorry ``Gnus``. Apparently I was
not alone, and someone else (thankfully, smarter and more skilled) felt this needed to be solved
too. ``mu4e`` along with ``offlineimap`` have given me an in-Emacs solution to
writing emails that I actually enjoy.
