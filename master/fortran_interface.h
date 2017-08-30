 /*
 * fortran_interface.h
 *
 *  Created on: May 21, 2012 (Integration Step)
 *      Author: nicola
 *
 *      Description: fortran_interface.h include file allow WrapperLib to call fortran ORAC program. In this file there are two declarations.
 *
 *                   1) md_init_ : to allow WrapperLib MD side to call ORAC molecular dynamic init routine
 *                   2) md_exec_ : to allow WrapperLib MD side to call ORAC molecular dynamic execution routine
 *
 *                   Basically in this file is declared the ORAC FORTRAN --> C WRAPPER interface
 */

#ifndef ORAC_FORTRAN_H_
#define ORAC_FORTRAN_H_

#include "proto.h"

#include <stdio.h>
#include <mpi.h>

/**
 * Constants based on original fortran program (orac)
 * */

#define ERRMSG_SIZE 80
#define MAPNL_SIZE 200000*10
#define P0_SIZE 200000
#define PG_SIZE 80000
#define PCM_SIZE 150000
#define GH_SIZE 50

/**
 * md_init_
 *
 * Initialize algorithm to prepare MD simulation.
 *
 * IMPORTANT: To call when program starts and before of "md_exec_" routine.
 *            Parameters name never change!!! ... it's very import because the mapping is 
 *            one2one with orac fortran interface
 * */

void md_init_(char *input_file, int *len_str, int *iret, char *errmsg, int *size_errmsg,
              int *my_id,
              int mapnl_p[MAPNL_SIZE],
              double xp0_p[P0_SIZE], double yp0_p[P0_SIZE], double zp0_p[P0_SIZE],
              double xpg_p[PG_SIZE],double ypg_p[PG_SIZE],double zpg_p[PG_SIZE],
              double gh_p[GH_SIZE]);

/**
 * med_exec_
 *
 * Execution of MD simulation
 *
 * IMPORTANT: To call after "md_init_" routine
 *            Parameters name never change!!! ... it's very import because the mapping is 
 *            one2one with orac fortran interface
 * */

void md_exec_(int mapnl[MAPNL_SIZE],
              double xp0[P0_SIZE], double yp0[P0_SIZE], double zp0[P0_SIZE],
              double xpg[PG_SIZE],double ypg[PG_SIZE],double zpg[PG_SIZE],
              double gh[GH_SIZE],
              double xpcm[PCM_SIZE], double ypcm[PCM_SIZE], double zpcm[PCM_SIZE],
              int *iret, char * errmsg, int *size_errmsg);

#endif
