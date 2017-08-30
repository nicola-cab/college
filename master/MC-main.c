/*
 * main.c
 *
 *  Created on: Jan 26, 2012
 *      Author: nicola
 */

#include "MC.h"
#include <signal.h>


#define MAX_LEN_MAPNL 10 //MAPNL_SIZE  //Set the size of your
#define MAX_LEN_GH 10//GH_SIZE
#define MAX_LEN_P0 10//P0_SIZE
#define MAX_LEN_PG 10//PG_SIZE
#define MAX_LEN_PCM 10//PCM_SIZE


void prepare_data(molecular_t *);
void testing_data(molecular_t *);


void handler_sigsegv(void)
{
	MC_Down();
	exit(0);
}

void handler_abort(void)
{
	MC_Down();
	exit(0);
}

void pabort(string msg)
{
	printf("%s\n",msg);
	exit(-1);
}

void handler_sigint(void)
{
	//Close server
	bool status = FALSE;
	status = MC_Down();
	if(FALSE == status)
	{
		pabort("MC closed uncorrectly\n");
	}
	else
		printf("MC closed correctly\n");

	exit(0);
}

void handler_sigkill(void)
{
	bool status = FALSE;
	status = MC_Down();
	if(FALSE == status)
	{
		pabort("MC closed uncorrectly\n");
	}
	else
		printf("MC closed correctly\n");

	exit(0);
}

int main(int argc, char ** argv)
{

	if(argc < 3)
	{
		printf("Usage is ./<MC_exec_file> <mc_id> <path where is the file to discover MDs> \n");
		return -1;
	}

	bool status = FALSE;
	MD_response MD_response;
	molecular_t  * data;

	srand(time(NULL));

	signal(SIGSEGV, handler_sigsegv);
	signal(SIGINT, handler_sigint);
	signal(SIGKILL, handler_sigkill);
	signal(SIGABRT, handler_abort);


	data =  (molecular_t *)malloc(sizeof(molecular_t));
	prepare_data(data);

#if 0
	//to print data
	testing_data(data);
#endif

	//parameters needed
	unsigned short int mc_id = 0;
	mc_id = atoi(argv[1]);
	char * path_discover = argv[2];

	status = Wrapper_MC_Ask_MD_Simulation(path_discover,mc_id,data,&MD_response);

	if(FALSE == status)
	{
		printf("Error to communicate with MD\n");
	}
	else
	{
#if 0
		testing_data(&MD_response.response.data);
#endif

		printf("Molecular simulation executed with success !!! \n");
	}


	free(data);

	return 0;
}


