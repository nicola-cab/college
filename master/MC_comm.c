/*
 * MC_comm.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifdef MC_REPLICA__


#include "MC_comm.h"

extern int MC_sock;

//list of MDs
md_discovery_t * MDs = NULL;

//Status variable
STATUS MC_task_status = MC_DOWN;

//status variable to manage the MD discovery process
bool MDs_list_discoverd = FALSE;


bool MC_Init()
{
	bool status = FALSE;
	bool flag_MD_found = FALSE;
	MDs = (md_discovery_t*)malloc(sizeof(md_discovery_t));  //head of the list
	char dbg_msg[80];
	memset(dbg_msg, 0 , 80);
	int size;


	if(MC_DOWN == MC_task_status)
	{
		debug_MC_log("MC Client initialization!!");

		//ok init MC
		debug_MC_log("MC discovery process");

		//if is the firs time for the MC I need to discover the list of MDs
		if(!MDs_list_discoverd)
			MD_Discover(MDs, &size);


		md_discovery_t *tmp = MDs;

		if(size > 0)
		{
			MDs_list_discoverd = TRUE;

			//TODO add check loop
			sprintf(dbg_msg,"MC has discovered %d MDs", size);
			debug_MC_log(dbg_msg);

			request_t request;
			response_t response;

			while(( tmp ) && ( FALSE == flag_MD_found ))
			{
				memset(dbg_msg, 0 , 80);
				sprintf(dbg_msg,"Try to connect to MD server with ip %s", tmp->MD_ip_address);
				debug_MC_log(dbg_msg);

				status = Client_Connect(tmp); //Connect to MD server

				if(FALSE == status)
				{
					error_MC_log("Impossible to connect to MD server");
				}
				else
				{
					memset(dbg_msg,0,80);
					sprintf(dbg_msg,"Connection Done to MD using interface %s and ip %s!!",
							tmp->MD_interface, tmp->MD_ip_address);
					debug_MC_log(dbg_msg);

					//set the status variable
					MC_task_status = MC_UP ;

					//ok here I need to ask the MD status
					debug_MC_log("MC asking the status from the MD server");

					//prepare the request (Prepare a MD_Status Message)
					status = prepare_MD_request(&request,MD_STATUS_OP, NULL);

					if(status == TRUE)
					{
						status = exchange(&request,&response);
						if(TRUE == status)
						{
							debug_MC_log("MD response correctly ... check response parameters");

							if(response.type_response < 0)
							{
								debug_MC_log("MD ko response");

								memset(dbg_msg,0,80);
								sprintf(dbg_msg,"STATUS DEL MD : %d\n",response.MD_status);
								debug_MC_log(dbg_msg);
								status = FALSE;
							}
							else
							{
								//MD ok response
								debug_MC_log("MD ok response");

								memset(dbg_msg,0,80);
								sprintf(dbg_msg,"MD has response and its status is %d",response.MD_status);
								debug_MC_log(dbg_msg);

								if(response.MD_status == MD_READY)
								{
									debug_MC_log("MD READY");
									status = TRUE;
								}
								else
								{
									debug_MC_log("MD BUSY");
									status = FALSE;
								}
							}


							flag_MD_found = TRUE;
						}
						else
						{
							debug_MC_log("MD/MC error exchange");
						}
					}
					else
					{
						debug_MC_log("Unable to prepare a request to send to MD server");
					}
				}
				tmp = tmp->next;
			}
		}
		else
		{
			debug_MC_log("Unable to find a list of MDs");
		}
	}
	else
	{
		error_MC_log("MC is Down.. Unable to find a MD server");
		status = FALSE;
	}

	//if status is false or a error is occurred or no MD has been found free

	if( FALSE == flag_MD_found )
		debug_MC_log("NO MD has been found free");

	memset(dbg_msg, 0, 80);
	sprintf(dbg_msg,"MC_Init ends with status %d", status);
	debug_MC_log(dbg_msg);

	return status;
}

bool request(request_t * req, response_t * res)
{
	bool status = FALSE;

	return status;
}

/**
 * Close MC client
 * */
