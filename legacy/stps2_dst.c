/*
 * stps2_dst.c 
 *
 * $Source:$
 * $Log:$
 *
 * This file includes the pack/unpack/ascii-dump routines for the STPS2 bank
 * defined in stps2_dst.h
 *
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_err_codes.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "stps2_dst.h"  

stps2_dst_common stps2_;  /* allocate memory to stps2_common */

static integer4 stps2_blen = 0; 
static integer4 stps2_maxlen = sizeof(integer4) * 2 + sizeof(stps2_dst_common);
static integer1 *stps2_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* stps2_bank_buffer_ (integer4* stps2_bank_buffer_size)
{
  (*stps2_bank_buffer_size) = stps2_blen;
  return stps2_bank;
}



static void stps2_bank_init(void)
{
  stps2_bank = (integer1 *)calloc(stps2_maxlen, sizeof(integer1));
  if (stps2_bank==NULL)
    {
      fprintf(stderr, 
	      "stps2_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    }
}

integer4 stps2_common_to_bank_(void)
{	
  static integer4 id = STPS2_BANKID, ver = STPS2_BANKVERSION;
  integer4 rcode, nobj;
  integer4 ieye;

  if ( stps2_bank == NULL ) stps2_bank_init();

  /* Initialize stps2_blen, and pack the id and version to bank */
  if ((rcode = dst_initbank_(&id, &ver, &stps2_blen, &stps2_maxlen, stps2_bank)))
    return rcode;

  //maxeye, if_eye
  if ((rcode = dst_packi4_( &stps2_.maxeye, (nobj=1, &nobj), stps2_bank, &stps2_blen, &stps2_maxlen))) return rcode;
  if ((rcode = dst_packi4_( stps2_.if_eye,  (nobj=stps2_.maxeye, &nobj), stps2_bank, &stps2_blen, &stps2_maxlen))) return rcode;

  /* 
     Pack plog, rvec, rwalk, aveTime, sigmaTime, avePhot, sigmaPhot, 
     lifetime, totalLifetime, and ang.. all real4's.. 
  */
  for ( ieye=0; ieye<stps2_.maxeye; ++ieye ) {
    if ( stps2_.if_eye[ieye] != 1 ) continue;

    if ((rcode = dst_packr4_( &(stps2_.plog[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.rvec[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.rwalk[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.ang[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.aveTime[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.sigmaTime[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.avePhot[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.sigmaPhot[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.lifetime[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_packr4_( &(stps2_.totalLifetime[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
  
    /* Pack inTimeTubes.. */
    if ((rcode = dst_packi4_( &(stps2_.inTimeTubes[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
  
    /* Pack the only integer1: the upward bit.. */
    if ((rcode = dst_packi1_( &(stps2_.upward[ieye]), (nobj=1, &nobj), stps2_bank,
			      &stps2_blen, &stps2_maxlen)))
      return rcode;
  }

  return SUCCESS;
}


integer4 stps2_bank_to_dst_(integer4 *NumUnit)
{	
  return dst_write_bank_(NumUnit, &stps2_blen, stps2_bank );
}

integer4 stps2_common_to_dst_(integer4 *NumUnit)
{
  integer4 rcode;
  if ((rcode = stps2_common_to_bank_()))
    {
      fprintf (stderr,"stps2_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit(0);			 	
    }             
  if ((rcode = stps2_bank_to_dst_(NumUnit)))
    {
      fprintf (stderr,"stps2_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit(0);			 	
    }

  return SUCCESS;
}

integer4 stps2_bank_to_common_(integer1 *bank)
{
  integer4 rcode = 0;
  integer4 nobj;
  integer4 ieye;
  
  stps2_blen = 2 * sizeof(integer4); /* skip id and version  */

  /* 
     Unpack plog, rvec, rwalk, aveTime, sigmaTime, avePhot, sigmaPhot, 
     lifetime, totalLifetime, and ang.. all real4's.. 
  */


  // maxeye, if_eye
  if ((rcode = dst_unpacki4_( &stps2_.maxeye, (nobj=1, &nobj), bank, &stps2_blen, &stps2_maxlen))) return rcode;
  if ((rcode = dst_unpacki4_( stps2_.if_eye,  (nobj=stps2_.maxeye, &nobj), bank, &stps2_blen, &stps2_maxlen))) return rcode;

  
  for ( ieye=0; ieye<stps2_.maxeye; ++ieye ) {
    if ( stps2_.if_eye[ieye] != 1 ) continue;

    if ((rcode = dst_unpackr4_( &(stps2_.plog[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.rvec[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.rwalk[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.ang[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.aveTime[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.sigmaTime[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.avePhot[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.sigmaPhot[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.lifetime[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    if ((rcode = dst_unpackr4_( &(stps2_.totalLifetime[ieye]), (nobj=1,&nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;

    /* Unpack inTimeTubes.. */
    if ((rcode = dst_unpacki4_( &(stps2_.inTimeTubes[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
    
    /* Unpack the only integer1: upward.. */
    if ((rcode = dst_unpacki1_( &(stps2_.upward[ieye]), (nobj=1, &nobj), bank,
			       &stps2_blen, &stps2_maxlen)))
      return rcode;
  }

  return SUCCESS;
}

integer4 stps2_common_to_dump_(integer4 *long_output)
{
  return stps2_common_to_dumpf_(stdout, long_output);
}

integer4 stps2_common_to_dumpf_(FILE* fp, integer4 *long_output)
{
  integer4 ieye;

  fprintf(fp,"\nSTPS2 bank. \n\n");

  /* Short output.. */
  for ( ieye=0; ieye<stps2_.maxeye; ++ieye ) {
    if ( stps2_.if_eye[ieye] != 1 ) continue;

    fprintf(fp, "eyeid %d  ________________________________\n", ieye + 1 );
    fprintf(fp, "Event based Plog:\t\t%7.2f\n", stps2_.plog[ieye]);
    fprintf(fp, "Event Rayleigh Vector Mag:\t%7.2f\n", stps2_.rvec[ieye]);
    fprintf(fp, "Time spread of all tubes:\t%7.2f us\n", stps2_.totalLifetime[ieye]);  
    fprintf(fp, "Time spread of in-time tubes:\t%7.2f us\n", stps2_.lifetime[ieye]); 

    if ( *long_output == 1 ) {
      /* Long output.. */
      fprintf(fp, "Random Walk Vector Mag:\t\t%7.2f\n", stps2_.rwalk[ieye]);
      fprintf(fp, "Mean tube trigger time:\t\t%7.2f us\n", stps2_.aveTime[ieye]);
      fprintf(fp, "Spread of trigger times:\t%7.2f us\n", stps2_.sigmaTime[ieye]);
      fprintf(fp, "Mean calibrated photons:%15.2f\n", stps2_.avePhot[ieye]);
      fprintf(fp, "Spread of calibrated photons:%10.2f\n", stps2_.sigmaPhot[ieye]);
      fprintf(fp, "Upward:\t\t\t\t%7s\n", (stps2_.upward[ieye] ? "Yes" : "No"));
      fprintf(fp, "Angle: \t\t\t\t%7.2f degrees\n", stps2_.ang[ieye]);
    }
  }
  fprintf(fp, "\n");

  return SUCCESS;
}



