/*
 * Common.h
 *
 *  Created on: Jan 23, 2012
 *      Author: nicola
 *
 *      Description: In this header file Common.h will be defined all common data structures or variables
 */

#ifndef COMMON_H_
#define COMMON_H_

#define DEBUG_STDOUT_PRINT  //if defined all printfs in the code will be enabled

// MD Unit Max Entry ( a single unit can be composed by 8/16/32 cores or more .... )
// This serves to store a sufficient number of MD Units! It does not represent the numbers of core allocate for each MD Unit
#define MAX_MD_UNITS 256

#define TRUE 1
#define FALSE 0

#define ERROR -1
#define OK 0

typedef double TYPE_MOLECULAR;
typedef char * string;
typedef unsigned short int  bool;
typedef short int STATUS;


#endif /* COMMON_H_ */
