/*
 * MD_comm.h
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifndef MD_COMM_H_
#define MD_COMM_H_


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <unistd.h>


#include "MD_MC_shared.h"
#include "MD_discover.h"
#include "MD_sock.h"
#include "proto.h"
#include "orac.h"

extern md_discovery_t * MD_Unit ;
extern md_discovery_t * MDs;


//MAX WAITING QUEUE ON SEM
#define MAX_QUEUE 64

//MAC CONNECTION
#define MAX_CONN 5

typedef struct
{
	int sd;
}md_info_thread_t;


/**
 * Setting MD Server network interface to allow MC to discover a MD
 * Server
 *
 * @param: network_interface --> the name of interface which MC can use to discovery MD server
 * */
void set_MD_Network_interface(char * network_interface);

/**
 * Initialization of MD server
 *
 * @return: bool     true .. if operation ends correctly
 * 					 false .. otherwise
 * */
bool MD_Init();

/**
 * Close MD server replica
 * */
bool MD_Down();

/**
 *
 *  Startup of the server side process (MD)
 *
 *	@return: bool    true .. if operation ends correctly
 * 					 false .. otherwise
 * */
bool Server_Startup();

/**
 * Run the MD Server, this call never returns as long as MD service is up
 *
 * */
void Server_Run();

/**
 * Here will be changed in atomic way the MD_task_status
 *
 * @return: true .. if the status variable is set correctly
 * */
bool atomic_set_MD_status(STATUS task_status);

/**
 * @param current_Status ... the current status of the MD replica
 *
 * @return: true .. if the read operation ends correctly
 * 			false ... otherwise
 * */
bool atomic_get_MD_status(STATUS *current_status);



#endif /* MD_COMM_H_ */
