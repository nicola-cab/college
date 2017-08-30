/*
 * MC_discover.h
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */



#ifndef MC_DISCOVER_H_
#define MC_DISCOVER_H_

#include "proto.h"
#include "MD_MC_shared.h"


/*
extern MDs_list_discoverd
- multiple definition of `MDs'
- multiple definition of `MD_Unit'
*/

/**
 *
 * Read from discovery file the list of MDs
 *
 * @param : MDs .. the list of MD entry in the file
 * @param : size ... numbers of entry readed
 *
 * @return: true ... if everything works fine
 * 			false .... otherwise
 *
 * */
bool read_from_file(md_discovery_t * MDs, int * size);


/**
 *
 * @param: md .. the new md found to connect
 * @param: size ... dimension of the md array
 *
 * @return : true .. if everything works fine
 * 			 false .. otherwise
 * */
bool MD_Discover(md_discovery_t *md, int *size);

#endif /* MC_DISCOVER_H_ */
