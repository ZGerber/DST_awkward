/*
 * $Source: /hires_soft/uvm2k/bank/stop_dst.c,v $
 * $Log: stop_dst.c,v $
 * Revision 1.3  1995/07/25 10:57:28  mjk
 * Change dst_initbank to dst_initbank_
 * Use return code SUCCESS (from dst_err_codes.h)
 *
 * Revision 1.2  1995/03/18  00:35:22  jeremy
 * *** empty log message ***
 *
 * modified by jT at 6:57 PM on 3/10/95
 * modified by jds at 11:09 AM on 3/17/95
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_err_codes.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "stop_dst.h"  

static integer4 stop_blen; 
static integer4 stop_maxlen;
static integer4 stop_id=STOP_BANKID;
static integer4 stop_version=STOP_BANKVERSION;
static integer1 *stop_bank=NULL;

static void stop_bank_init()
{
  stop_maxlen=sizeof(stop_id)+sizeof(stop_version);
  stop_bank=(integer1 *) calloc(stop_maxlen, sizeof(integer1) );
  if (stop_bank==NULL)
    {
      fprintf (stderr,"stop_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    }
}

integer4 stop_to_dst_(integer4 *unit)
{
  integer4 rcode;

  if (stop_bank==NULL) stop_bank_init() ;

  rcode = dst_initbank_ ( &stop_id,&stop_version, &stop_blen, &stop_maxlen, stop_bank) ;
  if (rcode)
    {
      fprintf (stderr,"stop_to_dst_ ERROR dst_initbank: %d\n", rcode);
      exit(0);			 	
    }             
 
  rcode = dst_write_bank_(unit, &stop_blen, stop_bank );
  if (rcode)
    {
      fprintf (stderr,"stop_to_dst_ ERROR dst_write_bank_ : %d\n",  rcode);
      exit(0);			 	
    }
  return SUCCESS;
}	
