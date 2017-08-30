/*
 * orac.c
 *
 *  Created on: Jan 22, 2012 (Integration step)
 *      Author: nicola
 */

#ifdef MD_REPLICA__

#include "orac.h"
#include "logger.h"

//variable to check if init MD has been called
#define FALSE_MD_INIT 0
#define TRUE_MD_INIT 1
unsigned short int init_MD_Called = FALSE_MD_INIT;

int orac_md_init_(char * input_file, int len,
		char *errmsg, int size_errmsg,
		int * my_id )
{
    int iret = 0;

    sprintf(errmsg,"%s","");

    debug_MD_log("MD ORAC INIT ... CALLING FORTRAN TO DO INITIALIZATION ... ");

    debug_MD_log("Input file ----> ");
    debug_MD_log(input_file);
  
    /*md_init_(input_file, &len,
             &iret,
             errmsg, &size_errmsg,
             &(*my_id),
             orac.mapnl_p,
             orac.xp0_p, orac.yp0_p, orac.zp0_p,
             orac.xpg_p, orac.ypg_p, orac.zpg_p,
             orac.gh_p);*/

    if( iret < 0 )
    	debug_MD_log("MD ORAC INIT FAIL ... MD NOT AVAILABLE ");
    else
	init_MD_Called=TRUE_MD_INIT;  //MD Init finish with success

    return iret;
}

int orac_md_exec_(char *errmsg, int size_errmsg,molecular_t * data)
{
    int iret = 0;

    sprintf(errmsg,"%s","");

    debug_MD_log("MD ORAC EXEC ... CALLING FORTRAN TO DO EXECUTION ...");

    if(init_MD_Called == TRUE_MD_INIT)
    {	
      debug_MD_log("MD is invoked");

      if(data->size_mapnl > 0)
      {
    	  debug_MD_log("MD pass to orac MAPNL");
    	  orac.mapnl_p = data->mapnl_d;
      }
      if(data->size_gh > 0)
      {
    	  debug_MD_log("MD pass to orac GH");
    	  orac.gh_p = data->gh_d;
      }
      if(data->size_p0 > 0)
      {
    	  debug_MD_log("MD pass to orac *P0");
    	  orac.xp0_p = data->xp0_d;
    	  orac.yp0_p = data->yp0_d;
    	  orac.zp0_p = data->zp0_d;
      }
      if(data->size_pg > 0)
      {
    	  debug_MD_log("MD pass to orac *PG");
    	  orac.xpg_p = data->xpg_d;
    	  orac.ypg_p = data->ypg_d;
    	  orac.zpg_p = data->zpg_d;
      }
      if(data->size_pcm > 0)
      {
    	  debug_MD_log("MD pass to orac *PMC");
    	  orac.xpcm_p = data->xpcm_d;
    	  orac.ypcm_p = data->ypcm_d;
    	  orac.zpcm_p = data->zpcm_d;
      }


#if 0

      //Testing modification of data..

      printf("PAY ATTENTION \n");
      printf("IF IT IS PRINTED THIS MESSAGE THAN YOU ARE NOT USING ORAC ... CONTROL orac.c \n ");

      unsigned int i = 0;

      for( i = 0 ; i < data->size_mapnl ; i++)
    	  orac.mapnl_p[i] *= 2;

      for( i = 0 ; i < data->size_gh ; i++)
          	  orac.gh_p[i] *= 2;

      for( i = 0 ; i < data->size_p0 ; i++)
      {
    	  orac.xp0_p[i] *= 2;
    	  orac.yp0_p[i] *= 2;
    	  orac.zp0_p[i] *= 2;
      }

      for( i = 0 ; i < data->size_pg ; i++)
      {
    	  orac.xpg_p[i] *= 2;
    	  orac.ypg_p[i] *= 2;
    	  orac.zpg_p[i] *= 2;
      }

      for( i = 0 ; i < data->size_pcm ; i++)
      {
    	  orac.xpcm_p[i] *= 2;
    	  orac.ypcm_p[i] *= 2;
    	  orac.zpcm_p[i] *= 2;
      }

#else
    	  /*md_exec_(orac.mapnl_p,
    	    		  orac.xp0_p, orac.yp0_p, orac.zp0_p,
    	    		  orac.xpg_p, orac.ypg_p, orac.zpg_p,
    	    		  orac.gh_p,
    	    		  orac.xpcm_p, orac.ypcm_p, orac.zpcm_p,
    	    		  &iret,errmsg,&size_errmsg);*/

    	  if(iret < 0)
    		  debug_MD_log("MD ORAC EXEC FAIL ... MD WILL NO RETURN DATA TO MC ...");

#endif


    }
    else
     {
       debug_MD_log("MD ORAC INIT ERROR ...");
     }  

    return iret;
}

#endif
