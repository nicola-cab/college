/*
 * MD_sock.h
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifndef MD_SOCK_H_
#define MD_SOCK_H_

#include "proto.h"
#include "MD_MC_shared.h"

/**
 * MD read from socket
 *
 * @param: sd ...socket desciptor
 * @param: request ... the MC request read from socket
 *
 * @return: true .. if the read works fine
 * 			false ... otherwise
 * */
bool MD_read(int sd,request_t * request);

/**
 * MD write to socket
 *
 * @param: sd ... socket descriptor
 * @param: response ... the MD response to MC to be written to socket
 *
 * @return: true .. if the write works fine
 * 			false ... otherwise
 * */
bool MD_write(int sd, response_t * response);


#endif /* MD_SOCK_H_ */
