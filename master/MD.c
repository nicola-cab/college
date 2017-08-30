/*
 * MD.c
 *
 *  Created on: Jan 26, 2012
 *      Author: nicola
 */

#include "MD.h"

#ifdef MD_REPLICA__

bool MD_up_Server()
{
	bool status = FALSE;


	debug_MD_log("MD Wrapper initialize server");

	//Start the MD server
	status = MD_Init();

	if(FALSE == status)
		error_MD_log("Unable to star MD server");


	return status;
}


bool MD_down_Server()
{
	bool status = FALSE;

	debug_MD_log("MD Wrapper close server");

	status = MD_Down();

	return status;
}

#endif
