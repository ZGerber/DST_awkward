/*
 * hctim_dst.c 
 *
 * $Source: /hires_soft/uvm2k/bank/hctim_dst.c,v $
 * $Log: hctim_dst.c,v $
 * Revision 1.1  1999/06/28 21:06:14  tareq
 * Initial revision
 *
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
#include "hctim_dst.h"  

#ifndef DEGRAD
#define DEGRAD 57.2958
#endif

hctim_dst_common hctim_;  /* allocate memory to hctim_common */

static integer4 hctim_blen = 0; 
static integer4 hctim_maxlen = sizeof(integer4) * 2 + sizeof(hctim_dst_common);
static integer1 *hctim_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* hctim_bank_buffer_ (integer4* hctim_bank_buffer_size)
{
  (*hctim_bank_buffer_size) = hctim_blen;
  return hctim_bank;
}



static void
hctim_bank_init(void) {

  hctim_bank = (integer1 *)calloc(hctim_maxlen, sizeof(integer1));

  if(hctim_bank==NULL) {
    fprintf(stderr, "hctim_bank_init: fail to assign memory to bank. Abort.\n");
    exit(0);
  }
}

integer4
hctim_common_to_bank_(void) {
	
  static integer4 id = HCTIM_BANKID, ver = HCTIM_BANKVERSION;
  integer4 rcode, nobj;
  integer4 i, nmir, ntube, timinfo=0;

  if (hctim_bank == NULL) hctim_bank_init();

  /* Initialize hctim_blen, and pack the id and version to bank */
  
  if((rcode = dst_initbank_(&id, &ver, &hctim_blen, &hctim_maxlen, hctim_bank)))
    return rcode;
  
  for(i=0; i<HCTIM_MAXFIT; i++) {
    timinfo = timinfo << 1;
    if ( hctim_.timinfo[i] == HCTIM_TIMINFO_USED )  timinfo++;
  }
  if ((rcode = dst_packi4asi2_( &timinfo, (nobj=1, &nobj), hctim_bank, 
				&hctim_blen, &hctim_maxlen))) return rcode; 

  for ( i=0; i<HCTIM_MAXFIT; i++ ) {

    if ( hctim_.timinfo[i] == HCTIM_TIMINFO_UNUSED ) continue;

    if ((rcode = dst_packi4_(&hctim_.jday[i], (nobj=1, &nobj), hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packi4_(&hctim_.jsec[i], (nobj=1, &nobj), hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packi4_(&hctim_.msec[i], (nobj=1, &nobj), hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packi4_(&hctim_.failmode[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode; 

    if (hctim_.failmode[i] != SUCCESS) continue;

    if ((rcode = dst_packr8_(&hctim_.mchi2[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.rchi2[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.lchi2[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(&hctim_.mrp[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.rrp[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.lrp[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(&hctim_.mpsi[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.rpsi[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.lpsi[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(&hctim_.mthe[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.rthe[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.lthe[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(&hctim_.mphi[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.rphi[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(&hctim_.lphi[i], (nobj=1, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(hctim_.mtkv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.rtkv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.ltkv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(hctim_.mrpv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.rrpv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.lrpv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(hctim_.mrpuv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.rrpuv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.lrpuv[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(hctim_.mshwn[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.rshwn[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.lshwn[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_packr8_(hctim_.mcore[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.rcore[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.lcore[i], (nobj=3, &nobj), hctim_bank, 
			     &hctim_blen, &hctim_maxlen))) return rcode;

    nmir = hctim_.nmir[i];
    if ((rcode = dst_packi4asi2_(&nmir, (nobj=1, &nobj), hctim_bank, 
				 &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_(hctim_.mir[i], (nobj=nmir, &nobj), hctim_bank, 
				 &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_(hctim_.mirntube[i], (nobj=nmir, &nobj),
				 hctim_bank, &hctim_blen, &hctim_maxlen))) return rcode; 

    ntube = hctim_.ntube[i];

    if ((rcode = dst_packi4asi2_(&ntube, (nobj=1, &nobj), hctim_bank, 
				 &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_(hctim_.tube[i], (nobj=ntube, &nobj),
				 hctim_bank, &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_(hctim_.tubemir[i], (nobj=ntube, &nobj),
				 hctim_bank, &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_(hctim_.ig[i], (nobj=ntube, &nobj), hctim_bank, 
				 &hctim_blen, &hctim_maxlen))) return rcode; 

    if ((rcode = dst_packr8_(hctim_.time[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.timefit[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.thetb[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.sgmt[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.asx[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.asy[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_packr8_(hctim_.asz[i], &nobj, hctim_bank,
			     &hctim_blen, &hctim_maxlen))) return rcode;
  }
  return SUCCESS;
}


integer4
hctim_bank_to_dst_(integer4 *NumUnit) {
	
  return dst_write_bank_(NumUnit, &hctim_blen, hctim_bank );
}

integer4
hctim_common_to_dst_(integer4 *NumUnit) {

  integer4 rcode;
  
  if((rcode = hctim_common_to_bank_())) {
    fprintf (stderr,"hctim_common_to_bank_ ERROR : %ld\n", (long) rcode);
    exit(0);			 	
  }             
  
  if((rcode = hctim_bank_to_dst_(NumUnit))) {
    fprintf (stderr,"hctim_bank_to_dst_ ERROR : %ld\n", (long) rcode);
    exit(0);
  }
  
  return SUCCESS;
}

integer4
hctim_bank_to_common_(integer1 *bank) {

  integer4 rcode = 0;
  integer4 nobj;
  integer4 i, nmir, ntube, timinfo;

  hctim_blen = 2 * sizeof(integer4); /* skip id and version  */

  if ((rcode = dst_unpacki2asi4_(&timinfo, (nobj=1, &nobj), bank,
				 &hctim_blen, &hctim_maxlen))) return rcode; 

  for(i=0; i<HCTIM_MAXFIT; i++) {
    if ( timinfo & 0x8000 )  hctim_.timinfo[i] = HCTIM_TIMINFO_USED;
    else                     hctim_.timinfo[i] = HCTIM_TIMINFO_UNUSED;
    timinfo = timinfo << 1;
  }

  for ( i=0; i<HCTIM_MAXFIT; i++ ) {

    if ( hctim_.timinfo[i] == HCTIM_TIMINFO_UNUSED ) continue;

    if ((rcode = dst_unpacki4_(&hctim_.jday[i], (nobj=1, &nobj), bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpacki4_(&hctim_.jsec[i], (nobj=1, &nobj), bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpacki4_(&hctim_.msec[i], (nobj=1, &nobj), bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpacki4_(&hctim_.failmode[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode; 

    if (hctim_.failmode[i] != SUCCESS) continue;

    if ((rcode = dst_unpackr8_(&hctim_.mchi2[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.rchi2[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.lchi2[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(&hctim_.mrp[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.rrp[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.lrp[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(&hctim_.mpsi[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.rpsi[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.lpsi[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(&hctim_.mthe[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.rthe[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.lthe[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(&hctim_.mphi[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.rphi[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(&hctim_.lphi[i], (nobj=1, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(hctim_.mtkv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.rtkv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.ltkv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(hctim_.mrpv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.rrpv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.lrpv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(hctim_.mrpuv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.rrpuv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.lrpuv[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(hctim_.mshwn[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.rshwn[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.lshwn[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpackr8_(hctim_.mcore[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.rcore[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.lcore[i], (nobj=3, &nobj), bank, 
			       &hctim_blen, &hctim_maxlen))) return rcode;

    if ((rcode = dst_unpacki2asi4_(&nmir, (nobj=1, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 
    hctim_.nmir[i] = nmir;

    if ((rcode = dst_unpacki2asi4_(hctim_.mir[i], (nobj=nmir, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_unpacki2asi4_(hctim_.mirntube[i], (nobj=nmir, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 

    if ((rcode = dst_unpacki2asi4_(&ntube, (nobj=1, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 
    hctim_.ntube[i] = ntube;

    if ((rcode = dst_unpacki2asi4_(hctim_.tube[i], (nobj=ntube, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_unpacki2asi4_(hctim_.tubemir[i], (nobj=ntube, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 
    if ((rcode = dst_unpacki2asi4_(hctim_.ig[i], (nobj=ntube, &nobj), bank, 
				   &hctim_blen, &hctim_maxlen))) return rcode; 

    if ((rcode = dst_unpackr8_(hctim_.time[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.timefit[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.thetb[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.sgmt[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.asx[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.asy[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
    if ((rcode = dst_unpackr8_(hctim_.asz[i], &nobj, bank,
			       &hctim_blen, &hctim_maxlen))) return rcode;
  }

  return SUCCESS;
}

integer4
hctim_common_to_dump_(integer4 *long_output) {

  return hctim_common_to_dumpf_(stdout, long_output);
}

integer4
hctim_common_to_dumpf_(FILE* fp, integer4 *long_output) {

  integer4 i, j, k;
  static struct {
    integer4  code;
    integer1 *mess;
  } trans2[] = {
    { HCTIM_FIT_NOT_REQUESTED,       "Fit not requested"                     },
    { HCTIM_NOT_IMPLEMENTED,         "Fit not implemented"                   },
    { HCTIM_REQUIRED_BANKS_MISSING,  
      "Bank(s) required for fit are missing or have failed"                  },
    { HCTIM_MISSING_TRAJECTORY_INFO,
      "Bank(s) required for desired trajectory source are missing/failed"    },
    { HCTIM_UPWARD_GOING_TRACK,        "Upward going track"                  },
    { HCTIM_TOO_FEW_GOOD_TUBES,      "Too few good tubes"                    },
    { HCTIM_FITTER_FAILURE,          "Fitter failed"                         },    
    { HCTIM_INSANE_TRAJECTORY,
      "Trajectory (direction and/or core) unreasonable"                      },
    { 999,                          "Unknown failmode"                       } /* marks end of list */ 
  };

  fprintf(fp,"\nHCTIM bank. tubes: ");
  for ( i=0; i<HCTIM_MAXFIT; i++ ) {
    if ( hctim_.timinfo[i] == HCTIM_TIMINFO_USED )
      fprintf(fp," %03d", hctim_.ntube[i]);
    else
      fprintf(fp," -- ");
  }
  fprintf(fp,"\n");

  for(i=0; i<HCTIM_MAXFIT; i++ ) {
    if ( hctim_.timinfo[i] == HCTIM_TIMINFO_UNUSED ) continue;

    fprintf(fp,"\n    -> Fit %2d jDay/Sec: %d/%5.5d %02d:%02d:%02d.%03d\n",
	    i+1, hctim_.jday[i], hctim_.jsec[i], hctim_.msec[i] / 3600000,
            (hctim_.msec[i] / 60000) % 60, (hctim_.msec[i] / 1000) % 60,
	    hctim_.msec[i] % 1000);

    if ( hctim_.failmode[i] != SUCCESS ) {
      for ( k=0; trans2[k].code!=999 ; k++ )
        if ( hctim_.failmode[i] == trans2[k].code ) break;
      fprintf(fp,"    %s\n", trans2[k].mess );
      continue;    /* Nothing else to show for this fit */
    }

    fprintf(fp,"chi2 :%10.2f   range= [%10.2f, %10.2f]\n",
            hctim_.mchi2[i], hctim_.rchi2[i], hctim_.lchi2[i]);
    fprintf(fp,"rp   :%10.2f   range= [%10.2f, %10.2f]  meters\n",
            hctim_.mrp[i], hctim_.rrp[i], hctim_.lrp[i]);
    fprintf(fp,"psi  :%10.2f   range= [%10.2f, %10.2f]  degrees\n",
	    hctim_.mpsi[i]*DEGRAD, hctim_.rpsi[i]*DEGRAD, hctim_.lpsi[i]*DEGRAD);
    fprintf(fp,"theta:%10.2f   range= [%10.2f, %10.2f]  degrees\n",
	    hctim_.mthe[i]*DEGRAD, hctim_.rthe[i]*DEGRAD, hctim_.lthe[i]*DEGRAD);
    fprintf(fp,"phi  :%10.2f   range= [%10.2f, %10.2f]  degrees\n",
	    hctim_.mphi[i]*DEGRAD, hctim_.rphi[i]*DEGRAD, hctim_.lphi[i]*DEGRAD);
    fprintf(fp,"\n");

    fprintf(fp,"core location x: %10.2f,  range = [ %10.2f, %10.2f]  meters\n",
	    hctim_.mcore[i][0], hctim_.rcore[i][0], hctim_.lcore[i][0] );
    fprintf(fp,"core location y: %10.2f,  range = [ %10.2f, %10.2f]  meters\n",
	    hctim_.mcore[i][1], hctim_.rcore[i][1], hctim_.lcore[i][1] );
    fprintf(fp,"\n");
    fprintf(fp, "shower direction: ( %7.4f, %7.4f, %7.4f )\n",
	    hctim_.mtkv[i][0], hctim_.mtkv[i][1], hctim_.mtkv[i][2] );
    fprintf(fp, "      dir bound1: ( %7.4f, %7.4f, %7.4f )\n",
	    hctim_.rtkv[i][0], hctim_.rtkv[i][1], hctim_.rtkv[i][2] );
    fprintf(fp, "      dir bound2: ( %7.4f, %7.4f, %7.4f )\n",
	    hctim_.ltkv[i][0], hctim_.ltkv[i][1], hctim_.ltkv[i][2] );

  }
  if( *long_output == 1 ) {
    for(i=0; i<HCTIM_MAXFIT; i++ ) {
      if( hctim_.timinfo[i] == HCTIM_TIMINFO_UNUSED ) continue;
      if( hctim_.failmode[i] != SUCCESS ) continue;

      fprintf(fp,"\n    -> Fit %2d Tubes\n", i+1);

      fprintf(fp," mir tube     time (ns)  timefit    sgmt view_ang     asx     asy     asz   ig\n");

      for(j=0; j<hctim_.ntube[i]; j++) {
        fprintf(fp,"  %2d  %03d %10.1lf %10.1lf %8.1lf  %7.2lf %7.4lf %7.4lf %7.4lf %3d\n", 
		hctim_.tubemir[i][j], hctim_.tube[i][j], 
		hctim_.time[i][j], hctim_.timefit[i][j], hctim_.sgmt[i][j], 
		hctim_.thetb[i][j]*DEGRAD, 
		hctim_.asx[i][j], hctim_.asy[i][j], hctim_.asz[i][j],
		hctim_.ig[i][j] );
      }
    }
  }
  return SUCCESS;
}
