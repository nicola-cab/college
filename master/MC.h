/*
 * MC.h
 *
 *  Created on: Jan 26, 2012
 *      Author: nicola
 */

#ifndef MC_H_
#define MC_H_

#include "MC_comm.h"

#define SIZE_ARRAY(array) (sizeof(array)/sizeof(array[0]))

typedef int (* CALLBACK)(void *);

typedef struct{
	bool op_state;
	response_t  response;
}MD_response;


/**
 * Connect the MC client to MD server and execute a simulation.
 * Using this function the MC client use the library to connect
 * to MD server and to execute simulation.
 * The MD server is automatically chosen by the library
 * and this function is blocking.
 * This means that the function returns only if:
 * 		- The simulation has been executed correctly
 * 		- No MD simulation is executed because there is not a free MD
 *
 * @param: discovery_path --> discovery file path to ask simulation to MD
 * @param: mc_id --> mc_id (not required ... always 0 if it is present only one MC)
 * @param: x -> x arrays of coordinates
 * @param: y -> y arrays of coordinates
 * @param: z -> z arrays of coordinates
 * @param: size_x,y,z -> size of arrays
 * @param: resp -> the result of simulation
 *
 * */
bool Wrapper_MC_Ask_MD_Simulation(string discovery_path,
		unsigned short int mc_id,
		molecular_t * data,
		MD_response * resp);


/**
 * To use only for async request.
 * This routine release all MD resources allocated and MD come back in a free state
 * */
bool Wrapper_MC_Exit();


#endif /* MC_H_ */
