hotel_wakeup
============

C project for OS. Due 18:00 7th November 2014

folder for submission in one of our accounts:

````
~/ca321-1415/assignment1
```

|Name|Student Number|
|:-------------:|:--------:|
|Adam O'Flynn | 12378651|
|Conor Smyth | 12452382|

Overview
--------

This assignment is about POSIX threads.

You work for a really big hotel with thousands of rooms. Guests ring you to request wake-up calls. Until now you have been 
handling everything manually, writing down the times and room numbers on post-its, keeping them in earliest-first order and dialling each room at the appropriate time. With so many rooms, however, your manual system has begun to creak under the load: your supply of post-its is running low and keeping them in earliest-first order as new calls come in has become a nightmare. You decide to write a computer program to help you.

Approaching your manager, you find she is keen on the idea and says there might be a promotion in it for you if you get this right. She says you can implement your system on the hotel's computer (by lucky coincidence it runs the same version of Linux as the machines in the labs). As the computer already runs the hotel's web server and reservations database she requests you address both performance and efficiency concerns in your solution.

Assignment
----------

Write a C program that:

* Continuously generates future wake-up calls
* Prints out a message each time a wake-up call is generated
* Prints out a message when it is time to make a wake-up call
* Displays a count of expired and pending wake-up calls
* On receiving CTRL-C shuts down all threads with each thread freeing any resources it owns or has allocated and each thread then printing a farewell message

Approach
--------

Your program carries out two distinct and separate tasks (and so a multithreaded approach seems suitable):

* Generating (and logging) wake-up calls
* Making wake-up calls
* Generating a wake-up call consists of generating a random room number and a random future wake-up time. The only guests who stay at the hotel are Linux users who (rather conveniently) specify their wake-up times in Linux time (i.e. the number of seconds since midnight 1 January 1970).

We cannot tell in advance how many wake-up calls will be logged so a dynamic data structure must be used for storing them. You are to use a heap at whose root is always stored the earliest (next due) wake-up call. An example might look like this. You will have to research and implement a heap data structure. (Its implementation as a resizable array and the methods it supports are quite simple. Heaps will be covered by most books on data structures and algorithms.)

There is also a particular pthread library routine pthread_cond_timedwait, which will be particularly useful in solving the problem. See the FAQ below for some issues with using this function.

Deliverables
------------

You are required to deliver the following in text files:

* One C source code file containing your solution
* design.txt
* problems.txt

Marks are awarded for solving the assignment, solving it efficiently and demonstrating your understanding of problems, issues and solutions through comments. Any shortcomings in your solution must be clearly identified. 
Marks will be deducted for:

* Overly-complicated solutions
* Failing to cleanly shutdown threads
* Excessive resource usage
* Busy-waiting
* Solutions that unnecessarily rely on calls to sleep to function correctly
* Commented out code
* Undocumented shortcomings
* Lines longer than 80 characters
* Unrequested additional "features"
* Failing to meet the requirements listed above

Provide a brief description of how your solution functions in design.txt. In problems.txt provide an explanation of any shortcomings or weaknesses in your solution. Failure to list any shortcomings indicates you are not aware of them and marks will be deducted accordingly.

What's it worth?
----------------

15% of your overall mark.

FAQ
---

```
Q. I am not in a project team. What should I do?
A. E-mail me as soon as possible and I'll try to put you in touch with other students in the same position. If you leave e-mailing me too late I will not accept your submission and you will receive a mark of zero.

Q. I was sick during the period we had to work on the assignment. What can I do?
A. Submit a doctor's note to the faculty office and to the chair of your degree programme covering the period in question. The programme board will then make a decision regarding your eligibility to repeat over the summer.

Q. What would a working solution look like?
A. It might produce output like that shown here. (Note output should vary significantly from one run to the next.)

Q. Can I implement my heap using a resizable array?
A. Yes. I recommend you do so.

Q. How do I get the current time in C?
A. Use a call to the built-in time() function.

Q. I am trying to check whether my call to pthread_cond_timedwait timed out by comparing its return value to ETIMEDOUT but the ETIMEDOUT constant is not recognised and compilation fails. What can I do?
A. You can add #define ETIMEDOUT 110 to your code and use it from then on.

Q. I have no idea how to solve this assignment. Will you help me?
A. There are numerous ways to solve it, some better than others. If you are stuck you could try a simpler approach that perhaps uses more resources or is less efficient than an optimal solution. You will be penalised for doing so but a solution is better than none at all.

Q. How often are wake-up calls generated?
A. They are generated by a thread that sleeps for a random number (0-5) of seconds before generating the next one.

Q. How far into the future are wake-up calls?
A. A random number (0-100) of seconds.

Q. How many rooms are there in the hotel?
A. So many that pre-allocating resources for each is not an option.

Q. Can I create one thread per alarm?
A. No. With potentially thousands of alarms to be registered this is not a good idea. The assignment requires that solutions pay attention to both performance and efficiency. Among other things this entails tracking alarm calls in a data structure.
```
