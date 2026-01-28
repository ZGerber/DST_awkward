/*
 * hcbin_dst.c 
 *
 * $Source: /hires_soft/uvm2k/bank/hcbin_dst.c,v $
 * $Log: hcbin_dst.c,v $
 * Revision 1.1  1999/06/28 21:05:17  tareq
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
#include "hcbin_dst.h"  

hcbin_dst_common hcbin_;  /* allocate memory to hcbin_common */

static integer4 hcbin_blen = 0; 
static integer4 hcbin_maxlen = sizeof(integer4) * 2 + sizeof(hcbin_dst_common);
static integer1 *hcbin_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* hcbin_bank_buffer_ (integer4* hcbin_bank_buffer_size)
{
  (*hcbin_bank_buffer_size) = hcbin_blen;
  return hcbin_bank;
}



static void hcbin_bank_init(void) {

  hcbin_bank = (integer1 *)calloc(hcbin_maxlen, sizeof(integer1));
  if(hcbin_bank==NULL) {
    fprintf(stderr,"hcbin_bank_init: fail to assign memory to bank. Abort.\n");
    exit(0);
  }
}

integer4 hcbin_common_to_bank_(void) {
	
  static integer4 id = HCBIN_BANKID, ver = HCBIN_BANKVERSION;
  integer4 rcode, nobj;
  integer4 i, nbin, bininfo=0;

  if (hcbin_bank == NULL) hcbin_bank_init();

  /* Initialize hcbin_blen, and pack the id and version to bank */
  
  if ((rcode = dst_initbank_(&id, &ver, &hcbin_blen, &hcbin_maxlen, hcbin_bank)))
    return rcode;
  
  for(i=0; i<HCBIN_MAXFIT; i++) {
    bininfo = bininfo << 1;
    if ( hcbin_.bininfo[i] == HCBIN_BININFO_USED )  bininfo++;
  }
  if ((rcode = dst_packi4asi2_( &bininfo, (nobj=1, &nobj), hcbin_bank, 
				&hcbin_blen, &hcbin_maxlen))) return rcode; 

  for ( i=0; i<HCBIN_MAXFIT; i++ ) {
    if ( hcbin_.bininfo[i] == HCBIN_BININFO_UNUSED ) continue;
    
    if ((rcode = dst_packi4_(&hcbin_.jday[i], (nobj=1, &nobj), hcbin_bank,
                          &hcbin_blen, &hcbin_maxlen))) return rcode;
    if ((rcode = dst_packi4_(&hcbin_.jsec[i], (nobj=1, &nobj), hcbin_bank,
                          &hcbin_blen, &hcbin_maxlen))) return rcode;
    if ((rcode = dst_packi4_(&hcbin_.msec[i], (nobj=1, &nobj), hcbin_bank,
                          &hcbin_blen, &hcbin_maxlen))) return rcode;

    if ((rcode = dst_packi4_(&hcbin_.failmode[i], (nobj=1, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if (hcbin_.failmode[i] != SUCCESS) continue;

    if ( hcbin_.nbin[i] < 0 )  
      nbin = 0;
    else if ( hcbin_.nbin[i] > HCBIN_MAXBIN ) 
      nbin = HCBIN_MAXBIN;
    else 
      nbin = hcbin_.nbin[i];
    
    if ( hcbin_.nbin[i] < 0 || hcbin_.nbin[i] > HCBIN_MAXBIN ) {
      fprintf(stderr,
         "%s Number of bins out of range (%d) for fit %d; only packing (%d)\n", 
         "hcbin_common_to_bank_:", hcbin_.nbin[i], i+1, nbin);
    }
    if ((rcode = dst_packi4asi2_(&nbin, (nobj=1, &nobj), hcbin_bank, 
                                &hcbin_blen, &hcbin_maxlen))) return rcode; 

    if ((rcode = dst_packr8_(hcbin_.bvx[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(hcbin_.bvy[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(hcbin_.bvz[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(hcbin_.bsz[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(hcbin_.sig[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(hcbin_.sigerr[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(hcbin_.cfc[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_packi4_(hcbin_.ig[i], (nobj=nbin, &nobj), hcbin_bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
  }
  return SUCCESS;
}


integer4 hcbin_bank_to_dst_(integer4 *NumUnit) {	

  return dst_write_bank_(NumUnit, &hcbin_blen, hcbin_bank );
}

integer4 hcbin_common_to_dst_(integer4 *NumUnit) {

  integer4 rcode;
  if ((rcode = hcbin_common_to_bank_())) {
    fprintf (stderr,"hcbin_common_to_bank_ ERROR : %ld\n", (long) rcode);
    exit(0);			 	
  }             
  if ((rcode = hcbin_bank_to_dst_(NumUnit))) {
    fprintf (stderr,"hcbin_bank_to_dst_ ERROR : %ld\n", (long) rcode);
    exit(0);			 	
  }
  return SUCCESS;
}

integer4 hcbin_bank_to_common_(integer1 *bank) {

  integer4 rcode = 0;
  integer4 i, nobj;
  integer4 nbin, bininfo;


  hcbin_blen = 2 * sizeof(integer4); /* skip id and version  */

  if ((rcode = dst_unpacki2asi4_(&bininfo, (nobj=1, &nobj), bank,
                                &hcbin_blen, &hcbin_maxlen))) return rcode; 

  for(i=0; i<HCBIN_MAXFIT; i++) {
    if ( bininfo & 0x8000 )  hcbin_.bininfo[i] = HCBIN_BININFO_USED;
    else                     hcbin_.bininfo[i] = HCBIN_BININFO_UNUSED;
    bininfo = bininfo << 1;
  }

  for ( i=0; i<HCBIN_MAXFIT; i++ ) {
    if ( hcbin_.bininfo[i] == HCBIN_BININFO_UNUSED ) continue;

    if ((rcode = dst_unpacki4_(&hcbin_.jday[i], (nobj=1, &nobj), bank,
                          &hcbin_blen, &hcbin_maxlen))) return rcode;
    if ((rcode = dst_unpacki4_(&hcbin_.jsec[i], (nobj=1, &nobj), bank,
                          &hcbin_blen, &hcbin_maxlen))) return rcode;
    if ((rcode = dst_unpacki4_(&hcbin_.msec[i], (nobj=1, &nobj), bank,
                          &hcbin_blen, &hcbin_maxlen))) return rcode;

    if ((rcode = dst_unpacki4_(&hcbin_.failmode[i], (nobj=1, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if (hcbin_.failmode[i] != SUCCESS) continue;

    if ((rcode = dst_unpacki2asi4_(&nbin, (nobj=1, &nobj), bank, 
                                &hcbin_blen, &hcbin_maxlen))) return rcode; 
    hcbin_.nbin[i] = nbin;

    if ((rcode = dst_unpackr8_(hcbin_.bvx[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(hcbin_.bvy[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(hcbin_.bvz[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(hcbin_.bsz[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(hcbin_.sig[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(hcbin_.sigerr[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(hcbin_.cfc[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
    if ((rcode = dst_unpacki4_(hcbin_.ig[i], (nobj=nbin, &nobj), bank, 
                            &hcbin_blen, &hcbin_maxlen))) return rcode; 
  }
  return SUCCESS;
}

integer4 hcbin_common_to_dump_(integer4 *long_output) {

  return hcbin_common_to_dumpf_(stdout, long_output);
}

integer4 hcbin_common_to_dumpf_(FILE* fp, integer4 *long_output) {

  integer4 i, j, k;

  static struct {
    integer4  code;
    integer1 *mess;
  } trans1[] = {
    { HCBIN_IG_CHERENKOV_CUT, "cherenkov cut"   },
    { HCBIN_IG_SICKPLNFIT,    "sick plane fit"  },
    { HCBIN_IG_OVERCORRECTED, "too much corr"   },
    { HCBIN_IG_GOODBIN,       ""                },   /* good bin */
    { 999,                    "unknown ig code" }    /* marks end of list */ 
  };

  static struct {
    integer4  code;
    integer1 *mess;
  } trans2[] = {
    { HCBIN_FIT_NOT_REQUESTED,       "Fit not requested"                  },
    { HCBIN_NOT_IMPLEMENTED,         "Fit not implemented"                },
    { HCBIN_REQUIRED_BANKS_MISSING,
      "Bank(s) required for fit are missing or have failed"               },
    { HCBIN_MISSING_TRAJECTORY_INFO,
      "Bank(s) required for desired trajectory source are missing/failed" },
    { HCBIN_UPWARD_GOING_TRACK,      "Upward going track"                 },
    { HCBIN_TOO_FEW_GOOD_BINS,       "Too few good bins"                  },
    { HCBIN_FITTER_FAILURE,          "Fitter failed"                      },    
    {  HCBIN_INSANE_TRAJECTORY,
       "Trajectory (direction and/or core) unreasonable"                  },
    {  999,                          "Unknown failmode"                   } /* marks end of list */ 
  };
  (void)(long_output);
  fprintf(fp,"\nHCBIN bank. bins: ");
  for ( i=0; i<HCBIN_MAXFIT; i++ ) {
    if ( hcbin_.bininfo[i] == HCBIN_BININFO_USED )
      fprintf(fp," %03d", hcbin_.nbin[i]);
    else
      fprintf(fp," -- ");
  }
  fprintf(fp,"\n\n");


/* if ( *long_output == 1 ) {} ... */

  for(i=0; i<HCBIN_MAXFIT; i++ ) {
    if ( hcbin_.bininfo[i] == HCBIN_BININFO_UNUSED ) continue;

    fprintf(fp,"    -> Fit %1d jDay/Sec: %d/%5.5d %02d:%02d:%02d.%03d\n", i+1,
             hcbin_.jday[i], hcbin_.jsec[i], hcbin_.msec[i] / 3600000,
            (hcbin_.msec[i] / 60000) % 60, (hcbin_.msec[i] / 1000) % 60,
             hcbin_.msec[i] % 1000);

    if ( hcbin_.failmode[i] != SUCCESS ) {
      for ( k=0; trans2[k].code!=999 ; k++ )
        if ( hcbin_.failmode[i] == trans2[k].code ) break;
      fprintf(fp,"    %s\n", trans2[k].mess );
      continue;    /* Nothing else to show for this fit */
    }

    fprintf(fp,"             bin direction     bin size  signal   signal   cfc\n");
    fprintf(fp,"  bin      nx      ny      nz    (deg) pe/deg/m^2  error   m^2 ig\n");
    for(j=0; j<hcbin_.nbin[i]; j++) {
      for ( k=0; trans1[k].code!=999 ; k++ ) {
        if ( hcbin_.ig[i][j] == trans1[k].code ) break;
      }
      fprintf(fp, " %4d %8.5f %8.5f %8.5f %4.2f %8.1f %8.1f %6.2f %2d  %s\n",
              j, hcbin_.bvx[i][j], hcbin_.bvy[i][j], hcbin_.bvz[i][j],
              hcbin_.bsz[i][j], hcbin_.sig[i][j], hcbin_.sigerr[i][j], hcbin_.cfc[i][j],
              hcbin_.ig[i][j],  trans1[k].mess);

    }
    fprintf(fp,"\n");
  }
  return SUCCESS;
}
