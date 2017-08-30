# Bachelor's degree project
=====================

***Description:***  
The project was aimed to get a reliable and efficient way for monitoring the events inside the linux kernel. In particular the main goal was to have a way of dumping the scheduler contex switches and all the hard/soft interrupts.   
  
The main problem was to do so in the fastest way possible withouth impacting too much the system itself. We had a linux kernel 2.6 recompiled within a couple of function pointers defined globally in order to be triggered for every context switch and interrupt handler invokation.  

The kernel was compiled for a particular embedded system, but I developed the solution on a vanilla kernel because all the function pointers were inserted into the part of the kernel that sits on top of the hardware specific code.  

All the events where collected and dumped into an user space/kernel space circular buffer. The dumping was done under critical section and the buffer moved into user space only when it was filled completely.  

The user space application is simple and composed by 2 tasks, one for reading the buffer (triggered by the kernel itself) and the other one for pushing data into a binary file, read by a java plotting application (I did not touch that code too much, except for implementing a down sampling mechanism to cope with all the events, then I did not put it in here).

**Note: code can be fuzzy :-)**
