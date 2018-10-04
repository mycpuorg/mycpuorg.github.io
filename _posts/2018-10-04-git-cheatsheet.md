---
layout: post
title: "Yeat Another Git CheatSheet"
excerpt: "Super helpful Git CheatSheet stolen from the Internet"
tags: [Git, Emacs, Linux, Magit, CheatSheet ]
comments: true
---

In the past, I wrote about using [Magit in my Emacs workflow](http://www.mycpu.org/emacs-24-magit-magic/) which helped me tremendously.
This time, I want to dump all the git commands that I use frequently that were gradually ~~stolen~~ curated from the Internet over time.

Here's my cheat sheet of git commands. By simply going through the list
you will learn much more than you need for day-to-day activities.

But it also helps you get a step closer to becoming a git Guru.

Git cheat list

-   name of the current branch and nothing else (for automation)
    
    bash Emacs git rev-parse &#x2013;abbrev-ref HEAD

-   all commits that your branch has that are not yet in master
    
    bash Emacs git log master..<HERE\\<sub>COMES</sub>\\<sub>YOUR</sub>\\<sub>BRANCH</sub>\\<sub>NAME</sub>>

-   setting up a character used for comments
    
    bash Emacs git config core.commentchar
    <HERE\\<sub>COMES</sub>\\<sub>YOUR</sub>\\<sub>COMMENT</sub>\\<sub>CHAR</sub>>

-   fixing \`fatal: Could not parse object\` after unsuccessful revert
    
    bash Emacs git revert &#x2013;quit

-   view diff with inline changes
    
    bash Emacs git diff &#x2013;word-diff=plain master

-   view quick stat of a diff
    
    bash Emacs git diff &#x2013;shortstat master git diff &#x2013;numstat master git
    diff &#x2013;dirstat master

-   undo last just made commit
    
    bash Emacs git reset HEAD~

-   list last 20 hashes in reverse
    
    bash Emacs git log &#x2013;format="%p..%h %cd %<(17)%an %s"
    &#x2013;date=format:"%a %m/%d %H:%M" &#x2013;reverse -n 20

-   list commits between
    
    bash Emacs git log &#x2013;format="%p..%h %cd %<(17)%an %s"
    &#x2013;date=format:"%a %m/%d %H:%M" &#x2013;reverse
    &#x2013;after=2016-11-09T00:00:00-05:00 &#x2013;before=2016-11-10T00:00:00-05:00

-   try a new output for
    
    bash Emacs git diff &#x2013;compaction-heuristic &#x2026; &#x2013;color-words &#x2026;

-   enable more thorough comparison
    
    bash Emacs git config &#x2013;global diff.algorithm patience

-   restoring a file from a certain commit relative to the latest
    
    bash Emacs git checkout HEAD~<NUMBER> &#x2013; <RELATIVE\\<sub>PATH</sub>\\<sub>TO</sub>\\<sub>FILE</sub>>

-   restoring a file from a certain commit relative to the given commit
    
    bash Emacs git checkout <COMMIT\\<sub>HASH</sub>>~<NUMBER> &#x2013;
    <RELATIVE\\<sub>PATH</sub>\\<sub>TO</sub>\\<sub>FILE</sub>>

-   restoring a file from a certain commit
    
    bash Emacs git checkout <COMMIT\\<sub>HASH</sub>> &#x2013; <RELATIVE\\<sub>PATH</sub>\\<sub>TO</sub>\\<sub>FILE</sub>>

-   creating a diff file from unstaged changes for a ****specific folder****
    
    bash Emacs git diff &#x2013; <RELATIVE\\<sub>PATH</sub>\\<sub>TO</sub>\\<sub>FOLDER</sub>> changes.diff

-   applying a diff file

-   go to the root directory of your repository
    
    bash Emacs git apply changes.diff

-   show differences between last commit and currrent changes:
    
    bash Emacs git difftool -d

-   referring to:

-   last commits \`&#x2026; HEAD~1 &#x2026;\`
-   last 3 commits \`&#x2026; HEAD~3 &#x2026;\`

-   show the history of changes of a file
    
    bash Emacs git log -p &#x2013; ./Scripts/Libs/select2.js

-   ignoring whitespaces
    
    bash Emacs git rebase &#x2013;ignore-whitespace <BRANCH\\<sub>NAME</sub>>

-   pulling for fast-forward only (eliminating a chance for unintended
    merging)
    
    bash Emacs git pull &#x2013;ff-only

-   list of all tags
    
    bash Emacs git fetch git tag -l

-   archive a branch using tags
    
    bash Emacs git tag <TAG\\<sub>NAME</sub>> <BRANCH\\<sub>NAME</sub>> git push origin &#x2013;tags
    
    you can delete your branch now

-   get a tagged branch
    
    bash Emacs git checkout -b <BRANCH\\<sub>NAME</sub>> <TAG\\<sub>NAME</sub>>

-   list of all branches that haven't been merged to master
    
    bash Emacs git branch &#x2013;no-merge master

-   enable more elaborate diff algorithm by default
    
    bash Emacs git config &#x2013;global diff.algorithm histogram

-   list of all developers
    
    bash Emacs git shortlog -s -n -e

-   display graph of branches
    
    bash Emacs git log &#x2013;decorate &#x2013;graph &#x2013;all &#x2013;date=relative
    
    or
    
    bash Emacs git log &#x2013;decorate &#x2013;graph &#x2013;all &#x2013;oneline

-   remembering the password
    
    bash Emacs git config &#x2013;global credential.helper store git fetch
    
    the first command tells git to remember the credentials that you are
    going to provide for the second command

-   path to the global config
    
    bash Emacs ~/.gitconfig

-   example of a global config
-   viewing differences between current and other branch
    
    bash Emacs git difftool -d BRANCH\\<sub>NAME</sub>

-   viewing differences between current and stash
    
    bash Emacs git difftool -d stash

-   viewing differences between several commits in a diff tool
    
    bash Emacs git difftool -d HEAD@{2}&#x2026;HEAD@{0}

-   view all global settings
    
    bash Emacs git config &#x2013;global -l

-   delete tag
    
    bash Emacs git tag -d my-tag git push origin :refs/tags/my-tag

-   pushing tags
    
    bash Emacs git push &#x2013;tags

-   checking the history of a file or a folder
    
    bash Emacs git log &#x2013; <FILE\\<sub>OR</sub>\\<sub>FOLDER</sub>>

-   disabling the scroller
    
    bash Emacs git &#x2013;no-pager <&#x2026;>

-   who pushed last which branch
    
    bash Emacs git for-each-ref &#x2013;format="%(committerdate) %09 %(refname)
    %09 %(authorname)"

-   deleting remote branch
    
    bash Emacs git push origin :<BRANCH\\<sub>NAME</sub>>

-   deleting remote branch localy
    
    bash Emacs git branch -r -D <BRANCH\\<sub>NAME</sub>>
    
    or to sync with the remote
    
    bash Emacs git fetch &#x2013;all &#x2013;prune

-   deleting local branch
    
    bash Emacs git branch -d <BRANCH\\<sub>NAME</sub>>

-   list ****actual**** remote branchs
    
    bash Emacs git ls-remote &#x2013;heads origin

-   list all remote (fetched) branches
    
    bash Emacs git branch -r

-   list all local branches
    
    bash Emacs git branch -l

-   find to which branch a given commit belongs
    
    bash Emacs git branch &#x2013;contains <COMMIT>

