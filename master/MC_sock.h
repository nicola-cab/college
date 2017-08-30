/*
 * MC_sock.h
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifndef MC_SOCK_H_
#define MC_SOCK_H_

#include "proto.h"
#include "MD_MC_shared.h"

#include <sys/types.h>
#include <sys/socket.h>

/**
 * MD read from socket
 *
 * @param: response ... the MD response for the MC request
 *
 * @return: true .. if the read works fine
 * 			false ... otherwise
 * */
bool MC_read(response_t * response);

/**
 * MC write to socket
 *
 * @param: response ... the MC request to send to MD
 *
 * @return: true .. if the write works fine
 * 			false ... otherwise
 * */
bool MC_write(request_t * request);


#endif /* MC_SOCK_H_ */
