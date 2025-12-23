/*
 * C functions for rusdgeom
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
#include "rusdgeom_dst.h"

rusdgeom_dst_common rusdgeom_;	/* allocate memory to rusdgeom_common */

static integer4 rusdgeom_blen = 0;
static integer4 rusdgeom_maxlen =
  sizeof (integer4) * 2 + sizeof (rusdgeom_dst_common);
static integer1 *rusdgeom_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* rusdgeom_bank_buffer_ (integer4* rusdgeom_bank_buffer_size)
{
  (*rusdgeom_bank_buffer_size) = rusdgeom_blen;
  return rusdgeom_bank;
}



static void
rusdgeom_bank_init ()
{
  rusdgeom_bank = (integer1 *) calloc (rusdgeom_maxlen, sizeof (integer1));
  if (rusdgeom_bank == NULL)
    {
      fprintf (stderr,
	       "rusdgeom_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"rusdgeom_bank allocated memory %d\n",rusdgeom_maxlen); */
}

integer4
rusdgeom_common_to_bank_()
{
  static integer4 id = RUSDGEOM_BANKID, ver = RUSDGEOM_BANKVERSION;
  integer4 rcode, nobj, i;

  if (rusdgeom_bank == NULL)
    rusdgeom_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &rusdgeom_blen, &rusdgeom_maxlen, rusdgeom_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;
  
  rcode +=
    dst_packi4_ (&rusdgeom_.nsds, &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  
  nobj=rusdgeom_.nsds;
  
  rcode +=
    dst_packi4_ (&rusdgeom_.nsig[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);

  for (i=0; i<rusdgeom_.nsds;i++)
    {
      nobj=rusdgeom_.nsig[i];
      
      rcode +=
	dst_packr8_ (&rusdgeom_.sdsigq[i][0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		     &rusdgeom_maxlen);
      rcode +=
	dst_packr8_ (&rusdgeom_.sdsigt[i][0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		     &rusdgeom_maxlen); 

      rcode +=
	dst_packr8_ (&rusdgeom_.sdsigte[i][0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		     &rusdgeom_maxlen); 

      rcode +=
	dst_packi4_ (&rusdgeom_.igsig[i][0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		     &rusdgeom_maxlen);
      rcode +=
	dst_packi4_ (&rusdgeom_.irufptn[i][0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		     &rusdgeom_maxlen);  
      nobj=3;
      rcode +=
	dst_packr8_ (&rusdgeom_.xyzclf[i][0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		     &rusdgeom_maxlen);    
    }

  nobj=rusdgeom_.nsds;
  
  rcode +=
    dst_packr8_ (&rusdgeom_.pulsa[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);

  rcode +=
    dst_packr8_ (&rusdgeom_.sdtime[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);

  rcode +=
    dst_packr8_ (&rusdgeom_.sdterr[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  
  rcode +=
    dst_packi4_ (&rusdgeom_.igsd[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packi4_ (&rusdgeom_.xxyy[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packi4_ (&rusdgeom_.sdirufptn[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  
  

  nobj=3;
  
  rcode +=
    dst_packr8_ (&rusdgeom_.xcore[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.dxcore[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.ycore[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.dycore[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.t0[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.dt0[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.theta[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.dtheta[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.phi[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.dphi[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.chi2[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packi4_ (&rusdgeom_.ndof[0], &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  nobj=1;
  
  rcode +=
    dst_packr8_ (&rusdgeom_.a, &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.da, &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  rcode +=
    dst_packr8_ (&rusdgeom_.tearliest, &nobj, rusdgeom_bank, &rusdgeom_blen,
		 &rusdgeom_maxlen);
  
  
  
  
  

  return rcode;
}

integer4
rusdgeom_bank_to_dst_ (integer4 * unit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (unit, &rusdgeom_blen, rusdgeom_bank);
  free (rusdgeom_bank);
  rusdgeom_bank = NULL;
  return rcode;
}

integer4
rusdgeom_common_to_dst_ (integer4 * unit)
{
  integer4 rcode;
    if ( (rcode = rusdgeom_common_to_bank_()) )
    {
      fprintf (stderr, "rusdgeom_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
    if ( (rcode = rusdgeom_bank_to_dst_(unit) ))
    {
      fprintf (stderr, "rusdgeom_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4
rusdgeom_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj, i;
  rusdgeom_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;

  
  rcode +=
    dst_unpacki4_ (&rusdgeom_.nsds, &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  
  nobj=rusdgeom_.nsds;
  
  rcode +=
    dst_unpacki4_ (&rusdgeom_.nsig[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  
  for (i=0; i<rusdgeom_.nsds;i++)
    {
      nobj=rusdgeom_.nsig[i];
      
      rcode +=
	dst_unpackr8_ (&rusdgeom_.sdsigq[i][0], &nobj, bank, &rusdgeom_blen,
		       &rusdgeom_maxlen);
      rcode +=
	dst_unpackr8_ (&rusdgeom_.sdsigt[i][0], &nobj, bank, &rusdgeom_blen,
		       &rusdgeom_maxlen);   
      rcode +=
	dst_unpackr8_ (&rusdgeom_.sdsigte[i][0], &nobj, bank, &rusdgeom_blen,
		       &rusdgeom_maxlen);   
      rcode +=
	dst_unpacki4_ (&rusdgeom_.igsig[i][0], &nobj, bank, &rusdgeom_blen,
		       &rusdgeom_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdgeom_.irufptn[i][0], &nobj, bank, &rusdgeom_blen,
		       &rusdgeom_maxlen);  
      nobj=3;
      rcode +=
	dst_unpackr8_ (&rusdgeom_.xyzclf[i][0], &nobj, bank, &rusdgeom_blen,
		       &rusdgeom_maxlen);    
    }
  
  nobj=rusdgeom_.nsds;
  
  rcode +=
    dst_unpackr8_ (&rusdgeom_.pulsa[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  
  rcode +=
    dst_unpackr8_ (&rusdgeom_.sdtime[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);

  rcode +=
    dst_unpackr8_ (&rusdgeom_.sdterr[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  
  rcode +=
    dst_unpacki4_ (&rusdgeom_.igsd[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdgeom_.xxyy[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdgeom_.sdirufptn[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  
  nobj=3;
  
  rcode +=
    dst_unpackr8_ (&rusdgeom_.xcore[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.dxcore[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.ycore[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.dycore[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.t0[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.dt0[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.theta[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.dtheta[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.phi[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.dphi[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.chi2[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdgeom_.ndof[0], &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  nobj=1;
  
  rcode +=
    dst_unpackr8_ (&rusdgeom_.a, &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.da, &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdgeom_.tearliest, &nobj, bank, &rusdgeom_blen,
		   &rusdgeom_maxlen);

  return rcode;
}

integer4
rusdgeom_common_to_dump_ (integer4 * long_output)
{
  return rusdgeom_common_to_dumpf_ (stdout, long_output);
}

integer4
rusdgeom_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  integer4 i,j;
  fprintf (fp, "%s :\n","rusdgeom");
  fprintf(fp,"nsds=%d tearliest=%.2f\n",rusdgeom_.nsds,rusdgeom_.tearliest);

  fprintf(fp,
	  "Plane fit  xcore=%.2f+/-%.2f ycore=%.2f+/-%.2f t0=%.2f+/-%.2f theta=%.2f+/-%.2f phi=%.2f+/-%.2f chi2=%.2f ndof=%d\n",
	  rusdgeom_.xcore[0],rusdgeom_.dxcore[0],rusdgeom_.ycore[0],rusdgeom_.dycore[0],rusdgeom_.t0[0],
	  rusdgeom_.dt0[0],rusdgeom_.theta[0],rusdgeom_.dtheta[0],rusdgeom_.phi[0],rusdgeom_.dphi[0],
	  rusdgeom_.chi2[0],rusdgeom_.ndof[0]);
  fprintf(fp,
	  "Modified Linsley fit  xcore=%.2f+/-%.2f ycore=%.2f+/-%.2f t0=%.2f+/-%.2f theta=%.2f+/-%.2f phi=%.2f+/-%.2f chi2=%.2f ndof=%d\n",
	  rusdgeom_.xcore[1],rusdgeom_.dxcore[1],rusdgeom_.ycore[1],rusdgeom_.dycore[1],rusdgeom_.t0[1],
	  rusdgeom_.dt0[1],rusdgeom_.theta[1],rusdgeom_.dtheta[1],rusdgeom_.phi[1],rusdgeom_.dphi[1],
	  rusdgeom_.chi2[1],rusdgeom_.ndof[1]);

  fprintf(fp,
	  "Mod. Lin. fit w curv.  xcore=%.2f+/-%.2f ycore=%.2f+/-%.2f t0=%.2f+/-%.2f theta=%.2f+/-%.2f phi=%.2f+/-%.2f a=%.2f+/-%.2f chi2=%.2f ndof=%d\n",
	  rusdgeom_.xcore[2],rusdgeom_.dxcore[2],rusdgeom_.ycore[2],rusdgeom_.dycore[2],rusdgeom_.t0[2],
	  rusdgeom_.dt0[2],rusdgeom_.theta[2],rusdgeom_.dtheta[2],rusdgeom_.phi[2],rusdgeom_.dphi[2],
	  rusdgeom_.a,rusdgeom_.da,rusdgeom_.chi2[2],rusdgeom_.ndof[2]);
  

  fprintf(fp,"%s%8s%18s%17s%16s%10s%8s\n",
	  "index","xxyy","pulsa,[VEM]","sdtime,[1200m]","sdterr,[1200m]","sdirufptn","igsd");
  for(i=0;i<rusdgeom_.nsds;i++)
    {
      fprintf(fp,"%3d%10.04d%15f%15f%15f%11d%12d\n",i,
	      rusdgeom_.xxyy[i],rusdgeom_.pulsa[i],rusdgeom_.sdtime[i],
	      rusdgeom_.sdterr[i],rusdgeom_.sdirufptn[i],rusdgeom_.igsd[i]);
    }
  

  if (*long_output == 1)
    { 
      
      fprintf(fp,"\n%s%8s%18s%17s%16s%10s%8s\n",
	      "index","xxyy","sdsigq,[VEM]","sdsigt,[1200m]","sdsigte,[1200m]","sdirufptn","igsig");
      for(i=0;i<rusdgeom_.nsds;i++)
	{
	  for(j=0;j<rusdgeom_.nsig[i];j++)
	    {
	      fprintf(fp,"%3d%10.04d%15f%15f%15f%11d%12d\n",i,
		      rusdgeom_.xxyy[i],rusdgeom_.sdsigq[i][j],rusdgeom_.sdsigt[i][j],
		      rusdgeom_.sdsigte[i][j],rusdgeom_.irufptn[i][j],rusdgeom_.igsig[i][j]);
	    }
	  
	}
    } 
  return 0;
}
