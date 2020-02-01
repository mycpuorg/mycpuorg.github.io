---
layout: post
title: "One Simple (Git) Trick To Document Your Project"
excerpt: "Super helpful Git Hack"
tags: [Git, Emacs, Linux, Magit, CheatSheet]
comments: true
---
The big challenge today's software developer faces is not about having to come
up with his/her implementation of a problem. That's easy, not fast, but easy.
The biggest challenge a developer faces is to do with inheriting/understanding
a codebase. This typically entails looking at strange choices the original
developer made and, let's face it, bitching about how bad the codebase is. Most
times, you are consciously aware that you, in all your wisdom and hackery, would
not have made a better choice. In fact, some times you are confident you would
have sucked in the situation that brought the orginal developer to make this
"dumb" choice.

I think a majority of times, the issue arises from the fact that the original
developer fails to convey their thoughts diligently. No matter how rich the
documentation of a project is, the "feeling" of the developer while making the
commit is never conveyed so intimately as the commit text.

So, here's my "time machine" idea. Make the git commit text as rich and relevant
as possible. This can be as crazy as you often find in the Linux Kernel or something
more moderate as a 5-liner that succinctly describes all you need to know about
the commit. In general, having a structure to the commit text
always helps. It does not have to vex eloquent about the process through which
the solution or "solution" was arrived at. It can simply have a predictable
structure, something as follows might be worthwhile considering:

### Structure of a Good Commit Text:
```plain
This is a one line summary of the commit text

This is a paragraph describing the problem in simple present tense. It can
describe why the issue exists, any history, any causal events, any triggers
etc. can be described here.

This is a paragraph describing the solution to the above problem in simple
present tense. It should describe why this change is needed as clearly as
possible. If there any strange implementation choices this is a good place to
describe them. Do not complicate it, just put your throughts down in simple
words. Don't be afraid to describe and log any thoughts about mini decisions
made such as a choice of data structure, any choice to call a particular library
function instead of implementing it (or vice versa). More details are acceptable
here.

This is a paragraph describes what you have tested and their results
```

Just doing this conveys more than enough info to the reader who does not have to
switch to the documentation any more. In fact, now they should have a
blow-by-blow account of the detailed history of the project. Of course, this may
not be ideal to get the big picture or the overall design of the project. This
is the stuff that comes after all that. A single, well written commit can
actually save the reader several tens of hours of digging around the code base,
debugging, or just get to a productive stage and help contribute to the project.

### Best For The Last:
Git allows us to define a commit template that it will bring up each time you
are making a new commit. Defining the structure within such a template can
do most of the heavy lifting for you. It also prevents you from hitting the
"Writer's Block" since the structure of what needs to be added is already
present. You need to define this template in ``~/.git-commit-template.txt

Open ``~/.gitconfig``
```
[commit]
    # file containing the commit text template
    template = ~/.gitmessage
```

```
    One liner describing the change(<=50 chars)

    PROBLEM: Description of the problem goes here

    SOLUTION: Description of the solution goes here

    TESTS:
    Explain the how you tested the change. If possible also provide links
    where your working solution is hosted so that reviewer can test it.
    How you tested this change.

```

Of course, there are all sorts of other rigorous templates possible and even
used in some of the projects. But I personally feel that this particular
template hits the sweet spot of practical and useful.
All [my Linux Kernel
Commits](https://www.codeaurora.org/cgit/quic/la/kernel/msm-3.10/log/?qt=author&q=Manoj+Rao&h=msm-3.10)
follow this pattern.
