/*
 * C functions for rufptn
 * Dmitri Ivanov, ivanov@physics.rutgers.edu
 * Oct 1, 2008
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "rufptn_dst.h"

rufptn_dst_common rufptn_;	/* allocate memory to rufptn_common */

static integer4 rufptn_blen = 0;
static integer4 rufptn_maxlen =
  sizeof (integer4) * 2 + sizeof (rufptn_dst_common);
static integer1 *rufptn_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* rufptn_bank_buffer_ (integer4* rufptn_bank_buffer_size)
{
  (*rufptn_bank_buffer_size) = rufptn_blen;
  return rufptn_bank;
}



static void
rufptn_bank_init ()
{
  rufptn_bank = (integer1 *) calloc (rufptn_maxlen, sizeof (integer1));
  if (rufptn_bank == NULL)
    {
      fprintf (stderr,
	       "rufptn_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"rufptn_bank allocated memory %d\n",rufptn_maxlen); */
}

integer4
rufptn_common_to_bank_()
{
  static integer4 id = RUFPTN_BANKID, ver = RUFPTN_BANKVERSION;
  integer4 rcode, nobj, i;

  if (rufptn_bank == NULL)
    rufptn_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &rufptn_blen, &rufptn_maxlen, rufptn_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;

  rcode +=
    dst_packi4_ (&rufptn_.nhits, &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen); 
  rcode +=
    dst_packi4_ (&rufptn_.nsclust, &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packi4_ (&rufptn_.nstclust, &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);

  rcode +=
    dst_packi4_ (&rufptn_.nborder, &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  
  nobj=rufptn_.nhits;

  rcode +=
    dst_packi4_ (&rufptn_.isgood[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packi4_ (&rufptn_.wfindex[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packi4_ (&rufptn_.xxyy[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packi4_ (&rufptn_.nfold[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  
  for (i=0; i<rufptn_.nhits;i++)
    {
      nobj=2;
      rcode +=
	dst_packi4_ (&rufptn_.sstart[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packi4_ (&rufptn_.sstop[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packi4_ (&rufptn_.lderiv[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packi4_ (&rufptn_.zderiv[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      nobj=3;
      
      rcode +=
	dst_packr8_ (&rufptn_.xyzclf[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      nobj=2;

      rcode +=
	dst_packr8_ (&rufptn_.reltime[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.timeerr[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.fadcpa[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.fadcpaerr[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.pulsa[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.pulsaerr[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.ped[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.pederr[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.vem[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.vemerr[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);      
    }
  
  nobj=2;

  rcode +=
    dst_packr8_ (&rufptn_.qtot[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen); 
  rcode +=
    dst_packr8_ (&rufptn_.tearliest[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  
  for(i=0;i<3;i++)
    {
      nobj=rufptn_.nhits;
      
      rcode +=
	dst_packr8_ (&rufptn_.tyro_cdist[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
   
      nobj=5;
      
      rcode +=
	dst_packr8_ (&rufptn_.tyro_xymoments[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      nobj=2;
      
      rcode +=
	dst_packr8_ (&rufptn_.tyro_xypmoments[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.tyro_u[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.tyro_v[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
      rcode +=
	dst_packr8_ (&rufptn_.tyro_tfitpars[i][0], &nobj, rufptn_bank, &rufptn_blen,
		     &rufptn_maxlen);
    }

  nobj=3;
  
  rcode +=
    dst_packr8_ (&rufptn_.tyro_chi2[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packr8_ (&rufptn_.tyro_ndof[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packr8_ (&rufptn_.tyro_theta[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  rcode +=
    dst_packr8_ (&rufptn_.tyro_phi[0], &nobj, rufptn_bank, &rufptn_blen,
		 &rufptn_maxlen);
  

  return rcode;
}

integer4
rufptn_bank_to_dst_ (integer4 * unit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (unit, &rufptn_blen, rufptn_bank);
  free (rufptn_bank);
  rufptn_bank = NULL;
  return rcode;
}

integer4
rufptn_common_to_dst_ (integer4 * unit)
{
  integer4 rcode;
    if ( (rcode = rufptn_common_to_bank_()) )
    {
      fprintf (stderr, "rufptn_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
    if ( (rcode = rufptn_bank_to_dst_(unit) ))
    {
      fprintf (stderr, "rufptn_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4
rufptn_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj, i;
  rufptn_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;
 
  rcode +=
    dst_unpacki4_ (&rufptn_.nhits, &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen); 
  rcode +=
    dst_unpacki4_ (&rufptn_.nsclust, &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpacki4_ (&rufptn_.nstclust, &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpacki4_ (&rufptn_.nborder, &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  
  nobj=rufptn_.nhits;

  rcode +=
    dst_unpacki4_ (&rufptn_.isgood[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpacki4_ (&rufptn_.wfindex[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpacki4_ (&rufptn_.xxyy[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpacki4_ (&rufptn_.nfold[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  
  for (i=0; i<rufptn_.nhits;i++)
    {
      nobj=2;
      rcode +=
	dst_unpacki4_ (&rufptn_.sstart[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpacki4_ (&rufptn_.sstop[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpacki4_ (&rufptn_.lderiv[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpacki4_ (&rufptn_.zderiv[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      nobj=3;
      
      rcode +=
	dst_unpackr8_ (&rufptn_.xyzclf[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      nobj=2;

      rcode +=
	dst_unpackr8_ (&rufptn_.reltime[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.timeerr[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.fadcpa[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.fadcpaerr[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.pulsa[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.pulsaerr[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.ped[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.pederr[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.vem[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.vemerr[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);      
    }
  
  nobj=2;

  rcode +=
    dst_unpackr8_ (&rufptn_.qtot[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen); 
  rcode +=
    dst_unpackr8_ (&rufptn_.tearliest[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  
  for(i=0;i<3;i++)
    {
      nobj=rufptn_.nhits;
      
      rcode +=
	dst_unpackr8_ (&rufptn_.tyro_cdist[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
   
      nobj=5;
      
      rcode +=
	dst_unpackr8_ (&rufptn_.tyro_xymoments[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      nobj=2;
      
      rcode +=
	dst_unpackr8_ (&rufptn_.tyro_xypmoments[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.tyro_u[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.tyro_v[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
      rcode +=
	dst_unpackr8_ (&rufptn_.tyro_tfitpars[i][0], &nobj, bank, &rufptn_blen,
		       &rufptn_maxlen);
    }

  nobj=3;
  
  rcode +=
    dst_unpackr8_ (&rufptn_.tyro_chi2[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpackr8_ (&rufptn_.tyro_ndof[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpackr8_ (&rufptn_.tyro_theta[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  rcode +=
    dst_unpackr8_ (&rufptn_.tyro_phi[0], &nobj, bank, &rufptn_blen,
		   &rufptn_maxlen);
  

 

  return rcode;
}

integer4
rufptn_common_to_dump_ (integer4 * long_output)
{
  return rufptn_common_to_dumpf_ (stdout, long_output);
}

integer4
rufptn_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  integer4 i;
  fprintf (fp, "%s :\n","rufptn");
  fprintf(fp, 
	  "nhits %d nsclust %d nstclust %d nborder %d core_x %f core_y %f t0 %.9f \n",
	  rufptn_.nhits,rufptn_.nsclust,rufptn_.nstclust,rufptn_.nborder,
	  rufptn_.tyro_xymoments[2][0]+RUFPTN_ORIGIN_X_CLF,
	  rufptn_.tyro_xymoments[2][1]+RUFPTN_ORIGIN_Y_CLF,
	  0.5*(rufptn_.tearliest[0]+rufptn_.tearliest[1])+
	  rufptn_.tyro_tfitpars[2][0]/RUFPTN_TIMDIST*1e-6);
  
  
  fprintf(fp, "%s%8s%14s%15s%15s%18s%18s\n",
	  "#","XXYY","Q upper","Q lower","T upper","T lower","isgood");
  for(i=0; i<rufptn_.nhits; i++)
    {
      fprintf(fp,"%02d%7.4d%15f%15f%22.9f%18.9f%7d\n",
	      i,rufptn_.xxyy[i],rufptn_.pulsa[i][0],rufptn_.pulsa[i][1],
	      rufptn_.tearliest[0]+(4.0028e-6)*rufptn_.reltime[i][0],
	      rufptn_.tearliest[1]+(4.0028e-6)*rufptn_.reltime[i][1],
	      rufptn_.isgood[i]);
    }
  
  if(*long_output ==0)
    { 
    }
  
  else if (*long_output == 1)
    { 
    } 
  return 0;
}
