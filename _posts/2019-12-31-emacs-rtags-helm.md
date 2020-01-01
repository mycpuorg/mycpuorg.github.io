---
layout: post
title: "Emacs as C++ IDE - First Step: rtags"
excerpt: "Emacs"
tags: [Emacs, Linux, Code, IDE, source code navigation]
comments: true
---
For years, I have been battling hard to find an optimal setup on my Emacs. 
Finally, after sufficient __patience__ I think I have found a tags setup that is not
too crufty to navigate. I have visited [rtags home
page](https://github.com/Andersbakken/rtags) for long enough, I even watched
[the famous cmake-ide evangelist's
video](https://github.com/atilaneves/cmake-ide) but never really figured out how
to get my Emacs to behave with rtags.

Now, what I am doing requires to navigate a large C++ codebase like nobody's
business and things were getting serious. So, I decided to spend the weekend
looking closely into this.

At first, I wanted everything to work out of the box then I realized this was a
pipe dream so I started the rdm server from terminal and ran the indexer from
command line too (Gross!)

However, after enough digging and sifting through the following links here I
have a working out of the box rtags/cmake-ide setup. I'm sharing the final
output here. I am running Emacs 26.1 compiled from source.

### tl;dr: The complete code to get rtags working out of the box
First clone the rtags repo inside your ``~/.emacs.d`` directory and build the
repo based on the instructions provided in the [rtags home
page](https://github.com/Andersbakken/rtags)

```elisp
(require 'rtags)
(cmake-ide-setup)
(add-hook 'c-mode-hook 'rtags-start-process-unless-running)
(add-hook 'c++-mode-hook 'rtags-start-process-unless-running)

(use-package rtags
  :ensure t
  :hook (c++-mode . rtags-start-process-unless-running)
  :config (setq rtags-completions-enabled t
		rtags-path "/home/manoj/.emacs.d/rtags/src/rtags.el"
		rtags-rc-binary-name "/home/manoj/.emacs.d/rtags/bin/rc"
		rtags-use-helm t
		rtags-rdm-binary-name "/home/manoj/.emacs.d/rtags/bin/rdm")
  :bind (("C-c E" . rtags-find-symbol)
  	 ("C-c e" . rtags-find-symbol-at-point)
  	 ("C-c O" . rtags-find-references)
  	 ("C-c o" . rtags-find-references-at-point)
  	 ("C-c s" . rtags-find-file)
  	 ("C-c v" . rtags-find-virtuals-at-point)
  	 ("C-c F" . rtags-fixit)
  	 ("C-c f" . rtags-location-stack-forward)
  	 ("C-c b" . rtags-location-stack-back)
  	 ("C-c n" . rtags-next-match)
  	 ("C-c p" . rtags-previous-match)
  	 ("C-c P" . rtags-preprocess-file)
  	 ("C-c R" . rtags-rename-symbol)
  	 ("C-c x" . rtags-show-rtags-buffer)
  	 ("C-c T" . rtags-print-symbol-info)
  	 ("C-c t" . rtags-symbol-type)
  	 ("C-c I" . rtags-include-file)
  	 ("C-c i" . rtags-get-include-file-for-symbol)))

(setq rtags-display-result-backend 'helm)
```

#### Source:
+ http://xenodium.com/all/
+ https://github.com/philippe-grenet/exordium#rtags
+ http://martinsosic.com/development/emacs/2017/12/09/emacs-cpp-ide.html#rtags
+ https://vxlabs.com/2016/04/11/step-by-step-guide-to-c-navigation-and-completion-with-emacs-and-the-clang-based-rtags/
+ https://github.com/emacs-helm/helm/wiki
+ https://nilsdeppe.com/posts/emacs-c++-ide
+ https://github.com/hlissner/doom-emacs/issues/1579
+ https://manpages.debian.org/jessie/systemd/systemd.unit.5.en.html

Happy New Year! See you more often in 2020!
