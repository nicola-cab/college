/*
 * logger.h
 *
 *  Created on: Jan 23, 2012
 *      Author: nicola
 *
 *      Description: logger.h/c files define all methods which will be used in the code to add the logging function to the library
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "Common.h"

#define MD_LOG_FILE "Wrap-MD-"
#define MC_LOG_FILE "Wrap-MC-"

//Priorities of logging
#define E_INFO     0
#define E_DEBUG    1
#define E_ERROR    2
#define E_CRITICAL 3

#define MD_LOGGER 0
#define MC_LOGGER 1

//condictions of Log task
#define LOG_TASK_ACTIVE 0
#define LOG_TASK_DEACTIVE 1

#define LOG_STR_LEN 200
#define LOG_STR_STATUS_LEN 10
#define LOG_STR_TIME_LEN 30

//------------------------------------------------------------------
//Interface
//------------------------------------------------------------------

/**
 * Store the MD id to make possible to index the name of MD
 * based on id passed.
 *
 * e.g -> if MD_id is 0 then the MD log file is Wrap-MD-0.log
 *
 * @param: MD_id ... integer that indicates id of MD
 * */
void set_Replica_ID(int MD_id);

/**
 * Initialize the loggin procedure, this function is called by init task of the library
 *
 * @param:   loggerType ... the MD/MC logger to select the correct file descriptor
 * @return : true ... if the procedure ends correctly
 * 			 false ... otherwise
 * */
bool InitLoggingtask(u_int8_t loggerType);

/**
 * Close log file
 *
 * @param:  loggerType ... the MD/MC logger to select the correct file descriptor
 * @return: true ... if the close ends correctly
 * 			false ... otherwise
 * */
bool closeLogginTask(u_int8_t loggerType);

/**
 *
 * Logging on log file using the message passed as parameter and using log level
 *
 * @param: loggerType
 * @param: log_level
 * @param: msg
 *
 * @return: true ... logging operation ends correctly
 * 			false ... otherwise
 * */
bool Log(u_int8_t loggerType,STATUS log_level,string msg);


//----------------------------------------------------------------
//Private use only
//----------------------------------------------------------------

/**
 * Open the file log descriptor using parameter passed as parameter to select the correct fd
 *
 * @param typeLogger
 * @return: true ... operation ends with success
 * 			false ... otherwise
 * */
bool openfile(u_int8_t loggerType);

/**
 * Close the file log descriptor using parameter passed as parameter to select the correct fd
 *
 * @param: typeLogger
 *
 * @return: true ... operation ends with success
 * 			false ... otherwise
 * */
bool closefile(u_int8_t loggerType);

/**
 * Write on log file the message passed as parameter
 *
 * @param: loggerType
 * @param: log_level
 * @param: msg (the message)
 *
 * @return: true ... write operation ends with success
 * 			false ... otherwise
 * */
bool writefile(u_int8_t loggerType, STATUS log_level, string msg);

/**
 *
 * Retrieve the timestamp of the system stored in a string
 *
 * @return: string
 */
void getTimeStamp(string);


/**
 * Logging routines for MD
 * */
bool info_MD_log(string msg);
bool debug_MD_log(string msg);
bool error_MD_log(string msg);
bool critical_MD_log(string msg);

/**
 * Logging routines for MC
 * */
bool info_MC_log(string msg);
bool debug_MC_log(string msg);
bool error_MC_log(string msg);
bool critical_MC_log(string msg);



#endif /* LOGGER_H_ */
