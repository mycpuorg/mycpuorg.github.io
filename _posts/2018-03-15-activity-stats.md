---
layout: post
title: "Activity Stats"
excerpt: "Emacs Configuration to log activity"
tags: [Interview, Podcast, VoidStar, Tech, Achievers, Excellence]
comments: true
---

<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. Emacs offers you extreme configurability without depending solely on pre-defined functions and variables. Perhaps, the most significant characteristic of vim is its blazing speed and almost no burden from adding bells and whistles. And one of my favorite features in Vim is the relative numbering of the lines. This allows you to almost instantly use Vim's commands with lines/numbers as arguments. In the code snippet below (Linux Kernel), say, you wanted to remove the comment with "TODO" since the comment is no longer valid in the context. You do not have to start calculating</a></li>
<li><a href="#sec-2">2. This came out my interview for the VoidStar Podcast Series with Joe Armstrong.</a></li>
<li><a href="#sec-3">3. The idea is to track your activity via Emacs. Given that Emacs is more of "an OS that lacks a good editor" this is actually possible.</a></li>
<li><a href="#sec-4">4. Here I give you the outline of how to setup your Emacs system so that you can try and track your activity too.</a></li>
<li><a href="#sec-5">5. The primary idea is to track which file you have been spending the most time on per week.</a></li>
<li><a href="#sec-6">6. For this you will need an Emacs plugin that can keep track of when you opened a file and when you closed it.</a></li>
<li><a href="#sec-7">7. Here's my code to do this, but before that, I have to disclose that a part of this code was lifted from some response to StackOverflow question etc.</a></li>
<li><a href="#sec-8">8. Reference: </a></li>
<li><a href="#sec-9">9. This does not handle closing Emacs altogether, and killing Emacs all of a sudden. Frankly neither of these are an occurence in my system. So here goes.</a></li>
</ul>
</div>
</div>

### Emacs offers you extreme configurability without depending solely on pre-defined functions and variables. Perhaps, the most significant characteristic of vim is its blazing speed and almost no burden from adding bells and whistles. And one of my favorite features in Vim is the relative numbering of the lines. This allows you to almost instantly use Vim's commands with lines/numbers as arguments. In the code snippet below (Linux Kernel), say, you wanted to remove the comment with "TODO" since the comment is no longer valid in the context. You do not have to start calculating<a id="sec-1" name="sec-1"></a>

    void update_rq_clock(struct rq *rq)
    {
            s64 delta;
    
            lockdep_assert_held(&rq->lock);
    
            if (rq->clock_update_flags & RQCF_ACT_SKIP)
                    return;
    
            /* TODO: add config sched debug warning */
    #ifdef CONFIG_SCHED_DEBUG
            if (sched_feat(WARN_DOUBLE_CLOCK))
                    SCHED_WARN_ON(rq->clock_update_flags & RQCF_UPDATED);
            rq->clock_update_flags |= RQCF_UPDATED;
    #endif
    
            delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
            if (delta < 0)
                    return;
            rq->clock += delta;
            update_rq_clock_task(rq, delta);
    }

### This came out my interview for the VoidStar Podcast Series with Joe Armstrong.<a id="sec-2" name="sec-2"></a>

### The idea is to track your activity via Emacs. Given that Emacs is more of "an OS that lacks a good editor" this is actually possible.<a id="sec-3" name="sec-3"></a>

### Here I give you the outline of how to setup your Emacs system so that you can try and track your activity too.<a id="sec-4" name="sec-4"></a>

### The primary idea is to track which file you have been spending the most time on per week.<a id="sec-5" name="sec-5"></a>

### For this you will need an Emacs plugin that can keep track of when you opened a file and when you closed it.<a id="sec-6" name="sec-6"></a>

### Here's my code to do this, but before that, I have to disclose that a part of this code was lifted from some response to StackOverflow question etc.<a id="sec-7" name="sec-7"></a>

### Reference: <https://www.gnu.org/software/emacs/manual/html_node/elisp/Visiting-Functions.html#Visiting-Functions><a id="sec-8" name="sec-8"></a>

### This does not handle closing Emacs altogether, and killing Emacs all of a sudden. Frankly neither of these are an occurence in my system. So here goes.<a id="sec-9" name="sec-9"></a>

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;; My edits to track the files I use
    (add-hook 'before-save-hook 'time-stamp)
    
    (defvar my-m-x-log-file "~/mx.log")
    ;; (defadvice execute-extended-command (after log-execute-extended-command activate)
    ;;   (let ((logfile (find-file-noselect my-m-x-log-file)))
    ;;     (with-current-buffer logfile
    ;;       (goto-char (point-max))
    ;;       (insert (format "%s %s\n" this-command 'insert-time-stamp))
    ;;       (save-buffer))))
    (defvar current-date-time-format "%a %b %d %H:%M:%S %Z %Y")
    (add-hook 'find-file-hook 'my-fileopen-hook)
    (defun my-fileopen-hook()
      (setq my-buf-name (buffer-file-name))
      (let ((logfile (find-file-noselect my-m-x-log-file)))
      (with-current-buffer logfile
      (goto-char (point-max))
      (insert (format "[+] %s " my-buf-name))
      (insert (format-time-string current-date-time-format (current-time)))
      (insert "\n")
      (save-buffer)
      ))
      )
    
    (add-hook 'kill-buffer-hook 'my-fileclose-hook)
    (defun my-fileclose-hook()
      (setq my-buf-name (buffer-file-name))
      (let ((logfile (find-file-noselect my-m-x-log-file)))
      (with-current-buffer logfile
      (goto-char (point-max))
      (insert (format "[-] %s " my-buf-name))
      (insert (format-time-string current-date-time-format (current-time)))
      (insert "\n")
      (save-buffer)
      ))
    )