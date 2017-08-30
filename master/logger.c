/*
 * logger.c
 *
 *  Created on: Jan 23, 2012
 *      Author: nicola
 */


#include "logger.h"

//File descriptor for the MD log file
FILE * fd_md = NULL;
//File descriptor for the MC log file
FILE * fd_mc = NULL;


int g_Replica_id = 0;

void set_Replica_ID(int MD_id)
{
	g_Replica_id = MD_id;
}

bool InitLoggingtask(u_int8_t loggerType)
{
	bool status = FALSE;

	switch(loggerType)
	{
	case MD_LOGGER:
	{
		status = openfile(loggerType);
		break;
	}
	case MC_LOGGER:
	{
		status = openfile(loggerType);
		break;
	}
	default:
		break;
	}

	return status;
}

bool closeLogginTask(u_int8_t loggerType)
{
	bool status = FALSE;

	switch(loggerType)
	{
	case MD_LOGGER:
	{
		status = closefile(loggerType);
		break;
	}
	case MC_LOGGER:
	{
		status = closefile(loggerType);

		break;
	}
	default:
		break;
	}

	return status;
}

bool Log(u_int8_t loggerType, STATUS log_level, string msg)
{
	bool status = FALSE;

	if(( !fd_md ) || ( !fd_mc ))
	{
		status = InitLoggingtask(loggerType);
	}

	if(status == TRUE){

		char custom_msg[LOG_STR_LEN];
		char log_level_str[LOG_STR_STATUS_LEN];
		char time[LOG_STR_TIME_LEN];

		getTimeStamp(time);


		switch(log_level)
		{
		case E_INFO:
			sprintf(log_level_str,"%s","INFO");
			break;
		case E_DEBUG:
			sprintf(log_level_str,"%s","DEBUG");
			break;
		case E_ERROR:
			sprintf(log_level_str,"%s","ERROR");
			break;
		case E_CRITICAL:
			sprintf(log_level_str,"%s","CRITICAL");
			break;
		default:
			break;
		}

		sprintf(custom_msg,"[ %s ] - %s - %s \n", time, log_level_str, msg);
		status = writefile(loggerType,log_level,custom_msg);


	}

	return status;
}


//Private use only

bool openfile(u_int8_t loggerType)
{
	bool status = FALSE;

	switch(loggerType)
	{
		case MD_LOGGER:
		{

			char MD_log_file_name[30] ;
			sprintf(MD_log_file_name,"%s%d.log",MD_LOG_FILE,g_Replica_id);


			fd_md = fopen(MD_log_file_name,"a"); //I think that don't will be problems because the only processor 0 executes the library
			if( NULL == fd_md ) //error
			{
				status = FALSE;
			}
			else
			{
				status = TRUE;
			}

			break;
		}
		case MC_LOGGER:
		{
			char MC_log_file_name[30] ;
			sprintf(MC_log_file_name,"%s%d.log",MC_LOG_FILE,g_Replica_id);

			fd_mc = fopen(MC_log_file_name,"a");
			if( NULL == fd_mc ) //error
			{
				status = FALSE;
			}
			else
			{
				status = TRUE;
			}

			break;
		}
		default:
			break;
	}

	return status;
}

bool closefile(u_int8_t loggerType)
{
	bool status = FALSE;
	int ret ;


	if(MD_LOGGER == loggerType)
	{
		ret = fclose(fd_md);
		fd_md = NULL;
	}
	if(MC_LOGGER == loggerType)
	{
		ret = fclose(fd_mc);
		fd_mc = NULL;
	}

	if(ERROR == ret)
	{
		status = FALSE;
	}
	else
	{
		status = TRUE;
	}

	return status;
}

bool writefile( u_int8_t loggerType , STATUS log_level, string msg)
{
	bool status = FALSE;
	ssize_t nbytes = 0;

	if( MD_LOGGER == loggerType )
	{
		nbytes = fprintf(fd_md,"%s",msg);
		closefile(loggerType);
	}
	if( MC_LOGGER == loggerType )
	{
		nbytes = fprintf(fd_mc,"%s",msg);
		closefile(loggerType);
	}

	if(nbytes < 0)
	{
		status = FALSE;
	}
	else
	{
		status = TRUE;
	}

	return status;
}

void getTimeStamp(string time_str)
{
	 time_t t = time(NULL);   // get time now
	 struct tm * now = localtime( & t );
	 sprintf(time_str,"%d/%d/%d : %d:%d:%d",(now->tm_year + 1900), (now->tm_mon+1), now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
}

bool info_MD_log(string msg)
{
	bool status = FALSE;
	status = Log(MD_LOGGER, E_INFO, msg);
	return status;
}
bool debug_MD_log(string msg)
{
	bool status = FALSE;
	status = Log(MD_LOGGER, E_DEBUG, msg);
	return status;
}
bool error_MD_log(string msg)
{
	bool status = FALSE;
	status = Log(MD_LOGGER, E_ERROR, msg);
	return status;
}
bool critical_MD_log(string msg)
{
	bool status = FALSE;
	Log(MD_LOGGER,E_CRITICAL,msg);
	exit(ERROR);

	//no return
	return status;
}

bool info_MC_log(string msg)
{
	bool status = FALSE;
	status = Log(MC_LOGGER, E_INFO, msg);
	return status;
}
bool debug_MC_log(string msg)
{
	bool status = FALSE;
	status = Log(MC_LOGGER, E_DEBUG, msg);
	return status;
}
bool error_MC_log(string msg)
{
	bool status = FALSE;
	status = Log(MC_LOGGER, E_ERROR, msg);
	return status;
}
bool critical_MC_log(string msg)
{
	bool status = FALSE;
	status = Log(MC_LOGGER, E_CRITICAL, msg);
	exit(ERROR);
	return status;
}

