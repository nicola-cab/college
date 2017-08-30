/*
 * defs.h
 *
 *  Created on: Jan 23, 2012
 *      Author: nicola
 *
 *      Description: In this header file will be defined all data structures which represents all kind of messages exchanged between MD and MC
 */

#ifndef DEFS_H_
#define DEFS_H_

#include "logger.h"
#include "fortran_interface.h"

//type of request
#define MD_REQUEST 0
#define MD_STATUS_OP 			MD_REQUEST + 1
#define MD_SIMULATION_OP 		MD_REQUEST + 2
#define MD_CLOSE_SIM_OP			MD_REQUEST + 3

//errors
#define MD_GENERIC_ERROR 	-1
#define MD_COMMUNICATION_ERROR -2
#define MD_SIMULATION_ERROR -3

//type of response
#define MD_RESPONSE 			100
#define MD_STATUS_RESPONSE 		MD_RESPONSE + 1
#define MD_SIMULATION_RESPONSE 	MD_RESPONSE +2

typedef struct molecular_t_
{
	//----------------------------------------------------------//
	//Simulation

	//Size of molecular arrays

	unsigned int size_mapnl;
	unsigned int size_p0;
	unsigned int size_pg;
	unsigned int size_pcm;
	unsigned int size_gh;

	//Molecular arrays

	int * mapnl_d;

	TYPE_MOLECULAR * xp0_d;
	TYPE_MOLECULAR * yp0_d;
	TYPE_MOLECULAR * zp0_d;

	TYPE_MOLECULAR * xpg_d;
	TYPE_MOLECULAR * ypg_d;
	TYPE_MOLECULAR * zpg_d;

	TYPE_MOLECULAR * xpcm_d;
	TYPE_MOLECULAR * ypcm_d;
	TYPE_MOLECULAR * zpcm_d;

	TYPE_MOLECULAR * gh_d;


}molecular_t;

typedef struct request_t_
{
	//type of operation required
	STATUS type_op;

	//----------------------------------------------------------//
	//Molecular simulation
	molecular_t data;

}request_t;

typedef struct response_t_
{
	//----------------------------------------------------------//
	//Type of operation
	STATUS type_response;

	//----------------------------------------------------------//
	//Error
	char error_msg[80];  //if type_response is a error

	//----------------------------------------------------------//
	//MD Status (Only for status request)
	STATUS MD_status;  //if type_response is a status response in this field will stored the MD status

	//----------------------------------------------------------//
	//Molecular simulation
	molecular_t data;

}response_t;

#endif /* DEFS_H_ */
