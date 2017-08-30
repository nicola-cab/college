/*
 * MC.c
 *
 *  Created on: Jan 26, 2012
 *      Author: nicola
 */

#include "MC.h"

#ifdef MC_REPLICA__

bool Wrapper_MC_Ask_MD_Simulation( string discovery_path,
		unsigned short int mc_id,
		molecular_t * data,
		MD_response * resp)
{

	bool status = FALSE;

	debug_MC_log("MC Wrapper simulation in sync way");


	//------------------------------------------------------
	//Discovery part and id of mc replica
	set_Replica_ID(mc_id);
	char mc_discover_path[100];
	sprintf(mc_discover_path,"%s%s",discovery_path,SHARED_MEMORY_DISCOVERY_FILE);
	set_path_discovery_file(mc_discover_path);
	//------------------------------------------------------

	//prepare data first
	request_t  request;

	status = MC_Init();


	if(TRUE == status )
	{
		debug_MC_log("MC Wrapper connected to MD!");

		prepare_MD_request(&request,
				MD_SIMULATION_OP,
				data);


		//ok I'm connected to a MD... I need to do exchange
		status = exchange(&request,&(resp->response));
		if( TRUE == status)
		{
			if(resp->response.type_response > 0)
			{
				debug_MC_log("MC operation ends with success");
				resp->op_state = TRUE;
			}
			else
			{
				debug_MC_log("MC operation ends with unsuccess");
				resp->op_state = FALSE;
			}
		}
		else
		{
			error_MC_log("MC Wrapper - MD, exchange fail!!");
			resp->op_state = FALSE;
		}

		if(status == FALSE)
		{
			debug_MC_log("MC closed incorrectly ... end of simulation");
		}
		else
		{
			debug_MC_log("MC closed correctly ... end of simulation");
		}
	}
	else
	{
		error_MC_log("MC_Wrapper : Impossible to connect to MD ");
		resp->op_state = FALSE;
	}

	status = MC_Down();
	return status;
}

bool Wrapper_MC_Exit()
{
	bool status = FALSE;

	debug_MC_log("MC_Wrapper close client");

	status = MC_Down();

	return status;
}

#endif
