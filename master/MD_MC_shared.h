/*
 * MD_MC_shared.h
 *
 *  Created on: 04/mag/2012
 *      Author: akhela
 */

#ifndef MD_MC_SHARED_H_
#define MD_MC_SHARED_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <netinet/in.h>
#include <stropts.h>
#include <arpa/inet.h>

#include "Common.h"
#include "logger.h"
#include "proto.h"

//Num of items to write on socket
#define MAX_MTU 186

//listen port
#define PORT 10200

#define SHARED_MEMORY_DISCOVERY_FILE "MD_Units.list" //To change when will be the deploy ...

//status of MC and MD
#define MD_UP 0
#define MD_DOWN 1
#define MC_UP 2
#define MC_DOWN 3

//STATUS OF MD UNIT
#define MD_BUSY 4
#define MD_READY 5
#define MD_ERROR 6

struct md_item
{
	string MD_interface;   //name of interface (e.g eth0)
	string MD_ip_address; //ip address of MD Unit in the cluster, which allow MC to discover the service
	STATUS MD_status;     //status of MD Unit service

	struct md_item * next;  //pointer used in case of linked-list

};

typedef struct md_item md_discovery_t;


/**
 * Setting the discovery file path where to save and to load
 * the list of available MD
 *
 * @param path --> the path where save discovery file
 *
 * @return --> true if the path is set correctly
 * */
bool set_path_discovery_file(string path);

/**
 * Getting the path where the discovery where load/save the discovery file
 *
 * return --> the string containing the path where discovery file is or NULL
 * if no path has been found (BAD ERROR ... this cause the system abort)
 * */
string get_path_discovery_file(void);

/**
 * @param stream --> array stream where put the merge of arrays
 * @param data --> struct which define molecular dynamic data used
 *
 * @return: true if there is success, false otherwise
 * */
bool serialize_arrays(TYPE_MOLECULAR stream[],
		molecular_t * data);

/**
 * @param stream --> array stream where put the merge of arrays
 * @param size_x --> size of x array
 * @param size_y --> size of y array
 * @param size_z --> size of z array
 * @param x --> array
 * @param y --> array
 * @param z --> array
 *
 * @return: true if there is success, false otherwise
 * */
bool deserialize_arrays(TYPE_MOLECULAR stream[], molecular_t * data);


/**
 * Write on socket (common routine, used by MC and MD both)
 *
 * @param: stream ... stream of data to write on socket
 * */

bool write_on_Sock(int socket, TYPE_MOLECULAR * stream, unsigned int size);

/**
 * Read on socket (common routine, used by MC and MD both)
 *
 * @param: stream ... stream of data read from socket
 * */
bool read_from_Sock(int socket, TYPE_MOLECULAR * stream, unsigned int size);


#endif /* MD_MC_SHARED_H_ */