void prepare_data(molecular_t * data)
{
	//----------------------------------------------------------------------------------------------------------------------//
	//Prepare data!!!

	data->size_mapnl = MAX_LEN_MAPNL;
	data->size_gh = MAX_LEN_GH;
	data->size_p0 = MAX_LEN_P0;  //trasmit only p0 arrays*/
	data->size_pcm = MAX_LEN_PCM;
	data->size_pg = MAX_LEN_PG;

	if(data->size_mapnl > 0)
	{
		data->mapnl_d = (int*)malloc(data->size_mapnl * sizeof(int));
		memset(data->mapnl_d,0,data->size_mapnl);
	}

	if(data->size_gh > 0)
	{
		data->gh_d = (TYPE_MOLECULAR*)malloc(data->size_gh * sizeof(TYPE_MOLECULAR));
		memset(data->gh_d,0,data->size_gh);
	}

	if(data->size_p0 > 0)
	{
		data->xp0_d = (TYPE_MOLECULAR*)malloc(data->size_p0 * sizeof(TYPE_MOLECULAR));
		data->yp0_d = (TYPE_MOLECULAR*)malloc(data->size_p0 * sizeof(TYPE_MOLECULAR));
		data->zp0_d = (TYPE_MOLECULAR*)malloc(data->size_p0 * sizeof(TYPE_MOLECULAR));

		memset(data->xp0_d,0,data->size_p0);
		memset(data->yp0_d,0,data->size_p0);
		memset(data->zp0_d,0,data->size_p0);

	}

	if(data->size_pg > 0)
	{
		data->xpg_d = (TYPE_MOLECULAR*)malloc(data->size_pg * sizeof(TYPE_MOLECULAR));
		data->ypg_d = (TYPE_MOLECULAR*)malloc(data->size_pg * sizeof(TYPE_MOLECULAR));
		data->zpg_d = (TYPE_MOLECULAR*)malloc(data->size_pg * sizeof(TYPE_MOLECULAR));

		memset(data->xpg_d,0,data->size_pg);
		memset(data->ypg_d,0,data->size_pg);
		memset(data->zpg_d,0,data->size_pg);
	}

	if(data->size_pcm > 0)
	{
		data->xpcm_d = (TYPE_MOLECULAR*)malloc(data->size_pcm * sizeof(TYPE_MOLECULAR));
		data->ypcm_d = (TYPE_MOLECULAR*)malloc(data->size_pcm * sizeof(TYPE_MOLECULAR));
		data->zpcm_d = (TYPE_MOLECULAR*)malloc(data->size_pcm * sizeof(TYPE_MOLECULAR));

		memset(data->xpcm_d,0,data->size_pcm);
		memset(data->ypcm_d,0,data->size_pcm);
		memset(data->zpcm_d,0,data->size_pcm);
	}

	//Set values
	unsigned int i = 0;

	//mapnl
	for( i = 0 ; i < data->size_mapnl ; i++)
		data->mapnl_d[i] = 10 + i;

	//gh
	for( i = 0 ; i < data->size_gh ; i++)
		data->gh_d[i] = (double)20 + i;

	//p0 (x,y,z)
	for( i = 0 ; i < data->size_p0 ; i++)
	{
		data->xp0_d[i] = (double)30 + 1 + i;
		data->yp0_d[i] = (double)30 + 2 + i;
		data->zp0_d[i] = (double)30 + 3 + i;
	}

	//pg (x,y,z)
	for( i = 0 ; i < data->size_pg ; i++)
	{
		data->xpg_d[i] = (double)40 + 1 + i;
		data->ypg_d[i] = (double)40 + 2 + i;
		data->zpg_d[i] = (double)40 + 3 + i;
	}

	//pcm (x,y,z)
	for( i = 0 ; i < data->size_pcm ; i++)
	{
		data->xpcm_d[i] = (double)50 + 1 + i;
		data->ypcm_d[i] = (double)50 + 2 + i;
		data->zpcm_d[i] = (double)50 + 3 + i;
	}

	//----------------------------------------------------------------------------------------------------------------------//

	return;
}

