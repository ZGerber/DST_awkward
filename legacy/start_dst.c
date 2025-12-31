/*
 * $Source: /hires_soft/uvm2k/bank/start_dst.c,v $
 * $Log: start_dst.c,v $
 * Revision 1.3  1995/07/25 10:27:38  mjk
 * Change dst_initbank to dst_initbank_
 * Use SUCCESS (from dst_err_codes.h) as return
 *
 * Revision 1.2  1995/03/18  00:35:21  jeremy
 * *** empty log message ***
 *
 * modified by jT at 6:57 PM on 3/10/95
 * modified by jds at 11:06 AM on 3/17/95
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
#include "start_dst.h"  

static integer4 start_blen; 
static integer4 start_maxlen;
static integer4 start_id=START_BANKID;
static integer4 start_version=START_BANKVERSION;
static integer1 *start_bank=NULL;


static void start_bank_init()
{
  start_maxlen=sizeof(start_id)+sizeof(start_version);
  start_bank=(integer1 *) calloc(start_maxlen, sizeof(integer1) );
  if (start_bank == NULL)
    {
      fprintf (stderr,"start_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    }
}

integer4 start_to_dst_(integer4 *unit)
{
  integer4 rcode;

  if (start_bank==NULL) start_bank_init() ;

  rcode = dst_initbank_ ( &start_id, &start_version, &start_blen, &start_maxlen, start_bank) ;
  if (rcode)
    {
      fprintf (stderr,"start_to_dst_ ERROR dst_initbank: %d\n", rcode);
      exit(0);			 	
    }             
 
  rcode = dst_write_bank_(unit, &start_blen, start_bank );
  if (rcode)
    {
      fprintf (stderr,"start_to_dst_ ERROR dst_write_bank_ : %d\n",  rcode);
      exit(0);			 	
    }
  return SUCCESS;
}	
