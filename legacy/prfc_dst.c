/*
 * prfc_dst.c 
 *
 * bank PRFC Contains profile information from the group profile
 * program, pfl. This is Dai's original pfgh fitting program
 * after substantial additions by MJK.
 *
 * The PRFC bank is strict superset of the PRFA bank and is
 * intended as its replacement. (N.B.: PRFB was already claimed
 * by jTang)
 *
 * All hires eyes are handled in one bank.
 *
 * It is expected that alternative profiling banks will be named
 * prfd, prfe, ...
 *
 * $Source: /hires_soft/uvm2k/bank/prfc_dst.c,v $
 * $Log: prfc_dst.c,v $
 * Revision 1.3  1997/02/19 08:48:23  mjk
 * Fixed a bug in prfc_common_to_dumpf_() which caused whether or
 * not bin information was dumped out to be dependent on pflinfo[]
 * rather than the proper bininfo[].
 *
 * Revision 1.2  1996/07/02  22:30:33  mjk
 * Added PRFC_INSANE_TRAJECTORY failmode message.
 *
 * Revision 1.1  1996/05/03  22:27:34  mjk
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
#include "prfc_dst.h"  

integer4 eventNameFromId(integer4 bank_id, integer1 *name, integer4 len);


prfc_dst_common prfc_;  /* allocate memory to prfc_common */

static integer4 prfc_blen = 0; 
static integer4 prfc_maxlen = sizeof(integer4) * 2 + sizeof(prfc_dst_common);
static integer1 *prfc_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* prfc_bank_buffer_ (integer4* prfc_bank_buffer_size)
{
  (*prfc_bank_buffer_size) = prfc_blen;
  return prfc_bank;
}



