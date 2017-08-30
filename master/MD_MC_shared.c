/*
 * MC_sock.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */


#include "MD_MC_shared.h"

//global variable where store path of discovery file
string discovery_file_path = NULL;

bool set_path_discovery_file(string path)
{
	bool status = TRUE;

	unsigned int len = strlen(path);

	if( ( len > 0 ) && ( discovery_file_path == NULL ))
	{
		discovery_file_path = (string)malloc(len);
	}

	memcpy(discovery_file_path, path, len);

	printf("Using path %s \n", discovery_file_path);

	return status;
}


string get_path_discovery_file(void)
{
	string ret_str;

	if(discovery_file_path != NULL)
	{
		if(strlen(discovery_file_path) > 0)
			ret_str = discovery_file_path;
		else
			ret_str = "";
	}
	else
		ret_str = "";

	return ret_str;
}


//------------------------------------------------------------------------------------------------------

/*
 * Routines to build a request or a response.
 *
 * This routines are needed because a array, which is a pointer to a memory location, have a significant only in
 * the machine which it is used. To pass a array which is defined inside a structure I have two possibilities.
 *
 * first -- Pass first the structure and its type base fields and later pass with another write on socket the arrays
 * second -- Compose a byte array where put inside all the data and make a operation of serialization and deserialization
 *
 * This second approach is from write on socket point of view better then first approach.. but in this release the library
 * will use the first approach and all the arrays will be sent only when there is not a error in the computation
 * */


bool serialize_arrays(TYPE_MOLECULAR stream[], molecular_t * data)
{
	bool status = TRUE;

	unsigned long l_index = 0 ;

	unsigned int size = data->size_mapnl +
			data->size_gh +
			( data->size_p0 *3 ) +
			( data->size_pg *3 ) +
			( data->size_pcm *3 );

	//reset array
	memset(stream,0,size);

	if(data->size_mapnl > 0)
	{
		//mapnl must be sent

		unsigned int i = 0;

		for(i = 0 ; i < data->size_mapnl ; i++, l_index++)
			stream[l_index] = (double)data->mapnl_d[i];

	}

	if(data->size_gh > 0)
	{
		//gh must be sent

		unsigned int i = 0;
		for(i = 0 ; i < data->size_gh ; i++, l_index++)
		{
			stream[l_index] = data->gh_d[i];
		}
	}

	if(data->size_p0 > 0)
	{
		//p0 must be sent (xp0,yp0,zp0)

		//Copy
		unsigned int k = 0;
		for(k = 0 ; k < data->size_p0 ; k++, l_index++)
			stream[l_index] = data->xp0_d[k];

		unsigned int j = 0;
		for(j = 0 ; j < data->size_p0 ; j++, l_index++)
			stream[l_index] = data->yp0_d[j];

		unsigned int i = 0;
		for(i = 0 ; i < data->size_p0 ; i++, l_index++)
			stream[l_index] = data->zp0_d[i];

	}

	if(data->size_pg > 0)
	{
		//pg must be sent (xpg,ypg,zpg)

		//Copy
		unsigned int k = 0;
		for(k = 0 ; k < data->size_pg ; k++, l_index++)
			stream[l_index] = data->xpg_d[k];

		unsigned int j = 0;
		for(j = 0 ; j < data->size_pg ; j++, l_index++)
			stream[l_index] = data->ypg_d[j];

		unsigned int i = 0;
		for(i = 0 ; i < data->size_pg ; i++ , l_index++)
			stream[l_index] = data->zpg_d[i];

	}

	if(data->size_pcm > 0)
	{
		//pcm must be sent (xpcm, ypcm, zpcm)

		//Copy
		unsigned int k = 0;
		for(k = 0 ; k < data->size_pcm ; k++, l_index++ )
			stream[l_index] = data->xpcm_d[k];

		unsigned int j = 0;
		for(j = 0 ; j < data->size_pcm ; j++, l_index++)
			stream[l_index] = data->ypcm_d[j];

		unsigned int i = 0;
		for(i = 0 ; i < data->size_pcm ; i++, l_index++)
			stream[l_index] = data->zpcm_d[i];

	}

	return status;
}

