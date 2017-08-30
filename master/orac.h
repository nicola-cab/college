/*
* orac_interface.h
*
*  Created on: May 21, 2012 (Integration Step)
*      Author: nicola
*
*      Description: orac_interface.h include file defines at interface which allow WrapperLib to call fortran ORAC program
*                    using routines defined inside fortran_interface header file.
*                    Use the routines defined here is most important because all ORAC init data stuctures, which are needed when a new
*                    Molecular dynamic simulation is invoked, are stored at this level
*
*                   1) orac_md_init_ : to allow WrapperLib MD side to call ORAC molecular dynamic init routine
*                   2) orac_md_exec_ : to allow WrapperLib MD side to call ORAC molecular dynamic execution routine
*
*                   Basically in this file is declared the ORAC FORTRAN --> C WRAPPER interface
*/


#ifndef ORAC_INTERFACE_H_
#define ORAC_INTERFACE_H_

#include "fortran_interface.h"

typedef struct
{
    int * mapnl_p;
    double * xp0_p;
    double * yp0_p;
    double * zp0_p;
    double * xpg_p;
    double * ypg_p;
    double * zpg_p;
    double * gh_p;
    double * xpcm_p;
    double * ypcm_p;
    double * zpcm_p;

}orac_t;

//Orac data struct to store molecular dynamic init data
orac_t orac;


/**
 * orac_md_init_
 *
 * Initialize algorithm to prepare MD simulation passing from fortran_interface.
 * If initialization works fine than all data structures relative to initiliazation
 * has stored inside "orac_data" and passed to "orac_md_exec" when a new molecular
 * dynamic simulation is requested
 *
 * */
int orac_md_init_(char * input_file, int len,
		char *errmsg, int size_errmsg,
		int * my_id);

/**
 * orac_md_exec_
 *
 * Execute MD simulation using initialization information stored inside "orac_data"
 * from "orac_md_init".
 * */
int orac_md_exec_(char *errmsg, int size_errmsg, molecular_t * data);

#endif
