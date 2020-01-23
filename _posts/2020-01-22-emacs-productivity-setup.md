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
have enjoyed using ``Emacs`` anywhere nearly as much if it wasn't for things
listed I want to cover some of it here. I will provide the configs that I have
used to get them to work out of the box on my setup (as in, by literally copying
from ``.emacs``)

### Helm
If you don't know what this is, just drop whatever you are doing and give this a
shot. Odds are, it is totally going to be worth your time.

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
This is, obviously, a big controversial topic to stary away from a ``purist``'s
Emacs experience. To be honest, there is no such thing. In my opinion, the raw
power of Emacs mainly comes from the ability to turn Emacs into whatever you
want to turn it into. I grew up using machines that did not have anything apart
from ``vi`` in them so I ended up using it quite a bit and got quite good at
using it. I'm no guru, but I can use ``vim`` well enough to get some work done
quickly. I missed the single key press commands a lot, mostly because I am
terribly slow at typing, at least, in comparison with [real](http://www.mycpu.org/interview-with-Steve-Rostedt/) [masters](http://www.mycpu.org/interview-with-GKH/) [I have encountered](http://www.mycpu.org/interview-with-James-Gosling/)

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
``helm-projectile`` makes me feel.

#### Demo
![Helm Projectile Grep](/images/helm-projectile-git-grep.gif)

```lisp
(require 'helm-projectile)
(define-key evil-ex-map "g" 'helm-projectile-grep)
(define-key evil-ex-map "f" 'helm-projectile-find-file)
```

### Doom Themes
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

### MU4E
This really deserves a complete post to itself since the configuration requires
so many steps. But this has been one of my long time peeves that someone felt
needed to be solved. ``mu4e`` along with ``offlineimap`` have given me an
in-Emacs solution to writing emails that I actually enjoy.
