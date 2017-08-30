/*
 * MC_comm.h
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifndef MC_COMM_H_
#define MC_COMM_H_

#include "MD_MC_shared.h"
#include "MC_discover.h"
#include "MC_sock.h"
#include "proto.h"


/**
 * Initialization of MC client
 *
 * @return: bool     true .. if operation ends correctly
 * 					 false .. otherwise
 * */
bool MC_Init();

/**
 * Request of MD simulation from MC
 *
 * @param: request_t* -->  a request from MC to MD simulation
 * @param: response_t* --> a response to MC from MD
 *
 * @return: bool ... true if the request from MC has been sent to MD
 * 					 false otherwise
 * */
bool request(request_t * , response_t * );

/**
 * Close MC client
 * */
bool MC_Down();

/**
 *
 * Connection of the client (MC) to the server service (MD)
 *
 * @return: bool    true .. if operation ends correctly
 * 					false .. otherwise
 * */
bool Client_Connect(md_discovery_t *);

/**
 * Low level routine which implements the exchange between MD and MC
 *
 * @param: request_t* req --> the request
 * @param: response_t *res --> the response
 * */
bool exchange(request_t * , response_t * );

#endif /* MC_COMM_H_ */
