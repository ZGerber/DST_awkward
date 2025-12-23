/*
 * C functions for rufldf
 * Dmitri Ivanov, dmiivanov@gmai.com
 * May 16, 2019
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "rufldf_dst.h"

rufldf_dst_common rufldf_;	/* allocate memory to rufldf_common */

static integer4 rufldf_blen = 0;
static integer4 rufldf_maxlen =
  sizeof (integer4) * 2 + sizeof (rufldf_dst_common);
static integer1 *rufldf_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* rufldf_bank_buffer_ (integer4* rufldf_bank_buffer_size)
{
  (*rufldf_bank_buffer_size) = rufldf_blen;
  return rufldf_bank;
}



static void rufldf_bank_init ()
{
  rufldf_bank = (integer1 *) calloc (rufldf_maxlen, sizeof (integer1));
  if (rufldf_bank == NULL)
    {
      fprintf (stderr,
	       "rufldf_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"rufldf_bank allocated memory %d\n",rufldf_maxlen); */
}

integer4 rufldf_common_to_bank_()
{
  static integer4 id = RUFLDF_BANKID, ver = RUFLDF_BANKVERSION;
  integer4 rcode, nobj;

  if (rufldf_bank == NULL)
    rufldf_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &rufldf_blen, &rufldf_maxlen, rufldf_bank);
  /* Initialize test_blen, and pack the id and version to bank */
  
  nobj = 2;
  rcode +=
    dst_packr8_ (&rufldf_.xcore[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.dxcore[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.ycore[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.dycore[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.sc[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.dsc[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.s600[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.s600_0[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.s800[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.s800_0[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.aenergy[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.energy[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.atmcor[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.chi2[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  nobj = 1;  
  rcode +=
    dst_packr8_ (&rufldf_.theta, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.dtheta, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.phi, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.dphi, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.t0, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.dt0, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.bdist, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.tdistbr, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.tdistlr, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.tdistsk, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  rcode +=
    dst_packr8_ (&rufldf_.tdist, &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  nobj = 2;
  rcode +=
    dst_packi4_ (&rufldf_.ndof[0], &nobj, rufldf_bank, &rufldf_blen,
		 &rufldf_maxlen);
  return rcode;
}

integer4 rufldf_bank_to_dst_ (integer4 * unit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (unit, &rufldf_blen, rufldf_bank);
  free (rufldf_bank);
  rufldf_bank = NULL;
  return rcode;
}

integer4 rufldf_common_to_dst_ (integer4 * unit)
{
  integer4 rcode;
    if ( (rcode = rufldf_common_to_bank_()) )
    {
      fprintf (stderr, "rufldf_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
    if ( (rcode = rufldf_bank_to_dst_(unit) ))
    {
      fprintf (stderr, "rufldf_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4 rufldf_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj;
  integer4 bankid, bankversion;
  
  rufldf_blen = 0;
  nobj = 1;
  rcode += dst_unpacki4_ (&bankid, &nobj, bank, &rufldf_blen, &rufldf_maxlen);
  rcode += dst_unpacki4_ (&bankversion, &nobj, bank, &rufldf_blen, &rufldf_maxlen);

  nobj = 2;
  rcode +=
    dst_unpackr8_ (&rufldf_.xcore[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.dxcore[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.ycore[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.dycore[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.sc[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.dsc[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.s600[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.s600_0[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.s800[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.s800_0[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.aenergy[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.energy[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  
  if(bankversion >=1)
    {
      rcode +=
	dst_unpackr8_ (&rufldf_.atmcor[0], &nobj, bank, &rufldf_blen,
		       &rufldf_maxlen);
    }
  else
    {
      rufldf_.atmcor[0] = 1.0;
      rufldf_.atmcor[1] = 1.0;
    }
  
  rcode +=
    dst_unpackr8_ (&rufldf_.chi2[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  nobj = 1;  
  rcode +=
    dst_unpackr8_ (&rufldf_.theta, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.dtheta, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.phi, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.dphi, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.t0, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.dt0, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.bdist, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.tdistbr, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.tdistlr, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.tdistsk, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  rcode +=
    dst_unpackr8_ (&rufldf_.tdist, &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen);
  nobj = 2;
  rcode +=
    dst_unpacki4_ (&rufldf_.ndof[0], &nobj, bank, &rufldf_blen,
		   &rufldf_maxlen); 
  return rcode;
}

integer4 rufldf_common_to_dump_ (integer4 * long_output)
{
  return rufldf_common_to_dumpf_ (stdout, long_output);
}

integer4 rufldf_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  (void)(long_output);
  fprintf (fp, "%s :\n","rufldf");
  
  fprintf (fp, 
	   "xcore[0] %.2f dxcore[0] %.2f ycore[0] %.2f dycore[0] %.2f s800[0] %.2f energy[0] %.2f atmcor[0]: %.2f chi2[0] %.2f ndof[0] %d\n",
	   rufldf_.xcore[0],rufldf_.dxcore[0],
	   rufldf_.ycore[0],rufldf_.dycore[0],
	   rufldf_.s800[0],rufldf_.energy[0],
	   rufldf_.atmcor[0],rufldf_.chi2[0],rufldf_.ndof[0]
	   );
  
  fprintf (fp, 
	   "xcore[1] %.2f dxcore[1] %.2f ycore[1] %.2f dycore[1] %.2f s800[1] %.2f energy[1] %.2f atmcor[1]: %.2f chi2[1] %.2f ndof[1] %d\n",
	   rufldf_.xcore[1],rufldf_.dxcore[1],
	   rufldf_.ycore[1],rufldf_.dycore[1],
	   rufldf_.s800[1],rufldf_.energy[1],
	   rufldf_.atmcor[1],rufldf_.chi2[1],rufldf_.ndof[1]
	   );
  
  fprintf (fp, 
	   "theta %.2f dtheta %.2f phi %.2f dphi %.2f t0 %.2f dt0 %.2f\n",
	   rufldf_.theta,rufldf_.dtheta,rufldf_.phi,rufldf_.dphi,rufldf_.t0,rufldf_.dt0);
  
  fprintf (fp, 
	   "bdist %.2f tdist %.2f\n",
	   rufldf_.bdist,rufldf_.tdist);
  
  return 0;
}
