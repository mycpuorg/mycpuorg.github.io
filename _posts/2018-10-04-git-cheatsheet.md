---
layout: post
title: "Yet Another Git CheatSheet"
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

```
git rev-parse --abbrev-ref HEAD
```

<!-- -->

-   all commits that your branch has that are not yet in master

```
git log master..&lt;HERE\_COMES\_YOUR\_BRANCH\_NAME&gt;
```

<!-- -->

-   setting up a character used for comments

```
git config core.commentchar &lt;HERE\_COMES\_YOUR\_COMMENT\_CHAR&gt;
```


<!-- -->

-   fixing \`fatal: Could not parse object\` after unsuccessful revert

```
git revert --quit
```

<!-- -->

-   view diff with inline changes

```
git diff --word-diff=plain master
```

<!-- -->

-   view quick stat of a diff

```
git diff --shortstat master
git diff --numstat master
git diff --dirstat master
```


<!-- -->

-   undo last just made commit

```
git reset HEAD\~
```

<!-- -->

-   list last 20 hashes in reverse

```
git log --format="%p..%h %cd %&lt;(17)%an %s"--date=format:"%a %m/%d %H:%M" --reverse -n 20
```


<!-- -->

-   list commits between

```
git log --format="%p..%h %cd %&lt;(17)%an %s"--date=format:"%a %m/%d %H:%M" --reverse --after=2016-11-09T00:00:00-05:00 --before=2016-11-10T00:00:00-05:00
```


<!-- -->

-   try a new output for

```
git diff --compaction-heuristic ... --color-words ...
```

<!-- -->

-   enable more thorough comparison

```
git config --global diff.algorithm patience
```

<!-- -->

-   restoring a file from a certain commit relative to the latest

```
git checkout HEAD\~&lt;NUMBER&gt; --&lt;RELATIVE\_PATH\_TO\_FILE&gt;
```


<!-- -->

-   restoring a file from a certain commit relative to the given commit

```
git checkout &lt;COMMIT\_HASH&gt;\~&lt;NUMBER&gt; --&lt;RELATIVE\_PATH\_TO\_FILE&gt;
```


<!-- -->

-   restoring a file from a certain commit

```
git checkout &lt;COMMIT\_HASH&gt; --&lt;RELATIVE\_PATH\_TO\_FILE&gt;
```


<!-- -->

-   creating a diff file from unstaged changes for a \***specific
    folder**\*

```
git diff -- &lt;RELATIVE\_PATH\_TO\_FOLDER&gt;changes.diff
```


<!-- -->

-   applying a diff file

<!-- -->

-   go to the root directory of your repository

```
git apply changes.diff
```

<!-- -->

-   show differences between last commit and currrent changes:

```
git difftool -d
```

<!-- -->

-   referring to:

<!-- -->

-   last commits \`... HEAD\~1 ...\`
-   last 3 commits \`... HEAD\~3 ...\`

<!-- -->

-   show the history of changes of a file

```
git log -p -- ./Scripts/Libs/select2.js
```

<!-- -->

-   ignoring whitespaces

```
git rebase --ignore-whitespace &lt;BRANCH\_NAME&gt;
```

-   pulling for fast-forward only (eliminating a chance for
    unintended merging)

```
git pull --ff-only
```

<!-- -->

-   list of all tags

```
git fetch git tag -l
```

<!-- -->

-   archive a branch using tags

```
git tag &lt;TAG\_NAME&gt; &lt;BRANCH\_NAME&gt;
git push origin --tags
```


    you can delete your branch now

<!-- -->

-   get a tagged branch

```
git checkout -b &lt;BRANCH\_NAME&gt; &lt;TAG\_NAME&gt;
```

<!-- -->

-   list of all branches that haven't been merged to master

```
git branch --no-merge master
```

<!-- -->

-   enable more elaborate diff algorithm by default

```
git config --global diff.algorithm histogram
```

<!-- -->

-   list of all developers

```
git shortlog -s -n -e
```

<!-- -->

-   display graph of branches

```
git log --decorate --graph --all --date=relative
```

    or

```
git log --decorate --graph --all --oneline
```

<!-- -->

-   remembering the password

```
git config --global credential.helper store
git fetch
```

    the first command tells git to remember the credentials that you are
    going to provide for the second command

<!-- -->

-   path to the global config

```
\~/.gitconfig
```

<!-- -->

-   example of a global config\
    \
-   viewing differences between current and other branch

```
git difftool -d BRANCH\_NAME
```

<!-- -->

-   viewing differences between current and stash

```
git difftool -d stash
```

<!-- -->

-   viewing differences between several commits in a diff tool

```
git difftool -d HEAD@{2}...HEAD@{0}
```

<!-- -->

-   view all global settings

```
git config --global -l
```

-   delete tag

```
git tag -d my-tag git push origin :refs/tags/my-tag
```

<!-- -->

-   pushing tags

```
git push --tags
```

<!-- -->

-   checking the history of a file or a folder

```
git log -- &lt;FILE\_OR\_FOLDER&gt;
```

<!-- -->

-   disabling the scroller

```
git --no-pager &lt;...&gt;
```

<!-- -->

-   who pushed last which branch

```
git for-each-ref --format="%(committerdate)%09 %(refname) %09 %(authorname)"
```


<!-- -->

-   deleting remote branch

    bash Emacs git push origin :&lt;BRANCH\_NAME&gt;

<!-- -->

-   deleting remote branch localy

    bash Emacs git branch -r -D &lt;BRANCH\_NAME&gt;

    or to sync with the remote

    bash Emacs git fetch --all --prune

<!-- -->

-   deleting local branch

    bash Emacs git branch -d &lt;BRANCH\_NAME&gt;

<!-- -->

-   list \***actual**\* remote branchs

    bash Emacs git ls-remote --heads origin

<!-- -->

-   list all remote (fetched) branches

    bash Emacs git branch -r

<!-- -->

-   list all local branches

    bash Emacs git branch -l

<!-- -->

-   find to which branch a given commit belongs

    bash Emacs git branch --contains &lt;COMMIT&gt;


