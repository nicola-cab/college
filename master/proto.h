/*
 * proto.h
 *
 *  Created on: Jan 23, 2012
 *      Author: nicola
 *
 *      Description: In proto.h/c files will be defined all the FSM to compose and decompose a message which is exchanged between MD and MC
 */

#ifndef PROTO_H_
#define PROTO_H_

#include "defs.h"

/**
 * Prepare a MD request (used by MC)
 *
 * @param: type_op
 * @param: size_x
 * @param: size_y
 * @param: size_z
 * @param: x
 * @param: y
 * @param: z
 *
 * @return: true ... if the request has been built correctly
 * */
bool prepare_MD_request(request_t * request,
		STATUS type_op,
		molecular_t * data);

/**
 * Prepare a MC response (used by MD)
 *
 * @param: type_response
 * @param: errorMsg
 * @param: MD_status
 * @param: size_x
 * @param: size_y
 * @param: size_z
 * @param: x
 * @param: y
 * @param: z
 *
 * @return: bool ... true if operations works fine
 * 					 false otherwise
 * */
bool prepare_MC_response(response_t * response,
		STATUS type_response,
		string errorMsg,
		unsigned short int size_ErrorMsg,
		STATUS MD_status,
		molecular_t * data);

#endif /* PROTO_H_ */
