# Distributed compuation project
=====================

***Description:***  

The project was designed as toy in order to play with Berkeley sockets on linux. On top of a tcp/ip socket I built a multi-threaded server and a small client application along with a gtk based UI application for ubuntu.  

The main goal was to have a catalog of multimedia content queryable by several client applications. In order to have a way to put in communication client and server I defined and implemented an application level protocol. The download phase was itself carried into a separate communication flow implemented by a different thread, thus supporting multiple downloads in parallel (max 5 downloads). 
