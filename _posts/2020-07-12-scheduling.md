---
layout: post
title: "Scheduling Your Life Like An Engineer"
excerpt: "Scheduling tasks in real life"
tags: [Productivity, scheduler, scheduling]
comments: true
---
No matter which stage of career you are in, managing time and tasks become a
matter of utmost priority. Paul Graham's essay on [Maker's
Schedule](http://www.paulgraham.com/makersschedule.html) explains a good way
to think about typical programmers vs typical managers schedule.

Beyond this, there is a more specific problem that most programmers battle with
intra-project prioritization. You break down a large project into sub tasks and
how would you schedule them? Especially if the project is for a client (assuming
it doesn't entail a monolithic purpose to it, since this is extremely rare). For
example: it rarely is "Make me a new text editor"

With that said, if you looked into how Operating Systems handle this sort of a
thing we can see the various scheduling policies available. In Linux Kernel,
there are a ton of such policies:
+ Completely Fair Scheduler (CFS)
+ FIFO or FCFS (First In First Out)
+ Earliest Deadline First or Deadline Scheduling (EDF)

## Earliest Deadline First (EDF)
EDF has produces an optimal scheme for minimizing the maximum lateness.

![](./images/EDD.png)

If your goal is to simply minimize the amount of lateness only, then this is
it. It is the optimal scheme. However, this has the downside of producing a lot
of tasks can be late albeit by a smaller margin. What if you want to optimize
for minimizing the number of tasks that are late?

## Moore's Algorithm
Moore's Algorithm optimizes for the number of tasks that are late. It does so by
first ordering the tasks as per EDF scheme then trying to spot a largest job that
results in delaying a later task in the pipeline. It then moves this large job
to the end of the queue.

![](./images/MooresAlgo.png)

You can break down a big problem into smaller tasks, by simply using the better
of these two schemes recursively you have solved your schedule. You have even
mathematically optimized your scheduling scheme. Another important, yet
underrated benefit is that you have drastically reduced your cognitive load on
the planning/scheduling aspect. Instead you get to look deeper into the design
and other technical parts of the project.