bool deserialize_arrays(TYPE_MOLECULAR stream[], molecular_t * data)
{
	bool status = TRUE;

	unsigned long l_index = 0;

	if(data->size_mapnl > 0)
	{
		//mapnl must be received

		//Alloc
		data->mapnl_d = (int*)malloc( sizeof(int) * data->size_mapnl );

		//Init
		memset(data->mapnl_d, 0 , sizeof(int) * data->size_mapnl );

		//Copy data
		unsigned int i = 0;
		for(i = 0 ; i < data->size_mapnl ; i++, l_index++)
		{
			data->mapnl_d[i] = (int)stream[l_index];
		}
	}

	if(data->size_gh > 0)
	{
		//gh must be received

		//Alloc
		data->gh_d = (TYPE_MOLECULAR*)malloc(  data->size_gh * sizeof(TYPE_MOLECULAR) );

		//Init
		memset(data->gh_d, 0 , data->size_gh );

		//Copy Data
		unsigned int i = 0;
		for( i = 0 ; i < data->size_gh; i++ , l_index++)
		{
			data->gh_d[i] = stream[l_index];
		}
	}

	if(data->size_p0 > 0)
	{

		//Alloc
		data->xp0_d = (TYPE_MOLECULAR*)malloc( data->size_p0 * sizeof(TYPE_MOLECULAR) );
		data->yp0_d = (TYPE_MOLECULAR*)malloc( data->size_p0 * sizeof(TYPE_MOLECULAR) );
		data->zp0_d = (TYPE_MOLECULAR*)malloc( data->size_p0 * sizeof(TYPE_MOLECULAR) );

		//Init
		memset(data->xp0_d, 0 , data->size_p0 );
		memset(data->yp0_d, 0 , data->size_p0 );
		memset(data->zp0_d, 0 , data->size_p0 );

		//Copy
		unsigned int k = 0;
		for(k = 0 ; k < data->size_p0 ; k++ , l_index++)
			data->xp0_d[k] = stream[l_index];

		unsigned int j = 0;
		for(j = 0 ; j < data->size_p0 ; j++ , l_index++)
			data->yp0_d[j] = stream[l_index];

		unsigned int i = 0;
		for(i = 0 ; i < data->size_p0 ; i++ , l_index++)
			data->zp0_d[i] = stream[l_index];

	}

	if(data->size_pg > 0)
	{
		//pg must be received (xpg,ypg,zpg)

		//Alloc
		data->xpg_d = (TYPE_MOLECULAR*)malloc( data->size_pg * sizeof(TYPE_MOLECULAR) );
		data->ypg_d = (TYPE_MOLECULAR*)malloc( data->size_pg * sizeof(TYPE_MOLECULAR) );
		data->zpg_d = (TYPE_MOLECULAR*)malloc( data->size_pg * sizeof(TYPE_MOLECULAR) );

		//Init
		memset(data->xpg_d, 0 , data->size_pg );
		memset(data->ypg_d, 0 , data->size_pg );
		memset(data->zpg_d, 0 , data->size_pg );

		//Copy
		unsigned int k = 0;
		for(k = 0 ; k < data->size_pg ; k++, l_index++)
			data->xpg_d[k] = stream[l_index];

		unsigned int j = 0;
		for(j = 0 ; j < data->size_pg ; j++ , l_index++)
			data->ypg_d[j] = stream[l_index];

		unsigned int i = 0;
		for(i = 0 ; i < data->size_pg ; i++ , l_index++)
			data->zpg_d[i] = stream[l_index];

	}

	if(data->size_pcm > 0)
	{
		//pcm must be received (xpcm, ypcm, zpcm)

		//Alloc
		data->xpcm_d = (TYPE_MOLECULAR*)malloc( data->size_pcm * sizeof(TYPE_MOLECULAR) );
		data->ypcm_d = (TYPE_MOLECULAR*)malloc( data->size_pcm * sizeof(TYPE_MOLECULAR) );
		data->zpcm_d = (TYPE_MOLECULAR*)malloc( data->size_pcm * sizeof(TYPE_MOLECULAR) );

		//Init
		memset(data->xpcm_d, 0 , data->size_pcm );
		memset(data->ypcm_d, 0 , data->size_pcm );
		memset(data->zpcm_d, 0 , data->size_pcm );

		//Copy
		unsigned int k = 0;
		for( k = 0 ; k < data->size_pcm ; k++, l_index++)
			data->xpcm_d[k] = stream[l_index];

		unsigned int j = 0;
		for(j = 0 ; j < data->size_pcm ; j++ , l_index++)
			data->ypcm_d[j] = stream[l_index];

		unsigned int i = 0;
		for(i = 0 ; i < data->size_pcm ; i++ , l_index++)
			data->zpcm_d[i] = stream[l_index];

	}


	return status;
}


bool write_on_Sock(int socket, TYPE_MOLECULAR * l_stream, unsigned int size)
{
	bool status = TRUE;

	//---------------------------------------------------------------
	//Write Data --> Start

	TYPE_MOLECULAR buff[MAX_MTU];
	unsigned int items = 0;
	unsigned int index = 0;
	unsigned int i = 0;
	unsigned int n_items_write = 0;
	ssize_t nbyte = 0;

	while( items < size)
	{
		memset(buff,0,sizeof(TYPE_MOLECULAR) * MAX_MTU);
		n_items_write = 0;

		for( i = 0 ; i < MAX_MTU ; i++, index++)
		{
			if(index < size)
			{
				buff[i] = l_stream[index];
				n_items_write++;
			}
		}

		nbyte = send(socket, (void*) buff, sizeof(TYPE_MOLECULAR) * n_items_write, 0);
		if(nbyte == ERROR)
		{
			error_MD_log("MD unable to send to socket molecular arrays");
		}

		items += n_items_write;
	}

	//---------------------------------------------------------------
	//Write Data <-- End

	return status;
}

bool read_from_Sock(int socket ,TYPE_MOLECULAR * l_stream, unsigned int size)
{
	bool status = TRUE;

	unsigned int items = 0 ;
	unsigned int i = 0 ;
	unsigned int index = 0;
	unsigned int max_num_read = 0;
	ssize_t nbyte = 0;

	TYPE_MOLECULAR buff[MAX_MTU];

	while( items < size  )
	{
		max_num_read = 0;
		memset(buff,0,sizeof(TYPE_MOLECULAR) * MAX_MTU);
		nbyte = recv(socket,
				(void *) buff,
				(MAX_MTU * sizeof(TYPE_MOLECULAR)),0);

		if(nbyte == ERROR)
		{
			error_MD_log("MD unable to read from socket molecular arrays");
			status = FALSE;
			return status;
		}
		else
		{
			//Copy Values
			max_num_read = nbyte/sizeof(TYPE_MOLECULAR); //calculate num of items read from socket
			for( i = 0 ; i < max_num_read ; i++ , index++ )
				l_stream[index] = buff[i];
		}

		items += max_num_read;
	}

	//Read Data <-- End
	//-------------------------------------------------------

	return status;
}
