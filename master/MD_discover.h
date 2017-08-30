/*
 * MD_discover.h
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifndef MD_DISCOVER_H_
#define MD_DISCOVER_H_


#include "proto.h"
#include "MD_MC_shared.h"


/**
 * Add to the file of MDs a new entry
 *
 * @param: md_entry
 *
 * @return: bool ... true if all info about new MD entry is retrieved correctly
 * 					 false otherwise
 * */
bool Discover_MD_IP(md_discovery_t * md_entry);

/**
 *
 * Save on the file new MD and all relative information about that
 *
 * @param: md_entry
 *
 * @return: bool ... true if the file is saved correctly
 * 					 false otherwise
 * */
bool save_on_file(md_discovery_t * md_entry);


/**
 * Get the ip address of the interface
 *
 * @param: domain (IPV4 domain)
 *
 * @return: string* (array composed of name interface and ip address)
 * */
string* getIpAddress(const int domain);



#endif /* MD_DISCOVER_H_ */