void testing_data(molecular_t * data)
{
	printf("size mapnl %d \n", data->size_mapnl);
	printf("size gh %d \n", data->size_gh);
	printf("size p0 %d \n", data->size_p0);
	printf("size pg %d \n", data->size_pg);
	printf("size pcm %d \n", data->size_pcm);

	printf("Data to send to MD\n");

	unsigned int i = 0;

#if 1

	//Debug ...

	if(data->size_mapnl > 0)
	{
		printf("Send mapnl_d\n");

		//Print values
		for (i = 0; i < data->size_mapnl; i++) {
			printf("data->mapnl_d[%d] = %d\n", i, data->mapnl_d[i]);
		}

		//Printf memory stack
		printf("\n&(data->size_mapnl) = %p\n", &(data->size_mapnl));
		for (i = 0; i < data->size_mapnl; i++) {
			printf("&(data->mapnl_d[%d]) = %p\n", i, &(data->mapnl_d[i]));
		}

		printf("\n\n");
	}

	if(data->size_gh > 0)
	{
		printf("Send gh_d\n");

		for (i = 0; i < data->size_gh; i++) {
			printf("data->gh_d[%d] = %9f\n", i, data->gh_d[i]);
		}
		printf("\n&(data->size_gh) = %p\n", &(data->size_gh));
		for (i = 0; i < data->size_gh; i++) {
			printf("&(data->gh_d[%d]) = %p\n", i, &(data->gh_d[i]));
		}

		printf("\n\n");
	}

	if(data->size_p0 > 0)
	{
		printf("Send p0\n");

		//XP0
		for (i = 0; i < data->size_p0; i++) {
			printf("data->xp0_d[%d] = %9f\n", i, data->xp0_d[i]);
		}
		printf("\n&(data->size_p0) = %p\n", &(data->size_p0));
		for (i = 0; i < data->size_p0; i++) {
			printf("&(data->xp0_d[%d]) = %p\n", i, &(data->xp0_d[i]));
		}

		printf("\n");

		//YP0
		for (i = 0; i < data->size_p0; i++) {
			printf("data->yp0_d[%d] = %9f\n", i, data->yp0_d[i]);
		}
		printf("\n&(data->size_p0) = %p\n", &(data->size_p0));
		for (i = 0; i < data->size_p0; i++) {
			printf("&(data->yp0_d[%d]) = %p\n", i, &(data->yp0_d[i]));
		}

		printf("\n");

		//ZP0
		for (i = 0; i < data->size_p0; i++) {
			printf("data->zp0_d[%d] = %9f\n", i, data->zp0_d[i]);
		}
		printf("\n&(data->size_p0) = %p\n", &(data->size_p0));
		for (i = 0; i < data->size_p0; i++) {
			printf("&(data->zp0_d[%d]) = %p\n", i, &(data->zp0_d[i]));
		}

		printf("\n\n");

	}

	if(data->size_pg > 0)
	{
		printf("Send pg\n");

		//XPG
		for (i = 0; i < data->size_pg; i++) {
			printf("data->xpg_d[%d] = %9f\n", i, data->xpg_d[i]);
		}
		printf("\n&(data->size_pg) = %p\n", &(data->size_pg));
		for (i = 0; i < data->size_pg; i++) {
			printf("&(data->xpg_d[%d]) = %p\n", i, &(data->xpg_d[i]));
		}

		printf("\n");

		//YPG
		for (i = 0; i < data->size_pg; i++) {
			printf("data->ypg_d[%d] = %9f\n", i, data->ypg_d[i]);
		}
		printf("\n&(data->size_pg) = %p\n", &(data->size_pg));
		for (i = 0; i < data->size_pg; i++) {
			printf("&(data->ypg_d[%d]) = %p\n", i, &(data->ypg_d[i]));
		}

		printf("\n");

		//ZP0
		for (i = 0; i < data->size_pg; i++) {
			printf("data->zpg_d[%d] = %9f\n", i, data->zpg_d[i]);
		}
		printf("\n&(data->size_pg) = %p\n", &(data->size_pg));
		for (i = 0; i < data->size_pg; i++) {
			printf("&(data->zpg_d[%d]) = %p\n", i, &(data->zpg_d[i]));
		}


		printf("\n\n");
	}

	if(data->size_pcm > 0)
	{
		printf("Send pcm\n");

		//XPCM
		for (i = 0; i < data->size_pcm; i++) {
			printf("data->xpcm_d[%d] = %9f\n", i, data->xpcm_d[i]);
		}
		printf("\n&(data->size_pcm) = %p\n", &(data->size_pcm));
		for (i = 0; i < data->size_pcm; i++) {
			printf("&(data->xpcm_d[%d]) = %p\n", i, &(data->xpcm_d[i]));
		}

		printf("\n");

		//YPCM
		for (i = 0; i < data->size_pcm; i++) {
			printf("data->ypcm_d[%d] = %9f\n", i, data->ypcm_d[i]);
		}
		printf("\n&(data->size_pcm) = %p\n", &(data->size_pcm));
		for (i = 0; i < data->size_pcm; i++) {
			printf("&(data->ypcm_d[%d]) = %p\n", i, &(data->ypcm_d[i]));
		}

		printf("\n");

		//ZPCM
		for (i = 0; i < data->size_pcm; i++) {
			printf("data->zpcm_d[%d] = %9f\n", i, data->zpcm_d[i]);
		}
		printf("\n&(data->size_pcm) = %p\n", &(data->size_pcm));
		for (i = 0; i < data->size_pcm; i++) {
			printf("&(data->zpcm_d[%d]) = %p\n", i, &(data->zpcm_d[i]));
		}

		printf("\n\n");
	}

#endif


}
