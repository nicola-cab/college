# College
=================

A couple of projects developed during college.

1. ***bachelor's thesis***
Essentially a linux kernel module designed to be triggered by a particular patched version of the linux kernel 2.6 in order to dump scheduler and interrupt (hard/soft) activities seamlessly, without impacting too much the whole system activities. The events are collected and dump into a file by a user space application.
Technologies: C, linux kernel 2.6, linux misc kernel mdules, linux system programming 

2. ***master's thesis***
A distributed parallelized C/C++ application that uses OpenMPI to run several replicas on different nodes inside a linux HPC cluser in order to implement an embarrassingly parell algorithm creating a sort of cluster of fortran based molecular dynamic algorithms, running separetly as separate services contacted by a client monte carlo replica running inside the same cluster in order to scale up the application and run complex protein molecular dynamic simulations  

3. ***distributed network application***
A C client/server application to implement an application protocol on top of tcp berkeley sockets for linux, in order to discover and download multimedia files into a distributed network of nodes. 