bool MC_Down()
{
	bool status = FALSE;

	debug_MC_log("Closing MC");

	if( MC_UP == MC_task_status )
	{
		request_t request;
		request.type_op = MD_CLOSE_SIM_OP;
		//Write on socket to close
		MC_write(&request);

		close(MC_sock);
		MC_task_status = MC_DOWN;
		status = TRUE;
		free(MDs);
	}

	return status;
}

bool Client_Connect(md_discovery_t * md_replica)
{
	bool status = FALSE;
	struct sockaddr_in serv_add ;
	char msg_dbg[80];


	if( MC_DOWN == MC_task_status)
	{
		if ( ( MC_sock = socket ( AF_INET , SOCK_STREAM , 0)) < 0)
		{
			critical_MC_log("Impossible to create socket for MC");
		}
		memset (( void *) & serv_add , 0 , sizeof ( serv_add ));
		serv_add . sin_family = AF_INET ;
		serv_add . sin_port = htons (PORT);



		if ( ( inet_pton ( AF_INET , md_replica->MD_ip_address , & serv_add . sin_addr )) <= 0) {
			critical_MC_log("Impossible to format ip address for determinate the MD server");
		}
		/* extablish connection */

		if ( connect ( MC_sock , ( struct sockaddr *)& serv_add , sizeof ( serv_add )) < 0) {
			sprintf(msg_dbg,"%s : %s","Impossible to connect to this MD listening on ip address", md_replica->MD_ip_address);
			critical_MC_log(msg_dbg);
			memset(msg_dbg,0,80);
		}

		sprintf(msg_dbg,"MC connected to MD replica with ip %s",md_replica->MD_ip_address);
		debug_MC_log(msg_dbg);
		status = TRUE;
	}

	return status;
}


bool exchange(request_t * request, response_t *response )
{
	bool status = FALSE;
	string dbg_str_[80];

	debug_MC_log("MC request operation");

	switch(request->type_op)
	{
	case MD_STATUS_OP:
	{
		debug_MC_log("MC request MD status");
		status = MC_write(request); //start request from MC to MD to retrieve MD status
		if(TRUE == status)
		{
			//ok ..  request sent correctly

			debug_MC_log("MC waiting response");
			status = MC_read(response);
			if(FALSE == status)
			{
				error_MC_log("MC fail to read");
			}
			else
			{

				//MD Response Parameters
				memset(dbg_str_,0,80);
				sprintf(dbg_str_,"TYPE RESPONSE --> %d", response->type_response);
				debug_MC_log(dbg_str_);
				memset(dbg_str_,0,80);
				sprintf(dbg_str_,"MD STATUS --> %d", response->MD_status);
				debug_MC_log(dbg_str_);

				//response arrived
				if(response->type_response < 0 ) //error
				{
					error_MC_log("MD response with a error to get STATUS MD");
					status = FALSE;
				}
				else
				{
					info_MC_log("MD response is OK for STATUS MD");
					status = TRUE;
				}
			}
		}
		break;
	}

	case MD_SIMULATION_OP:
	{
		debug_MC_log("MC request a simulation");

		status = MC_write(request); //start request from MC to MD to execute a simulation
		if(TRUE == status)
		{
			//ok .. request sent correctly

			debug_MC_log("MC waiting response");
			status = MC_read(response);
			if(FALSE == status)
			{
				error_MC_log("MC fail to read");
			}
			else
			{
				//response arrived
				if(response->type_response < 0 ) //error
				{
					error_MC_log("MD response with a error to get SIMULATION MD");
				}
				else
				{
					info_MC_log("MD response is OK to get SIMULATION MD");
				}
			}
		}
		else
		{
			debug_MC_log("MC error to request a MD simulation");
		}
		break;
	}
	default:
		error_MC_log("Operation does not exist");
		break;
	}

	return status;
}



#endif
