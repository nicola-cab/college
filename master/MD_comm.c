/*
 * MD_comm.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifdef MD_REPLICA__

#include "MD_comm.h"


extern int MD_sock;
extern char g_network_interface[50];

//Single MD Unit (All info about single MD)
md_discovery_t * MD_Unit = NULL;

//Global variable to keep MD server status
STATUS MD_task_status = MD_DOWN;

//prev status for MD
STATUS  prev_MD_task_status = MD_DOWN;

//Thread synchronization
pthread_mutex_t MD_status_mutex = PTHREAD_MUTEX_INITIALIZER;

//semaphore to manage all tasks in queue for MD
sem_t MD_sem;

//semaphore to manage effective run simulation
sem_t MD_sem_simulation_start;

void set_MD_Network_interface(char * network_interface)
{
	debug_MD_log("Use network interface");
	debug_MD_log(network_interface);

	strcpy(g_network_interface,network_interface);
}

bool MD_Init()
{
	bool status = FALSE;

	if(MD_DOWN == MD_task_status)
	{
		debug_MD_log("MD Server initialization!!");

		status = Server_Startup();
		if( FALSE == status )
		{
			critical_MD_log("Impossible to initialize the MD server");
		}
		else
		{
			debug_MD_log(" MD Server running up process ...");
			Server_Run();
		}
	}

	return status;
}

/**
 * Close MD server replica
 * */
bool MD_Down()
{
	bool status = FALSE;

	debug_MD_log("Closing MD");

	if( MD_READY == MD_task_status  ) //Switch off MD only if is in MD_READY state
	{
		//free(MD_Unit); //free MD pointer
		pthread_mutex_destroy(&MD_status_mutex);
		sem_destroy(&MD_sem);
		sem_destroy(&MD_sem_simulation_start);
		MD_task_status = MD_DOWN;
		status = TRUE;
	}

	return status;
}