static void prfc_bank_init(void)
{
  prfc_bank = (integer1 *)calloc(prfc_maxlen, sizeof(integer1));
  if (prfc_bank==NULL)
    {
      fprintf(stderr, 
	      "prfc_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    }
}

integer4 prfc_common_to_bank_(void)
{	
  static integer4 id = PRFC_BANKID, ver = PRFC_BANKVERSION;
  integer4 rcode;
  integer4 i, nobj;
  integer4 nbin, pflinfo=0, bininfo=0, mtxinfo=0;


  if (prfc_bank == NULL) prfc_bank_init();

  /* Initialize prfc_blen, and pack the id and version to bank */
  if ((rcode = dst_initbank_(&id, &ver, &prfc_blen, &prfc_maxlen, prfc_bank)))
    return rcode;


  /* Pack pflinfo[], bininfo[], and mtxinfo[] tightly */

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    pflinfo = pflinfo << 1;
    bininfo = bininfo << 1;
    mtxinfo = mtxinfo << 1;
    if ( prfc_.pflinfo[i] == PRFC_PFLINFO_USED )  pflinfo++;
    if ( prfc_.bininfo[i] == PRFC_BININFO_USED )  bininfo++;
    if ( prfc_.mtxinfo[i] == PRFC_MTXINFO_USED )  mtxinfo++;
  }
  
  if ((rcode = dst_packi4asi2_( &pflinfo, (nobj=1, &nobj), prfc_bank, 
				&prfc_blen, &prfc_maxlen))) return rcode; 
  if ((rcode = dst_packi4asi2_( &bininfo, (nobj=1, &nobj), prfc_bank, 
				&prfc_blen, &prfc_maxlen))) return rcode; 
  if ((rcode = dst_packi4asi2_( &mtxinfo, (nobj=1, &nobj), prfc_bank, 
				&prfc_blen, &prfc_maxlen))) return rcode; 


  /* First pack the profile parameters and their errors */

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.pflinfo[i] == PRFC_PFLINFO_UNUSED ) continue;

    if ((rcode = dst_packi4_(&prfc_.failmode[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if (prfc_.failmode[i] != SUCCESS) continue;
    
    
    /* We only pack the profile parameters if the fit worked */
    
    if ((rcode = dst_packr8_(&prfc_.szmx [i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.dszmx[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.rszmx[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.lszmx[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.tszmx[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_packr8_(&prfc_.xm [i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.dxm[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.rxm[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.lxm[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.txm[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_packr8_(&prfc_.x0 [i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.dx0[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.rx0[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.lx0[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.tx0[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_packr8_(&prfc_.lambda [i], (nobj=1, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.dlambda[i], (nobj=1, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.rlambda[i], (nobj=1, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.llambda[i], (nobj=1, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.tlambda[i], (nobj=1, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_packr8_(&prfc_.eng [i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.deng[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.reng[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.leng[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.teng[i], (nobj=1, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    /* Pack remaining stuff */
    if ((rcode = dst_packi4_(&prfc_.traj_source[i], (nobj=1,&nobj), prfc_bank,
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packi4_(&prfc_.errstat[i], (nobj=1, &nobj), prfc_bank,
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packi4_(&prfc_.ndf[i], (nobj=1, &nobj), prfc_bank,
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(&prfc_.chi2[i], (nobj=1, &nobj), prfc_bank,
			     &prfc_blen, &prfc_maxlen))) return rcode; 
  }


  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.bininfo[i] == PRFC_BININFO_UNUSED ) continue;

    /* Pack number of bins - check that nbin[] are within range */
    
    if ( prfc_.nbin[i] < 0 )  
      nbin = 0;
    else if ( prfc_.nbin[i] > PRFC_MAXBIN ) 
      nbin = PRFC_MAXBIN;
    else 
      nbin = prfc_.nbin[i];
    
    if ( prfc_.nbin[i] < 0 || prfc_.nbin[i] > PRFC_MAXBIN ) {
      fprintf(stderr,
	      "%s Number of bins out of range (%d) for eye %d; only packing (%d)\n", 
	      "prfc_common_to_bank_:", prfc_.nbin[i], i+1, nbin);
    }
    if ((rcode = dst_packi4asi2_(&nbin, (nobj=1, &nobj), prfc_bank, 
				 &prfc_blen, &prfc_maxlen))) return rcode; 
    
    
    /* Pack information about bins along the show trajectory */
    
    if ((rcode = dst_packr8_(prfc_.dep[i], (nobj=nbin, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(prfc_.gm[i], (nobj=nbin, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    
    
    /* Pack information about light and MC match along show trajectory */
    
    if ((rcode = dst_packr8_(prfc_.scin[i], (nobj=nbin, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(prfc_.rayl[i], (nobj=nbin, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(prfc_.aero[i], (nobj=nbin, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(prfc_.crnk[i], (nobj=nbin, &nobj), prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(prfc_.sigmc[i], (nobj=nbin, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_(prfc_.sig[i], (nobj=nbin, &nobj),prfc_bank, 
			     &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_(prfc_.ig[i], (nobj=nbin, &nobj),prfc_bank,
				 &prfc_blen, &prfc_maxlen))) return rcode; 
  }
  
  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.mtxinfo[i] == PRFC_MTXINFO_UNUSED ) continue;
    
    if ((rcode = dst_packi4asi2_( &prfc_.nel[i], (nobj=1, &nobj), prfc_bank, 
				  &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packi4asi2_( &prfc_.mor[i], (nobj=1, &nobj), prfc_bank, 
				  &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_packr8_( prfc_.mxel[i], (nobj=prfc_.nel[i], &nobj),
			      prfc_bank, &prfc_blen, &prfc_maxlen))) return rcode;
  }

  return SUCCESS;
}


integer4 prfc_bank_to_dst_(integer4 *NumUnit)
{	
  return dst_write_bank_(NumUnit, &prfc_blen, prfc_bank );
}

integer4 prfc_common_to_dst_(integer4 *NumUnit)
{
  integer4 rcode;
  if ((rcode = prfc_common_to_bank_()))
    {
      fprintf (stderr,"prfc_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit(0);			 	
    }             
  if ((rcode = prfc_bank_to_dst_(NumUnit)))
    {
      fprintf (stderr,"prfc_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit(0);			 	
    }
  return SUCCESS;
}

integer4 prfc_bank_to_common_(integer1 *bank)
{
  integer4 rcode = 0;
  integer4 i, nobj;
  integer4 nbin, pflinfo, bininfo, mtxinfo;

  
  prfc_blen = 2 * sizeof(integer4); /* skip id and version  */


  /* Unpack pflinfo[], bininfo[], and mtxinfo[] */

  if ((rcode = dst_unpacki2asi4_(&pflinfo, (nobj=1, &nobj), bank, 
				 &prfc_blen, &prfc_maxlen))) return rcode; 
  if ((rcode = dst_unpacki2asi4_(&bininfo, (nobj=1, &nobj), bank,
				 &prfc_blen, &prfc_maxlen))) return rcode; 
  if ((rcode = dst_unpacki2asi4_(&mtxinfo, (nobj=1, &nobj), bank,
				 &prfc_blen, &prfc_maxlen))) return rcode; 

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( pflinfo & 0x8000 )  prfc_.pflinfo[i] = PRFC_PFLINFO_USED;
    else                     prfc_.pflinfo[i] = PRFC_PFLINFO_UNUSED;
    if ( bininfo & 0x8000 )  prfc_.bininfo[i] = PRFC_BININFO_USED;
    else                     prfc_.bininfo[i] = PRFC_BININFO_UNUSED;
    if ( mtxinfo & 0x8000 )  prfc_.mtxinfo[i] = PRFC_MTXINFO_USED;
    else                     prfc_.mtxinfo[i] = PRFC_MTXINFO_UNUSED;
    pflinfo = pflinfo << 1;
    bininfo = bininfo << 1;
    mtxinfo = mtxinfo << 1;
  }


  /* First unpack the profile parameters and their errors */

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.pflinfo[i] == PRFC_PFLINFO_UNUSED ) continue;
      
    if ((rcode = dst_unpacki4_(&prfc_.failmode[i], (nobj=1,&nobj), bank,
			       &prfc_blen, &prfc_maxlen))) return rcode; 

    if (prfc_.failmode[i] != SUCCESS) continue;
    
    
    /* We only unpack the profile parameters if the fit worked */
    
    if ((rcode = dst_unpackr8_(&prfc_.szmx [i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.dszmx[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.rszmx[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.lszmx[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.tszmx[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_unpackr8_(&prfc_.xm [i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.dxm[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.rxm[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.lxm[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.txm[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_unpackr8_(&prfc_.x0 [i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.dx0[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.rx0[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.lx0[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.tx0[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_unpackr8_(&prfc_.lambda [i], (nobj=1, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.dlambda[i], (nobj=1, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.rlambda[i], (nobj=1, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.llambda[i], (nobj=1, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.tlambda[i], (nobj=1, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    
    if ((rcode = dst_unpackr8_(&prfc_.eng [i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.deng[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.reng[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.leng[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.teng[i], (nobj=1, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    
    /* Unpack remaining stuff */
    if ((rcode = dst_unpacki4_(&prfc_.traj_source[i], (nobj=1,&nobj), bank,
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpacki4_(&prfc_.errstat[i], (nobj=1, &nobj), bank,
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpacki4_(&prfc_.ndf[i], (nobj=1, &nobj), bank,
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(&prfc_.chi2[i], (nobj=1, &nobj), bank,
			       &prfc_blen, &prfc_maxlen))) return rcode; 
  }
  

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.bininfo[i] == PRFC_BININFO_UNUSED ) continue;

    /* Unpack number of bins */
    
    if ((rcode = dst_unpacki2asi4_(&nbin, (nobj=1, &nobj), bank, 
				   &prfc_blen, &prfc_maxlen))) return rcode; 
    prfc_.nbin[i] = nbin;
    

    /* Unpack information about bins along the shower trajectory */
    if ((rcode = dst_unpackr8_(prfc_.dep[i], (nobj=nbin, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(prfc_.gm[i], (nobj=nbin, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    
    
    /* Unpack information about light and MC match along shower trajectory */
    
    if ((rcode = dst_unpackr8_(prfc_.scin[i], (nobj=nbin, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(prfc_.rayl[i], (nobj=nbin, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(prfc_.aero[i], (nobj=nbin, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(prfc_.crnk[i], (nobj=nbin, &nobj), bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(prfc_.sigmc[i], (nobj=nbin, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpackr8_(prfc_.sig[i], (nobj=nbin, &nobj),bank, 
			       &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpacki2asi4_(prfc_.ig[i], (nobj=nbin, &nobj),bank, 
				   &prfc_blen, &prfc_maxlen))) return rcode; 
  }

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.mtxinfo[i] == PRFC_MTXINFO_UNUSED ) continue;
    
    if ((rcode = dst_unpacki2asi4_( &prfc_.nel[i], (nobj=1, &nobj), bank, 
				    &prfc_blen, &prfc_maxlen))) return rcode; 
    if ((rcode = dst_unpacki2asi4_( &prfc_.mor[i], (nobj=1, &nobj), bank, 
				    &prfc_blen, &prfc_maxlen))) return rcode; 
    if ( prfc_.nel[i] > PRFC_MAXMEL ) {
      fprintf(stderr, "%s: Error Matrix for fit %d has more elements (%d) than in the\n",
	      __FILE__, i+1, prfc_.nel[i] );
      fprintf(stderr, "  %d reserved in the structure. Only unpacking %d elements\n",
	      PRFC_MAXMEL, PRFC_MAXMEL );
      prfc_.nel[i] = PRFC_MAXMEL;
    }
    if ((rcode = dst_unpackr8_( prfc_.mxel[i], (nobj=prfc_.nel[i], &nobj),
				bank, &prfc_blen, &prfc_maxlen))) return rcode;
  }

  return SUCCESS;
}

integer4 prfc_common_to_dump_(integer4 *long_output)
{
  return prfc_common_to_dumpf_(stdout, long_output);
}

integer4 prfc_common_to_dumpf_(FILE* fp, integer4 *long_output)
{
  integer4 mark_header = 0;
  integer4 i, j, k, idx;
  integer1 trajName[16];

  static struct {
    integer4  code;
    integer1 *mess;
  } trans1[] = {
    { PRFC_IG_CHERENKOV_CUT, "cherenkov cut"    },
    { PRFC_IG_SICKPLNFIT,    "sick plane fit"   },
    { PRFC_IG_OVERCORRECTED, "too much corr"    },
    { PRFC_IG_GOODBIN,       ""                 },  /* good bin */
    { 999,                   "unknown ig code"  }   /* marks end of list */ 
  };

  static struct {
    integer4  code;
    integer1 *mess;
  } trans2[] = {
    { PRFC_FIT_NOT_REQUESTED,       "Fit not requested"                     },
    { PRFC_NOT_IMPLEMENTED,         "Fit not implemented"                   },
    { PRFC_REQUIRED_BANKS_MISSING,  
      "Bank(s) required for fit are missing or have failed"                 },
    { PRFC_MISSING_TRAJECTORY_INFO,
      "Bank(s) required for desired trajectory source are missing/failed"   },
    { PRFC_UPWARD_GOING_TRACK,      "Upward going track"                    },
    { PRFC_TOO_FEW_GOOD_BINS,       "Too few good bins"                     },
    { PRFC_FITTER_FAILURE,          "Fitter failed"                         },	
    { PRFC_INSANE_TRAJECTORY,
      "Trajectory (direction and/or core) unreasonable"                     },
    { 999,                          "Unknown failmode"                      }/* marks end of list */
  };


  fprintf(fp,"\nPRFC bank. bins: ");
  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    if ( prfc_.bininfo[i] == PRFC_BININFO_USED )
      fprintf(fp," %03d", prfc_.nbin[i]);
    else
      fprintf(fp," -- ");
  }
  fprintf(fp,"\n\n");

  
  /* Show geometry results */

  for ( i=0; i<PRFC_MAXFIT; i++ ) {
    
    if ( prfc_.pflinfo[i] == PRFC_PFLINFO_UNUSED ) continue;

    fprintf(fp,"    -> Profile Fit %1d\n", i+1);
    
    if ( prfc_.failmode[i] != SUCCESS ) {
      for ( k=0; trans2[k].code!=999 ; k++ )
	if ( prfc_.failmode[i] == trans2[k].code ) break;

      fprintf(fp,"    %s\n", trans2[k].mess );
      continue;    /* Nothing else to show for this fit */
    }
    
    if ( !mark_header ) {
      fprintf(fp,"\n            value   stat error        right       left     geom error\n");
      mark_header = 1;
    }

    fprintf(fp,"  Szmx: %9.3e +- %9.3e   (%9.3e, %9.3e)  +- %9.3e  particles\n",
	    prfc_.szmx[i], prfc_.dszmx[i], prfc_.rszmx[i], prfc_.lszmx[i],
	    prfc_.tszmx[i] );
    
    fprintf(fp,"  Xmax: %9.2f +- %9.2f   (%9.2f, %9.2f)  +- %9.2f  g/cm^2\n",
	    prfc_.xm[i],  prfc_.dxm[i],  prfc_.rxm[i],  prfc_.lxm[i],
	    prfc_.txm[i] );
    
    fprintf(fp,"    X0: %9.2f +- %9.2f   (%9.2f, %9.2f)  +- %9.2f  g/cm^2\n",
	    prfc_.x0[i],  prfc_.dx0[i],  prfc_.rx0[i],  prfc_.lx0[i],
	    prfc_.tx0[i] );
    
    fprintf(fp,"  Lamb: %9.2f +- %9.2f   (%9.2f, %9.2f)  +- %9.2f  g/cm^2\n",
	    prfc_.lambda[i],  prfc_.dlambda[i], prfc_.rlambda[i], 
	    prfc_.llambda[i], prfc_.tlambda[i] );
    
    fprintf(fp,"  Engy: %9.3f +- %9.3f   (%9.3f, %9.3f)  +- %9.3f  EeV\n",
	    prfc_.eng[i],  prfc_.deng[i],  prfc_.reng[i],  prfc_.leng[i],
	    prfc_.teng[i] );
    
    fprintf(fp,"\n");
    fprintf(fp," chi2/ndf: %7.3f / %3d\n", prfc_.chi2[i], prfc_.ndf[i] );
    if ( eventNameFromId( prfc_.traj_source[i], trajName, sizeof(trajName))
	 == 0 ) strcpy(trajName,"Unknown Bank");
    fprintf(fp," trajectory source: %5d (%s)    errstat: %d\n", 
	    prfc_.traj_source[i], trajName, prfc_.errstat[i] );
    if ( prfc_.errstat[i] != SUCCESS ) {
      if ( prfc_.errstat[i] & PRFC_STAT_ERROR_FAILURE )
	fprintf(fp,"   STATISTICAL errors failed\n");
      if ( prfc_.errstat[i] & PRFC_RIGHT_ERROR_FAILURE )
	fprintf(fp,"   RIGHT TRAJECTORY errors failed\n");
      if ( prfc_.errstat[i] & PRFC_LEFT_ERROR_FAILURE )
	fprintf(fp,"   LEFT TRAJECTORY errors failed\n");
      if ( prfc_.errstat[i] & PRFC_GEOM_ERROR_FAILURE )
	fprintf(fp,"   GEOMETRICAL errors failed\n");
      if ( prfc_.errstat[i] & PRFC_GEOM_ERROR_INCOMPLETE )
	fprintf(fp,"   GEOMETRICAL errors incomplete\n");
    }
    fprintf(fp,"\n");
  }


  /* If long output is selected show the light contributions from
     each light source for each profile */

  if ( (*long_output)==1 ) {

    for (i=0; i<PRFC_MAXFIT; i++) {
    
      if ( prfc_.bininfo[i] == PRFC_BININFO_UNUSED ) continue;

      fprintf(fp,"    -> Profile Bins %1d\n", i+1);
      fprintf(fp,"    slant     scin     rayl    aero     crnk   mc_tot  signal  ig\n");
      
      for (j=0; j<prfc_.nbin[i]; j++) {
        for ( k=0; trans1[k].code!=999 ; k++ )
	  if ( prfc_.ig[i][j] == trans1[k].code ) break;
	
	fprintf(fp,"  %8.2f  %7.3f %7.3f %7.3f %9.4f %7.2f %7.2f  %s\n", 
		prfc_.dep[i][j],  prfc_.scin[i][j], prfc_.rayl[i][j], 
		prfc_.aero[i][j], prfc_.crnk[i][j], prfc_.sigmc[i][j],
		prfc_.sig[i][j],  trans1[k].mess );
      }
      fprintf(fp,"\n");
    }
    
    for (i=0; i<PRFC_MAXFIT; i++) {

      if ( prfc_.mtxinfo[i] == PRFC_MTXINFO_UNUSED ) continue;

      fprintf(fp,"    -> Error matrix %1d\n", i+1);
      for ( j=0; j<prfc_.mor[i]; j++) {
	if ( j == 0 ) fprintf(fp,"  / ");
	else if ( j == prfc_.mor[i]-1 ) fprintf(fp,"  \\ ");
	else fprintf(fp,"  | ");
	for ( k=0; k<prfc_.mor[i]; k++) {
	  if ( k >= j )
	    idx = prfc_.nel[i] - ((prfc_.mor[i]-j) * (prfc_.mor[i]-j + 1))/2
	      + ( k - j);
	  else
	    idx = prfc_.nel[i] - ((prfc_.mor[i]-k) * (prfc_.mor[i]-k + 1))/2
	      + ( j - k);
	  
	  fprintf(fp,"%13.6lg", prfc_.mxel[i][idx]);
	}
	if ( j == 0 ) fprintf(fp,"  \\\n");
	else if ( j == prfc_.mor[i]-1 ) fprintf(fp,"  /\n");
	else fprintf(fp,"  |\n");
      }
      fprintf(fp,"\n");
    }
  }
  
  fprintf(fp,"\n");

  return SUCCESS;
}
