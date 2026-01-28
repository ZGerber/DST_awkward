/*
 * C functions for tlfptn
 * Dmitri Ivanov, dmiivanov@gmail.com
 * Jan 28, 2020
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "tlfptn_dst.h"

tlfptn_dst_common tlfptn_;	/* allocate memory to tlfptn_common */

static integer4 tlfptn_blen = 0;
static integer4 tlfptn_maxlen =
  sizeof (integer4) * 2 + sizeof (tlfptn_dst_common);
static integer1 *tlfptn_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* tlfptn_bank_buffer_ (integer4* tlfptn_bank_buffer_size)
{
  (*tlfptn_bank_buffer_size) = tlfptn_blen;
  return tlfptn_bank;
}



static void tlfptn_bank_init ()
{
  tlfptn_bank = (integer1 *) calloc (tlfptn_maxlen, sizeof (integer1));
  if (tlfptn_bank == NULL)
    {
      fprintf (stderr,
	       "tlfptn_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"tlfptn_bank allocated memory %d\n",tlfptn_maxlen); */
}

integer4 tlfptn_common_to_bank_()
{
  static integer4 id = TLFPTN_BANKID, ver = TLFPTN_BANKVERSION;
  integer4 rcode, nobj, i;

  if (tlfptn_bank == NULL)
    tlfptn_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &tlfptn_blen, &tlfptn_maxlen, tlfptn_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;

  rcode +=
    dst_packi4_ (&tlfptn_.nhits, &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen); 
  rcode +=
    dst_packi4_ (&tlfptn_.nsclust, &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packi4_ (&tlfptn_.nstclust, &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);

  rcode +=
    dst_packi4_ (&tlfptn_.nborder, &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  
  nobj=tlfptn_.nhits;

  rcode +=
    dst_packi4_ (&tlfptn_.isgood[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packi4_ (&tlfptn_.wfindex[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packi4_ (&tlfptn_.xxyy[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packi4_ (&tlfptn_.nfold[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  
  for (i=0; i<tlfptn_.nhits;i++)
    {
      nobj=2;
      rcode +=
	dst_packi4_ (&tlfptn_.sstart[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packi4_ (&tlfptn_.sstop[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packi4_ (&tlfptn_.lderiv[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packi4_ (&tlfptn_.zderiv[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      nobj=3;
      
      rcode +=
	dst_packr8_ (&tlfptn_.xyzclf[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      nobj=2;

      rcode +=
	dst_packr8_ (&tlfptn_.reltime[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.timeerr[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.fadcpa[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.fadcpaerr[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.pulsa[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.pulsaerr[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.ped[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.pederr[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.vem[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.vemerr[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);      
    }
  
  nobj=2;

  rcode +=
    dst_packr8_ (&tlfptn_.qtot[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen); 
  rcode +=
    dst_packr8_ (&tlfptn_.tearliest[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  
  for(i=0;i<3;i++)
    {
      nobj=tlfptn_.nhits;
      
      rcode +=
	dst_packr8_ (&tlfptn_.tyro_cdist[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
   
      nobj=5;
      
      rcode +=
	dst_packr8_ (&tlfptn_.tyro_xymoments[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      nobj=2;
      
      rcode +=
	dst_packr8_ (&tlfptn_.tyro_xypmoments[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.tyro_u[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.tyro_v[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
      rcode +=
	dst_packr8_ (&tlfptn_.tyro_tfitpars[i][0], &nobj, tlfptn_bank, &tlfptn_blen,
		     &tlfptn_maxlen);
    }

  nobj=3;
  
  rcode +=
    dst_packr8_ (&tlfptn_.tyro_chi2[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packr8_ (&tlfptn_.tyro_ndof[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packr8_ (&tlfptn_.tyro_theta[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  rcode +=
    dst_packr8_ (&tlfptn_.tyro_phi[0], &nobj, tlfptn_bank, &tlfptn_blen,
		 &tlfptn_maxlen);
  

  return rcode;
}

integer4 tlfptn_bank_to_dst_ (integer4 * unit)
{
  integer4 rcode = dst_write_bank_ (unit, &tlfptn_blen, tlfptn_bank);
  free (tlfptn_bank);
  tlfptn_bank = NULL;
  return rcode;
}

integer4 tlfptn_common_to_dst_ (integer4 * unit)
{
  integer4 rcode;
    if ( (rcode = tlfptn_common_to_bank_()) )
    {
      fprintf (stderr, "tlfptn_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
    if ( (rcode = tlfptn_bank_to_dst_(unit) ))
    {
      fprintf (stderr, "tlfptn_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4 tlfptn_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj, i;
  tlfptn_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;
 
  rcode +=
    dst_unpacki4_ (&tlfptn_.nhits, &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen); 
  rcode +=
    dst_unpacki4_ (&tlfptn_.nsclust, &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpacki4_ (&tlfptn_.nstclust, &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpacki4_ (&tlfptn_.nborder, &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  
  nobj=tlfptn_.nhits;

  rcode +=
    dst_unpacki4_ (&tlfptn_.isgood[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpacki4_ (&tlfptn_.wfindex[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpacki4_ (&tlfptn_.xxyy[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpacki4_ (&tlfptn_.nfold[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  
  for (i=0; i<tlfptn_.nhits;i++)
    {
      nobj=2;
      rcode +=
	dst_unpacki4_ (&tlfptn_.sstart[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpacki4_ (&tlfptn_.sstop[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpacki4_ (&tlfptn_.lderiv[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpacki4_ (&tlfptn_.zderiv[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      nobj=3;
      
      rcode +=
	dst_unpackr8_ (&tlfptn_.xyzclf[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      nobj=2;

      rcode +=
	dst_unpackr8_ (&tlfptn_.reltime[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.timeerr[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.fadcpa[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.fadcpaerr[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.pulsa[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.pulsaerr[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.ped[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.pederr[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.vem[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.vemerr[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);      
    }
  
  nobj=2;

  rcode +=
    dst_unpackr8_ (&tlfptn_.qtot[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen); 
  rcode +=
    dst_unpackr8_ (&tlfptn_.tearliest[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  
  for(i=0;i<3;i++)
    {
      nobj=tlfptn_.nhits;
      
      rcode +=
	dst_unpackr8_ (&tlfptn_.tyro_cdist[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
   
      nobj=5;
      
      rcode +=
	dst_unpackr8_ (&tlfptn_.tyro_xymoments[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      nobj=2;
      
      rcode +=
	dst_unpackr8_ (&tlfptn_.tyro_xypmoments[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.tyro_u[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.tyro_v[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
      rcode +=
	dst_unpackr8_ (&tlfptn_.tyro_tfitpars[i][0], &nobj, bank, &tlfptn_blen,
		       &tlfptn_maxlen);
    }

  nobj=3;
  
  rcode +=
    dst_unpackr8_ (&tlfptn_.tyro_chi2[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpackr8_ (&tlfptn_.tyro_ndof[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpackr8_ (&tlfptn_.tyro_theta[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  rcode +=
    dst_unpackr8_ (&tlfptn_.tyro_phi[0], &nobj, bank, &tlfptn_blen,
		   &tlfptn_maxlen);
  

 

  return rcode;
}

integer4 tlfptn_common_to_dump_ (integer4 * long_output)
{
  return tlfptn_common_to_dumpf_ (stdout, long_output);
}

integer4 tlfptn_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  fprintf (fp, "%s :\n","tlfptn");
  fprintf(fp, 
	  "nhits %d nsclust %d nstclust %d nborder %d core_x %f core_y %f t0 %.9f \n",
	  tlfptn_.nhits,tlfptn_.nsclust,tlfptn_.nstclust,tlfptn_.nborder,
	  tlfptn_.tyro_xymoments[2][0]+TLFPTN_ORIGIN_X_CLF,
	  tlfptn_.tyro_xymoments[2][1]+TLFPTN_ORIGIN_Y_CLF,
	  0.5*(tlfptn_.tearliest[0]+tlfptn_.tearliest[1])+
	  tlfptn_.tyro_tfitpars[2][0] * 1e-6);
  
  if(*long_output)
    {
      integer4 i;
      fprintf(fp, "%s%8s%14s%15s%15s%18s%18s\n",
	      "#","XXYY","Q upper","Q lower","T upper","T lower","isgood");
      for(i=0; i<tlfptn_.nhits; i++)
	{
	  fprintf(fp,"%02d%7.4d%15f%15f%22.9f%18.9f%7d\n",
		  i,tlfptn_.xxyy[i],tlfptn_.pulsa[i][0],tlfptn_.pulsa[i][1],
		  tlfptn_.tearliest[0]+(1e-6)*tlfptn_.reltime[i][0],
		  tlfptn_.tearliest[1]+(1e-6)*tlfptn_.reltime[i][1],
		  tlfptn_.isgood[i]);
	}
    }
   
  return 0;
}