bool Server_Startup()
{
	bool status = FALSE;
	struct sockaddr_in serv_addr;
	md_discovery_t md_entry ;


	if( MD_DOWN == MD_task_status )
	{
		Log(MD_LOGGER, E_INFO, "MD server startup process starts  ...");

	    MD_sock = socket(AF_INET, SOCK_STREAM, 0);

	    if(MD_sock < 0)
	    {
	    	critical_MD_log("Unable to open socket !!");
	    }

	    bzero((char *) &serv_addr, sizeof(serv_addr));
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	    serv_addr.sin_port = htons(PORT);

	    if (bind(MD_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	    {
	    	critical_MD_log("Unable to do binding !!!");
	    }

	    listen(MD_sock,MAX_CONN);


	    //preparing all information about MD
	    status = Discover_MD_IP(&md_entry);

	    if(FALSE == status)
	    {
	    	close(MD_sock);
	    	critical_MD_log("Unable to add a new entry to the discovery file !!!");
	    }

	    //save current entry
	    if(MD_Unit == NULL)
	    {
	    	MD_Unit = (md_discovery_t *)malloc(sizeof(md_discovery_t));
	    }

	    MD_Unit->MD_interface = (string)malloc(strlen(md_entry.MD_interface));
	    MD_Unit->MD_ip_address = (string)malloc(strlen(md_entry.MD_ip_address));

	    strcpy(MD_Unit->MD_interface , md_entry.MD_interface);
	    strcpy(MD_Unit->MD_ip_address , md_entry.MD_ip_address);
	    MD_Unit->MD_status = md_entry.MD_status;

	    free(md_entry.MD_interface);
	    free(md_entry.MD_ip_address);

	    //set the status of MD Server
	    MD_task_status = MD_UP;
	    //MD_Unit->MD_status = MD_task_status; //MD Ready and Up

		info_MD_log("MD server startup process ends  ...");

	}

	return status;
}

/**
 * Thread Manager routine
 */

//FSM

void* MD_Manager_Request(void *data_thread)
{
	debug_MD_log("MD Logic Thread Manager for run a request from MC");

	md_info_thread_t * sd_info = (md_info_thread_t *) data_thread;

	response_t resp;
	request_t req;
	ssize_t size;

	char errorMsg[80];
	STATUS status_MD;

	bool in_loop = 1;

	bool status = FALSE;

	char msg[80];
	memset(msg,0,80);
	sprintf(msg,"Server nello status %d", MD_task_status);
	debug_MD_log(msg);

	//Execution loop
	while(in_loop)
	{
		debug_MD_log("MD Server : Waiting a request from MC");
		//printf("Valore del socket passato %d\n",sd_info->sd);
		size = MD_read(sd_info->sd, &req);
		if(size < 0)
		{
			error_MD_log("MD error to read from socket");
		}
		else
		{
			//Start FSM for MD
			switch(req.type_op)
			{
			case MD_STATUS_OP:
			{
				//waiting on queue for MD_STATUS_OP
				sem_wait(&MD_sem);
				//This will available only when simulation starts
				sem_wait(&MD_sem_simulation_start);

				debug_MD_log("MD_STATUS_OP");

				status = atomic_get_MD_status(&status_MD);

				if(TRUE == status)
				{
					debug_MD_log("Prepare MD response to MD_STATUS_OP");

					memset(msg,0,80);
					sprintf(msg,"STATUS MD %d", status_MD);
					debug_MD_log(msg);

					molecular_t data;
					status = prepare_MC_response(
							&resp,
							MD_STATUS_RESPONSE,
							errorMsg,sizeof(errorMsg),
							status_MD,
							NULL);

					if(status == TRUE)
					{
						debug_MD_log("Send MD status to MC");
						status = MD_write(sd_info->sd, &resp);
						if(TRUE == status)
						{
							debug_MD_log("MD status sent!!!");
						}
						else
						{
							error_MD_log("MD impossible to send status using socket!!!");
						}
						//MD locking to prevent race conditions between MCs

						prev_MD_task_status = status_MD; //save the previous status
						atomic_set_MD_status(MD_BUSY);

					}
					else
					{
						error_MD_log("Unable to send MD status"); //bad error;
					}
				}
				else
				{
					error_MD_log("MD Unable to get its status");

					strcpy(errorMsg,"MD Unable to get its status"); //bad error
					molecular_t data;
					status = prepare_MC_response(
							&resp,
							MD_GENERIC_ERROR,
							errorMsg,sizeof(errorMsg),
							status_MD,
							NULL);

					if(TRUE == status)
					{
						debug_MD_log("MD send error to MC");
						status = MD_write(sd_info->sd, &resp);
						if( TRUE == status )
						{
							debug_MD_log("MD sent error correctly");
						}
						else
						{
							error_MD_log("Unable to send MD error");
						}
					}
				}

				//Free Queue for MD_STATUS_OP
				sem_post(&MD_sem);

			}
			break;

			case MD_SIMULATION_OP:
			{
				//waiting on queue for MD_SIMULATION_OP
				sem_wait(&MD_sem);

				debug_MD_log("MD_SIMULATION_OP");

				atomic_get_MD_status(&status_MD);

				//If actually I'm busy and first I was
				if((MD_BUSY == status_MD) && (MD_READY == prev_MD_task_status))
				{
					//simulation starts.. free queue
					sem_post(&MD_sem_simulation_start);

					debug_MD_log("MD algorithm starts");

					//Attach MD FORTRAN execution
					//ORAC INTERFACE
					int ret = 0;
					ret = orac_md_exec_(errorMsg, 80, &req.data);

					debug_MD_log("MD algorithm ends");

					//DEBUG
					if(ret < 0)
					{
						//MD Error execution
						debug_MD_log("Error executing molecular dynamic simulation ");
						debug_MD_log("Error msg from fortran ORAC : ");
						debug_MD_log(errorMsg);
					}
					else
					{
						//MD Success
						debug_MD_log("Molecular dynamic execution success !!!");
					}

					//RESPONSE IS ALWAYS COMPOSED
					debug_MD_log("Prepare response for simulation request");
					prepare_MC_response(&resp,
							MD_SIMULATION_RESPONSE,
							errorMsg,
							strlen(errorMsg),
							status_MD,
							&req.data);

					debug_MD_log("Writing on socket result after simulation");
					//WRITE ON SOCKET
					MD_write(sd_info->sd, &resp);

				}
				else
				{
					error_MD_log("MD FSM in bad status ... check the code");
				}

				//Free Queue for MD_OPERATION_OP
				sem_post(&MD_sem);
			}
			break;

			case MD_CLOSE_SIM_OP:
			{
				debug_MD_log("MD_CLOSE_SIM_OP");
				debug_MD_log("MD release all resources... after this operation a new simulation will be available");

				//waiting a queue for release resources of MD after MD_SIMULATION_OP
				sem_wait(&MD_sem);

				//Exit .. MC request to release all resources and the thread dedicated
				//Set status variable
				debug_MD_log("Free MD Status");
				atomic_set_MD_status(MD_READY);

				//thread exit with success
				close(sd_info->sd);

				//End loop
				in_loop = 0;

				//Free queue for MD_CLOSE_OP
				sem_post(&MD_sem);

			}
			break;

			default:
				debug_MD_log("MD FSM : No request recognized with this option");
				break;
			}
		}

	} //execution loop

	return NULL;

} //Thread routine


void Server_Run()
{

	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	bool status;

	//thread to manage requests
	pthread_t MD_Thread;
	md_info_thread_t info;

	clilen = sizeof(cli_addr);

	if( MD_UP == MD_task_status )
	{
		//Save on file MD_Entry
		status = save_on_file(MD_Unit);

		if(status == FALSE)
		{
			//error
			critical_MD_log("Impossible to continue in the server startup process, cause unable to save MD discovery information");
		}

		MD_task_status = MD_READY;  //MD Ready

		//init sem
		sem_init(&MD_sem, 0, MAX_QUEUE);
		sem_init(&MD_sem_simulation_start, 0, MAX_QUEUE);

		debug_MD_log("MD Server running....");
		while(TRUE)
		{
			newsockfd = accept(MD_sock,(struct sockaddr *) &cli_addr,&clilen);

			string msg = (string)malloc(sizeof(char)*80);
			sprintf(msg,"%s %s",
					"MD server: arrived request from MC client with ip : ",
					inet_ntoa(cli_addr.sin_addr));
			debug_MD_log(msg);


			if (newsockfd < 0)
			{
				error_MD_log("Fail to do accept!!!");
			}
			else
			{

				info.sd = newsockfd;

				memset(msg,0,sizeof(msg));
				sprintf(msg,"MD manage MC request using socket descriptor %d", newsockfd);
				debug_MD_log(msg);

				pthread_create(&MD_Thread, NULL, MD_Manager_Request, (void *) &info);

			}
		}
	}
}

bool atomic_set_MD_status(STATUS task_status)
{
	bool status = TRUE;

	pthread_mutex_lock(&MD_status_mutex);
	MD_task_status = task_status;
	pthread_mutex_unlock(&MD_status_mutex);

	return status;
}

bool atomic_get_MD_status(STATUS *current_status)
{
	bool status = TRUE;

	pthread_mutex_lock(&MD_status_mutex);
	*current_status = MD_task_status;
	pthread_mutex_unlock(&MD_status_mutex);

	return status;
}

#endif
