/*
 * proto.c
 *
 *  Created on: Jan 23, 2012
 *      Author: nicola
 */

#include "proto.h"


bool prepare_MD_request(request_t * request,
		STATUS type_op,
		molecular_t  * data)
{
	debug_MC_log("MC compose a request for MD");
	bool status = FALSE;

	switch(type_op)
	{
		case MD_STATUS_OP:
		{
			debug_MC_log("Request MD Status");

			//Compose a request to ask the status of MD
			request->type_op = MD_STATUS_OP; //stop no other info is needed
			status = TRUE;
			break;
		}

		case MD_SIMULATION_OP:
		{
			debug_MC_log("Request MD simulation");

			request->type_op = MD_SIMULATION_OP;

			//---------------------------------------------------------------------
			//Simulation part

			//set the pointer of request
			request->data = *data;

			status = TRUE;

			break;
		}
		case MD_CLOSE_SIM_OP:
		{
			request->type_op = type_op;
			status = TRUE;
			break;
		}
		default:
			debug_MC_log("No request operation allowed with the operand passed as type_op");
			break;
	}


	char dbg_msg[40];
	memset(dbg_msg, 0 ,40);
	sprintf(dbg_msg,"prepare_MD_request ends with status %d", status);
	debug_MC_log(dbg_msg);

	return status;
}


bool prepare_MC_response(response_t *response,
		STATUS type_response,
		string errorMsg,
		unsigned short int size_ErrorMsg,
		STATUS MD_status,
		molecular_t * data)
{
	bool status = FALSE;

	debug_MD_log("MD prepare response for MC request");
	response_t l_response;

	memset((void *)errorMsg,' ',size_ErrorMsg);

	if( type_response < 0)
	{
		//there is a error!
		debug_MD_log("MD has recognized a error.. prepare error response ");

		switch(type_response)
		{
			case MD_GENERIC_ERROR:
			{
				debug_MD_log("MD is going to prepare a MD_GENERIC_ERROR msg");
				l_response.type_response = type_response;
				strcpy(l_response.error_msg , errorMsg);
				status = TRUE;
				break;
			}
			case MD_COMMUNICATION_ERROR:
			{
				debug_MD_log("MD is going to prepare a MD_COMMUNICATION_ERROR msg");
				l_response.type_response = type_response;
				strcpy(l_response.error_msg, errorMsg);
				status = TRUE;
				break;
			}
			case MD_SIMULATION_ERROR:
			{
				debug_MD_log("MD is going to prepare a MD_SIMULATION_ERROR msg");
				l_response.type_response = type_response;
				strcpy(l_response.error_msg, errorMsg);
				status = TRUE;
				break;
			}
			default:
				debug_MD_log("No error msg possible with this type_response");
				break;
		}
	}
	else
	{
		//this is a response
		debug_MD_log("MD compose response");

		switch(type_response)
		{
			case MD_STATUS_RESPONSE:
			{
				debug_MD_log("MD is going to prepare a MD_STATUS_RESPONSE msg");
				l_response.type_response = type_response;
				l_response.MD_status = MD_status;
				status = TRUE;
				break;
			}

			case MD_SIMULATION_RESPONSE:
			{
				debug_MD_log("MD is going to prepare a MD_SIMULATION_RESPONSE msg");

				strcpy(l_response.error_msg, errorMsg);

				l_response.type_response = type_response;


				//Set size for every array used by molecular dynamic
				l_response.data = *data;

				status = TRUE;
				break;
			}

			default:
				debug_MD_log("No response is possible with type_response");
			break;
		}
	}


	//debug
	debug_MD_log("MD has completed follow response");
	if(type_response < 0)
	{
		debug_MD_log("MD completed error response");
	}
	else
		debug_MD_log("MD completed success response");



	*response = l_response;

	return status;
}

