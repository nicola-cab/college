# Master's degree project
=====================

***Description:***  

The aim of this project was to bring and build a library for a fortran based algorithm to simulate the molecular dynamic of simple proteins into the the era of new computing, trying to increase the complexity of the proteins that were computable at that time, exploiting the capabilities of a HPC cluster.  

The program used to simulate the molecular dynamic was writen in fortran, thus not suitable for implementing complex system programming tasks like using socket programming and/or using linux system calls. Moreover it was not suitable to be scaled up.  

OpenMPI was already used by the fortran application, but the algorithm itself was bound to the max number of cores available on the machine and for the Amdahl's law I noticed that we could not get any gain beyound 16 cores per machine.  

Thus the idea of writing an embarrassingly parallel algorithm looked the right decision to take, in order to solve simple problems using always the same algorithm but submitting different inputs in form of PTB files.    

I wrote a entry point set of routines in order to allow a C application to talk with fortran, solving common problems like matrix row, column order and representation and so on. On top of this I implemented 2 lightweight libraries, one to transform the fortran parallel algorithm into a server and the other one to transform a monte carlo control simulation algorithm into a client for each molecular dynamic replica running in the cluster. These 2 applications were able to communicate to each other using a custom designed application protocol running on top of TCP sockets.  

**Molecular dynamic server:**  
The logic adopted to implement this was simple. Select always core 1 as the core dedicated to run the instance of the server (essentially implementing the protocol), gathering a set of molecular position arrays, plus forces and any other physic parameter, scattering them among the other cores and re-gathering the output to be returned to the monte carlo controller that asked for a simulation

**Monte carlo client:**    
This application was already written and the logic implemented. The only one thing I implemented was the socket programming part in order to implement the protocol and the wrap-up of such functionality into a set of APIs

**Protocol and system :**  
The protocol was simple enough. At the beginning each replica of the server running in the cluster had to write  into a common area its ip and port in order to be contacted by the monte carlo controller. As soon the monte carlo controller wanted to initiate a simulation it needed to send the appropriate message and send the appropriate physic vector parameters to start the simulation. The protocol was able to take care of busy servers and re-allocation of computations among the cluster.
