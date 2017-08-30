/*
 * MD_sock.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifdef MD_REPLICA__

#include "MD_sock.h"

//MD socket file descriptor
int MD_sock;

bool MD_read(int sd,request_t * request)
{
	bool status = FALSE;
	ssize_t nbyte = 0;

	debug_MD_log("MD : Low level read from socket");
	debug_MD_log("MD read on socket a MC request");

	nbyte = recv(sd, (void *) request, sizeof(request_t), 0);

	if(nbyte == ERROR)
	{
		//error
		error_MD_log("MD Unable to read from socket a MC request");
	}
	else
	{
		debug_MD_log("MD Reading from socket done correctly base request_t struct");

		if( request->type_op == MD_SIMULATION_OP )
		{
			debug_MD_log("MD SIMULATION OP requested ... low level API socket");

			unsigned int size =
					request->data.size_mapnl +
					request->data.size_gh +
					(request->data.size_p0 * 3) +
					(request->data.size_pg * 3) +
					(request->data.size_pcm * 3);  //global size of array to receive

			TYPE_MOLECULAR * l_stream = (TYPE_MOLECULAR *)malloc(sizeof(TYPE_MOLECULAR)* size);
			memset(l_stream, 0, size * sizeof(TYPE_MOLECULAR));

			bool status = read_from_Sock(sd, l_stream, size);
			if(status == FALSE)
			{
				debug_MD_log("HUGE ---- ERROR");
				debug_MD_log("-----------------------------------------------------------------------");
				critical_MD_log("Fail to read from socket molecular dynamic arrays");
			}

			debug_MD_log("MD deserialize arrays");

#if 0 //DEBUG_STDOUT_PRINT
			printf("Testing stream received by MD sent by MC \n");
			i = 0;
			for (i = 0 ; i < size ; i++)
			{
				printf("stream[%d] --> %9f 	\n", i, l_stream[i]);
			}
#endif

			status = deserialize_arrays(l_stream, &request->data);

#if 0

			//test...

			printf("Arrays passed by MC to pass to molecular dynamic routine (orac) \n");

			unsigned int j = 0;
			for( j = 0 ; j < request->data.size_mapnl ; j++)
			{
				printf("mapnl_d[%d] --> %9f \n",j, (double)request->data.mapnl_d[j]);
			}

			for( j = 0 ; j < request->data.size_gh ; j++)
			{
				printf("gh_d[%d] --> %9f \n",j, request->data.gh_d[j]);
			}

			for( j = 0 ; j < request->data.size_p0 ; j++)
			{
				printf("xp0_d[%d] --> %9f \n",j, request->data.xp0_d[j]);
				printf("yp0_d[%d] --> %9f \n",j, request->data.yp0_d[j]);
				printf("zp0_d[%d] --> %9f \n",j, request->data.zp0_d[j]);
			}

			for( j = 0 ; j < request->data.size_pg ; j++)
			{
				printf("xpg_d[%d] --> %9f \n",j, request->data.xpg_d[j]);
				printf("ypg_d[%d] --> %9f \n",j, request->data.ypg_d[j]);
				printf("zpg_d[%d] --> %9f \n",j, request->data.zpg_d[j]);
			}

			for( j = 0 ; j < request->data.size_pcm ; j++)
			{
				printf("xpcm_d[%d] --> %9f \n",j, request->data.xpcm_d[j]);
				printf("ypcm_d[%d] --> %9f \n",j, request->data.ypcm_d[j]);
				printf("zpcm_d[%d] --> %9f \n",j, request->data.zpcm_d[j]);
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

bool MD_write(int sd, response_t * response)
{
	bool status = FALSE;
	ssize_t nbyte = 0;

	debug_MD_log("MD write on socket a response to MC");

	nbyte = send(sd, (void *) response, sizeof(response_t),0);
	if(nbyte == ERROR)
	{
		error_MD_log("MD Unable to write MC response on socket");
	}
	else
	{
		debug_MD_log("MD Writing on socket done correctly base response_t struct");

		//NICOLA : write separately arrays
		if( (response->type_response == MD_SIMULATION_RESPONSE)
				&& (response->MD_status != MD_ERROR))
		{
			//Write arrays

			unsigned size = response->data.size_mapnl +
					response->data.size_gh +
					(response->data.size_p0 *3) +
					(response->data.size_pg *3) +
					(response->data.size_pcm *3);

			debug_MD_log("MD Serialize arrays");

			TYPE_MOLECULAR * l_stream = (TYPE_MOLECULAR *)malloc(sizeof(TYPE_MOLECULAR)* size);
			memset(l_stream, 0, size * sizeof(TYPE_MOLECULAR));

			status = serialize_arrays(l_stream,&response->data);

			if(status == TRUE)
			{
				status = write_on_Sock(sd, l_stream, size);
				if(status == FALSE)
				{
					debug_MD_log("HUGE ---- ERROR");
					debug_MD_log("-----------------------------------------------------------------------");
					critical_MD_log("Fail to write from socket molecular dynamic arrays");
				}
			}
			else
			{
				error_MD_log("MD Error to serialize arrays");
			}

			free(l_stream);
		}
		else
		{
			status = TRUE;
		}
	}

	return status;
}

#endif
