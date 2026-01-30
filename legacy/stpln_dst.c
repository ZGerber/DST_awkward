/*
 * $Source:$
 * $Log:$
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
#include "stpln_dst.h"  

stpln_dst_common stpln_;  /* allocate memory to stpln_common */

static integer4 stpln_blen = 0; 
static integer4 stpln_maxlen = sizeof(integer4) * 2 + sizeof(stpln_dst_common);
static integer1 *stpln_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* stpln_bank_buffer_ (integer4* stpln_bank_buffer_size)
{
  (*stpln_bank_buffer_size) = stpln_blen;
  return stpln_bank;
}



static void stpln_bank_init()
{
  stpln_bank = (integer1 *)calloc(stpln_maxlen, sizeof(integer1));
  if (stpln_bank==NULL)
    {
      fprintf(stderr, "stpln_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    }
}

integer4 stpln_common_to_bank_()
{	
  static integer4 id = STPLN_BANKID, ver = STPLN_BANKVERSION;

  integer4 ieye;
  integer4 rcode;
  integer4 nobj;

  if (stpln_bank == NULL) stpln_bank_init();

  /* Initialize stpln_blen, and pack the id and version to bank */
  if ((rcode = dst_initbank_(&id, &ver, &stpln_blen, &stpln_maxlen, stpln_bank))) return rcode;

  // jday, jsec, msec
  if ((rcode = dst_packi4_    (&stpln_.jday, (nobj=3, &nobj), stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;

  // neye, nmir, ntube
  if ((rcode = dst_packi4asi2_(&stpln_.neye, (nobj=3, &nobj), stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;

  //maxeye, if_eye
  if ((rcode = dst_packi4_( &stpln_.maxeye, (nobj=1, &nobj), stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4_( stpln_.if_eye,  (nobj=stpln_.maxeye, &nobj), stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;

  for ( ieye=0; ieye<stpln_.maxeye; ++ieye ) {
    if ( stpln_.if_eye[ieye] != 1 ) continue;

    nobj = 1; 

    if ((rcode = dst_packi4asi2_(&(stpln_.eyeid     [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_packi4asi2_(&(stpln_.eye_nmir  [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_packi4asi2_(&(stpln_.eye_ngmir [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_packi4asi2_(&(stpln_.eye_ntube [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_packi4asi2_(&(stpln_.eye_ngtube[ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;

    if ((rcode = dst_packr4_    (&(stpln_.rmsdevpln   [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;
    if ((rcode = dst_packr4_    (&(stpln_.rmsdevtim   [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;
    if ((rcode = dst_packr4_    (&(stpln_.tracklength [ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;
    if ((rcode = dst_packr4_    (&(stpln_.crossingtime[ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;
    if ((rcode = dst_packr4_    (&(stpln_.ph_per_gtube[ieye]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;

    // plane + errors
    nobj = 3;
    if ((rcode = dst_packr4_    (&(stpln_.n_ampwt   [ieye][0]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;
    nobj = 6; 
    if ((rcode = dst_packr4_    (&(stpln_.errn_ampwt[ieye][0]), &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode ;
  }

  nobj = stpln_.nmir; 
  if ((rcode = dst_packi4asi2_(stpln_.mirid, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4asi2_(stpln_.mir_eye, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4asi2_(stpln_.mir_type, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4_    (stpln_.mir_ngtube, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4_    (stpln_.mirtime_ns, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;

  nobj = stpln_.ntube;
  if ((rcode = dst_packi4asi2_(stpln_.ig, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4asi2_(stpln_.tube_eye, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  
  /* Variables that appear STPLN_BANKVERSION >= 2 */
  nobj = stpln_.ntube;
  if ((rcode = dst_packi4_(stpln_.saturated, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_packi4_(stpln_.mir_tube_id, &nobj, stpln_bank, &stpln_blen, &stpln_maxlen))) return rcode;
  
  return SUCCESS;
}

integer4 stpln_bank_to_dst_(integer4 *NumUnit)
{	
  return dst_write_bank_(NumUnit, &stpln_blen, stpln_bank );
}

integer4 stpln_common_to_dst_(integer4 *NumUnit)
{
  integer4 rcode;
  if ((rcode = stpln_common_to_bank_()))
    {
      fprintf (stderr,"stpln_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit(0);			 	
    }             
  if ((rcode = stpln_bank_to_dst_(NumUnit)))
    {
      fprintf (stderr,"stpln_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit(0);			 	
    }
  return SUCCESS;
}

integer4 stpln_bank_to_common_(integer1 *bank)
{
  integer4 ieye;
  integer4 rcode = 0;
  integer4 nobj;
  
  integer4 bankid, bankversion;
  
  stpln_blen = 0;
  
  if ((rcode = dst_unpacki4_( &bankid,      (nobj=1, &nobj), bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki4_( &bankversion, (nobj=1, &nobj), bank, &stpln_blen, &stpln_maxlen))) return rcode;


  // jday, jsec, msec
  if ((rcode = dst_unpacki4_(    &(stpln_.jday), (nobj = 3, &nobj) ,bank, &stpln_blen, &stpln_maxlen))) return rcode;

  // neye, nmir, ntube
  if ((rcode = dst_unpacki2asi4_(&(stpln_.neye), (nobj = 3, &nobj), bank, &stpln_blen, &stpln_maxlen))) return rcode;

  // maxeye, if_eye
  if ((rcode = dst_unpacki4_( &stpln_.maxeye, (nobj=1, &nobj), bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki4_( stpln_.if_eye,  (nobj=stpln_.maxeye, &nobj), bank, &stpln_blen, &stpln_maxlen))) return rcode;

  for ( ieye=0; ieye<stpln_.maxeye; ++ieye ) {
    if ( stpln_.if_eye[ieye] != 1 ) continue;

    nobj = 1;
    if ((rcode = dst_unpacki2asi4_(&(stpln_.eyeid     [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpacki2asi4_(&(stpln_.eye_nmir  [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpacki2asi4_(&(stpln_.eye_ngmir [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpacki2asi4_(&(stpln_.eye_ntube [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpacki2asi4_(&(stpln_.eye_ngtube[ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;

    if ((rcode = dst_unpackr4_(&(stpln_.rmsdevpln   [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpackr4_(&(stpln_.rmsdevtim   [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpackr4_(&(stpln_.tracklength [ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpackr4_(&(stpln_.crossingtime[ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    if ((rcode = dst_unpackr4_(&(stpln_.ph_per_gtube[ieye]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;

    nobj = 3;
    if ((rcode = dst_unpackr4_(&(stpln_.n_ampwt   [ieye][0]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    nobj = 6;
    if ((rcode = dst_unpackr4_(&(stpln_.errn_ampwt[ieye][0]), &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  }
 
  nobj = stpln_.nmir; 
  if ((rcode = dst_unpacki2asi4_(stpln_.mirid, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki2asi4_(stpln_.mir_eye, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki2asi4_(stpln_.mir_type, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki4_    (stpln_.mir_ngtube, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki4_    (stpln_.mirtime_ns, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;


  nobj = stpln_.ntube; 
  if ((rcode = dst_unpacki2asi4_(stpln_.ig, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  if ((rcode = dst_unpacki2asi4_(stpln_.tube_eye, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
  
  if(bankversion >= 2)
    {
      nobj = stpln_.ntube;
      if ((rcode = dst_unpacki4_(stpln_.saturated, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
      if ((rcode = dst_unpacki4_(stpln_.mir_tube_id, &nobj,bank, &stpln_blen, &stpln_maxlen))) return rcode;
    }
  else
    {
      /* don't have this information if reading out a DST file that was written using
	 an older version of the bank (ver < 2)*/
      int i;
      for (i=0; i<stpln_.ntube; i++)
	{
	  stpln_.saturated[i]   = 0;
	  stpln_.mir_tube_id[i] = 0;
	}
    }
  
  return SUCCESS;
}

integer4 stpln_common_to_dump_(integer4 *long_output)
{
  return stpln_common_to_dumpf_(stdout, long_output);
}

integer4 stpln_common_to_dumpf_(FILE* fp, integer4 *long_output)
{
  integer4 i;
  integer4 ieye;
  fprintf(fp, "\nSTPLN jDay/Sec: %d/%5.5d %02d:%02d:%02d.%03d ",
          stpln_.jday, stpln_.jsec, stpln_.msec / 3600000,
          (stpln_.msec / 60000) % 60, (stpln_.msec / 1000) % 60,
          stpln_.msec % 1000) ;

  fprintf(fp, "eyes: %2d  mirs: %2d  tubes: %4d \n",
          stpln_.neye, stpln_.nmir, stpln_.ntube);

  for ( ieye=0; ieye<stpln_.maxeye; ++ieye ) {
    if ( stpln_.if_eye[ieye] != 1 ) continue;

    fprintf(fp, "eyeid %d  ________________________________\n", ieye + 1 );
    fprintf(fp,"      n_ampwt   errn_ampwt\n");
    fprintf(fp, "  %11.8f %11.8f\n", 
	    stpln_.n_ampwt[ieye][0], sqrt(stpln_.errn_ampwt[ieye][0]));
    fprintf(fp, "  %11.8f %11.8f\n", 
	    stpln_.n_ampwt[ieye][1], sqrt(stpln_.errn_ampwt[ieye][3]));
    fprintf(fp, "  %11.8f %11.8f\n\n", 
	    stpln_.n_ampwt[ieye][2], sqrt(stpln_.errn_ampwt[ieye][5]));

      fprintf(fp,"  track info:\n  tracklength   : %11.8f\n",
	      stpln_.tracklength[ieye]);
    fprintf(fp,"  crossing time : %11.8f\n  ph_per_gtube  : %11.8f\n",
	    stpln_.crossingtime[ieye], stpln_.ph_per_gtube[ieye]);

    fprintf(fp,"  rmsdevpln : %11.8f\n  rmsdevtim : %11.8f\n",
	    stpln_.rmsdevpln[ieye], stpln_.rmsdevtim[ieye]);
  }

  fprintf(fp, "________________________________________\n" );

  /* -------------- mir info --------------------------*/

  for (i = 0; i < stpln_.nmir; i++) {
    fprintf(fp, " eye %2d mir %2d Rev %d  gtubes: %3d  ",
	    stpln_.mir_eye[i], stpln_.mirid[i], stpln_.mir_type[i],
	    stpln_.mir_ngtube[i]);
    fprintf(fp, "time: %10dnS\n",
	    stpln_.mirtime_ns[i] );
  }

  
  /* If long output is desired, show tube information */

  if ( *long_output == 1 ) {
    for (i = 0; i < stpln_.ntube; i++) {
      fprintf(fp, "itube %3d  eyeid %2d  mir_tube_id %5d saturated %2d ig %d\n", i, 
	      stpln_.tube_eye[i], stpln_.mir_tube_id[i], stpln_.saturated[i],stpln_.ig[i]);
    }
  }
  
  fprintf(fp,"\n");

  return SUCCESS;
}

