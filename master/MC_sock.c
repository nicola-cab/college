/*
 * MC_sock.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifdef MC_REPLICA__


#include "MC_sock.h"
#include "MD_MC_shared.h"

#include <sys/types.h>
#include <sys/socket.h>

int MC_sock;

bool MC_read(response_t * response)
{
	bool status = FALSE;
	ssize_t nbyte = 0;
	char dbg_msg[80];
	memset(dbg_msg,0,80);

	debug_MC_log("MC reading from socket a response from MD");

	nbyte = recv( MC_sock, (void *) response, sizeof(response_t),0);
	if(nbyte == ERROR)
	{
		error_MC_log("MC unable to read from socket the MD response!!!");
	}
	else
	{
		debug_MC_log("MC read from socket done correctly ");
		sprintf(dbg_msg,"MD's response is %d", response->type_response);
		debug_MC_log(dbg_msg);

		if( (response->type_response == MD_SIMULATION_RESPONSE)
				&& (response->MD_status != MD_ERROR))
		{

			unsigned int size = response->data.size_mapnl +
					response->data.size_gh +
					(response->data.size_p0 * 3) +
					(response->data.size_pg * 3) +
					(response->data.size_pcm *3);

			TYPE_MOLECULAR * l_stream = (TYPE_MOLECULAR *)malloc(sizeof(TYPE_MOLECULAR) * size);  //array to send
			memset(l_stream,0,size * sizeof(TYPE_MOLECULAR));


			status = read_from_Sock(MC_sock, l_stream, size);
			if(status == FALSE)
			{
				debug_MC_log("HUGE ---- ERROR");
				debug_MC_log("-----------------------------------------------------------------------");
				critical_MC_log("Fail to read from socket molecular dynamic arrays");
			}


			debug_MC_log("Arrives molecular dynamic arrays");
			debug_MC_log("MC deserialize arrays");

#if 0
			printf("Testing stream received by MC sent by MD as a response \n");
			unsigned int i = 0;
			for (i = 0 ; i < size ; i++)
			{
				printf("stream[%d] --> %9f 	\n", i, l_stream[i]);
			}
#endif

			status = deserialize_arrays(l_stream,&response->data);

#if 0

			//test...

			printf("Arrays passed by MD to MC as a response for molecular dynamic simulation (orac)\n");

			unsigned int j = 0;
			for( j = 0 ; j < response->data.size_mapnl ; j++)
			{
				printf("mapnl_d[%d] --> %9f \n",j, (double)response->data.mapnl_d[j]);
			}

			for( j = 0 ; j < response->data.size_gh ; j++)
			{
				printf("gh_d[%d] --> %9f \n",j, response->data.gh_d[j]);
			}

			for( j = 0 ; j < response->data.size_p0 ; j++)
			{
				printf("xp0_d[%d] --> %9f \n",j, response->data.xp0_d[j]);
				printf("yp0_d[%d] --> %9f \n",j, response->data.yp0_d[j]);
				printf("zp0_d[%d] --> %9f \n",j, response->data.zp0_d[j]);
			}

			for( j = 0 ; j < response->data.size_pg ; j++)
			{
				printf("xpg_d[%d] --> %9f \n",j, response->data.xpg_d[j]);
				printf("ypg_d[%d] --> %9f \n",j, response->data.ypg_d[j]);
				printf("zpg_d[%d] --> %9f \n",j, response->data.zpg_d[j]);
			}

			for( j = 0 ; j < response->data.size_pcm ; j++)
			{
				printf("xpcm_d[%d] --> %9f \n",j, response->data.xpcm_d[j]);
				printf("ypcm_d[%d] --> %9f \n",j, response->data.ypcm_d[j]);
				printf("zpcm_d[%d] --> %9f \n",j, response->data.zpcm_d[j]);
			}
#endif
			free(l_stream);
		}
		else
		{
			status = TRUE;
		}
	}

	return status;
}

bool MC_write(request_t * request)
{
	bool status = FALSE;
	ssize_t nbyte = 0;

	debug_MC_log("MC writing on socket a request");

	if(request->type_op == MD_STATUS_OP)
	{
		debug_MC_log("MC send a request msg to ask the status of MD");
	}
	else
	{
		if(request->type_op == MD_SIMULATION_OP)
		{
			debug_MC_log("MC send a request msg to ask a simulation from MD ");
		}
		else if(request->type_op == MD_CLOSE_SIM_OP)
		{
			debug_MC_log("MC close connection with MD");
		}
		else
		{
			debug_MC_log("Bad msg parameter");
			return ERROR;
		}
	}

	nbyte = send(MC_sock, (void *) request, sizeof(request_t),0);
	if(nbyte == ERROR)
	{
		error_MC_log("MC Unable to write MD request to socket");
	}
	else
	{

		if( request->type_op == MD_SIMULATION_OP )
		{
			unsigned int size = request->data.size_mapnl +
					request->data.size_gh +
					( request->data.size_p0 * 3 ) +
					( request->data.size_pg * 3 ) +
					(request->data.size_pcm * 3 );

			TYPE_MOLECULAR * l_stream = (TYPE_MOLECULAR *)malloc(sizeof(TYPE_MOLECULAR) * size);  //array to send
			memset(l_stream, 0, sizeof(TYPE_MOLECULAR) * size);

#if 0

			printf(" :::: MC_write :::: Data to pass to MD \n");
			unsigned int i = 0;

			if(request->data.size_mapnl > 0)
			{
				for(i = 0 ; i < request->data.size_mapnl ; i++)
				{
					printf("mapnl_d[%d]  --> %d \n", i,request->data.mapnl_d[i]);
				}

			}

			if(request->data.size_gh > 0)
			{
				for(i = 0 ; i < request->data.size_gh ; i++)
				{
					printf("gh_d[%d]  --> %9f \n", i,request->data.gh_d[i]);
				}
			}

			if(request->data.size_p0 > 0)
			{

				for(i = 0 ; i < request->data.size_p0 ; i++)
				{
					printf("xp0_d[%d]  --> %9f \n", i,request->data.xp0_d[i]);
					printf("yp0_d[%d]  --> %9f \n", i,request->data.yp0_d[i]);
					printf("zp0_d[%d]  --> %9f \n", i,request->data.zp0_d[i]);
				}

			}

			if(request->data.size_pg > 0)
			{

				for(i = 0 ; i < request->data.size_pg ; i++)
				{
					printf("xpg_d[%d]  --> %9f \n", i,request->data.xpg_d[i]);
					printf("ypg_d[%d]  --> %9f \n", i,request->data.ypg_d[i]);
					printf("zpg_d[%d]  --> %9f \n", i,request->data.zpg_d[i]);
				}

			}

			if(request->data.size_pcm > 0)
			{

				for(i = 0 ; i < request->data.size_pcm ; i++)
				{
					printf("xpcm_d[%d]  --> %9f \n", i,request->data.xpcm_d[i]);
					printf("ypcm_d[%d]  --> %9f \n", i,request->data.ypcm_d[i]);
					printf("zpcm_d[%d]  --> %9f \n", i,request->data.zpcm_d[i]);
				}

			}

			printf(" :::: MC_write :::: Data to pass \n");

#endif

			debug_MC_log("MC serialize arrays");
			status = serialize_arrays(l_stream, &request->data);

#if 0

			printf(" :::: MC_write :::: Data serialized  \n");
			unsigned int j = 0;


			for( j = 0 ; j < size ; j++)
				printf("stream[%d] ---> %9f\n", j, l_stream[j]);

#endif

			if(status == TRUE)
			{
				status = write_on_Sock(MC_sock, l_stream, size);
				if(status == FALSE)
				{
					debug_MC_log("HUGE ---- ERROR");
					debug_MC_log("-----------------------------------------------------------------------");
					critical_MC_log("Fail to write to socket molecular dynamic arrays");
				}
			}
			else
			{
				error_MD_log("MC Error to serialize arrays");
			}

			free(l_stream);

		}
		else if(request->type_op == MD_STATUS_OP)
		{
			debug_MC_log("MC requested MD_STATUS OP");
			status = TRUE;
		}
		else
		{
			debug_MC_log("MC requested MD_CLOSE_SIM_OP");
		}
		debug_MC_log("MC Writing on socket done correctly");
	}

	return status;
}

#endif
