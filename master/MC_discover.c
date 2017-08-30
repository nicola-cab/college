/*
 * MC_discover.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifdef MC_REPLICA__

#include "MC_discover.h"

extern STATUS MC_task_status;

bool read_from_file(md_discovery_t *MDs, int *size)
{
	bool status = FALSE;
	char entry[80];
	char tmp_entry[80];
	int i = 0;
	char info[3][20];
	int index_MDs = 0;
	bool flag_found_md = FALSE;
	FILE *fp;
	int sure_flag = 2;
	*size = 0;

	md_discovery_t *tmp = NULL;


	debug_MC_log("Read MDs entry from discovery file");

	if( MC_DOWN == MC_task_status )
	{
		fp = fopen(get_path_discovery_file(), "r");
		if(fp == NULL)
		{
			critical_MC_log("Impossible to read discovery file on shared space");
		}
		else
		{
			while(!feof(fp))  //leggi il file
			{
				 if(fgets(entry, 80, fp))
				 {
					 while(i < strlen(entry) )
					 {
						 tmp_entry[i] = entry[i];
						 i++;
					 }
					 tmp_entry[i-1] = '\0';  //delete \n
					 i = 0;

					 //printf("\n%s..%d\n",tmp_entry, strlen(tmp_entry));

					 //split process
					 int j = 0;
					 int k = 0;
					 int index_str = 0;
					 while(j < strlen(tmp_entry))
					 {
						 if(tmp_entry[j] == '-')
						 {
							 //int size = strlen(info[index_str]);
							 info[index_str][k] = '\0';
							 index_str ++;
							 k = 0;
							 sure_flag ++;
						 }
						 else
						 {
							 info[index_str][k] = tmp_entry[j];
							 k++;
						 }
						 j++;
					 }
					 info[index_str][k] = '\0';


					 //populate the data structure

					 //first item
					 if(!index_MDs)
					 {
						 //first element
						 MDs->MD_interface = (string)malloc(sizeof(char)*20);
						 MDs->MD_ip_address = (string)malloc(sizeof(char)*20);
						 strcpy(MDs->MD_interface  , info[0]);
						 strcpy(MDs->MD_ip_address , info[1]);
						 MDs->MD_status = atoi(info[2]);
						 MDs->next = NULL;
					 }
					 //other items
					 else
					 {
						 tmp = MDs;
						 while(tmp->next)
						 {
							 tmp = tmp->next;
						 }
						 tmp->next = (md_discovery_t *)malloc(sizeof(md_discovery_t));
						 tmp->next->MD_interface = (string)malloc(sizeof(char)*20);
						 tmp->next->MD_ip_address = (string)malloc(sizeof(char)*20);
						 strcpy(tmp->next->MD_interface  , info[0]);
						 strcpy(tmp->next->MD_ip_address , info[1]);
						 tmp->next->MD_status = atoi(info[2]);
						 tmp->next->next = NULL;
					 }

					 if( FALSE == flag_found_md )  //set flag
						 flag_found_md = TRUE;

					 index_MDs++;
				 }
			}
		}

		if(index_MDs > 0 )
		{
			//set status
			if(flag_found_md == TRUE){
				status = TRUE;
				*size = index_MDs;
			}
		}

		fclose(fp);
	}

	return status;
}



bool MD_Discover(md_discovery_t * md , int * size)
{
	bool status = FALSE;

	status = read_from_file(md, &(*size));

	if( FALSE == status )
	{
		free(md);
		critical_MC_log("Unable to find a MD in the discovery file!!!");
	}
	else
	{
		if(*size > 0)
		{
			debug_MC_log("MD list complete!!");
			status = TRUE;
		}
		else
		{
			error_MC_log("No MD entry found !!");
		}
	}

	return status;
}

#endif
