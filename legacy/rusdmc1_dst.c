/*
 * C functions for rusdmc1
 * Dmitri Ivanov, ivanov@physics.rutgers.edu
 * Nov 29, 2009
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "rusdmc1_dst.h"
#define RADDEG 57.2957795131


rusdmc1_dst_common rusdmc1_;	/* allocate memory to rusdmc1_common */

static integer4 rusdmc1_blen = 0;
static integer4 rusdmc1_maxlen =
  sizeof (integer4) * 2 + sizeof (rusdmc1_dst_common);
static integer1 *rusdmc1_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* rusdmc1_bank_buffer_ (integer4* rusdmc1_bank_buffer_size)
{
  (*rusdmc1_bank_buffer_size) = rusdmc1_blen;
  return rusdmc1_bank;
}



static void
rusdmc1_bank_init ()
{
  rusdmc1_bank = (integer1 *) calloc (rusdmc1_maxlen, sizeof (integer1));
  if (rusdmc1_bank == NULL)
    {
      fprintf (stderr,
	       "rusdmc1_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"rusdmc1_bank allocated memory %d\n",rusdmc1_maxlen); */
}

integer4
rusdmc1_common_to_bank_ ()
{
  static integer4 id = RUSDMC1_BANKID, ver = RUSDMC1_BANKVERSION;
  integer4 rcode, nobj;

  if (rusdmc1_bank == NULL)
    rusdmc1_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &rusdmc1_blen, &rusdmc1_maxlen, rusdmc1_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;

  rcode +=
    dst_packr8_ (&rusdmc1_.xcore, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.ycore, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.t0, &nobj,  rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.bdist, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.tdistbr, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.tdistlr, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.tdistsk, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  rcode +=
    dst_packr8_ (&rusdmc1_.tdist, &nobj, rusdmc1_bank, &rusdmc1_blen,
		 &rusdmc1_maxlen);
  
  
  return rcode;
}

integer4
rusdmc1_bank_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (NumUnit, &rusdmc1_blen, rusdmc1_bank);
  free (rusdmc1_bank);
  rusdmc1_bank = NULL;
  return rcode;
}

integer4
rusdmc1_common_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  if ((rcode = rusdmc1_common_to_bank_ ()))
    {
      fprintf (stderr, "rusdmc1_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  if ((rcode = rusdmc1_bank_to_dst_ (NumUnit)))
    {
      fprintf (stderr, "rusdmc1_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4
rusdmc1_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj;
  rusdmc1_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;
  
  rcode +=
    dst_unpackr8_ (&rusdmc1_.xcore, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.ycore, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.t0, &nobj,  bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.bdist, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.tdistbr, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.tdistlr, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.tdistsk, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  rcode +=
    dst_unpackr8_ (&rusdmc1_.tdist, &nobj, bank, &rusdmc1_blen,
		   &rusdmc1_maxlen);
  return rcode;
}

integer4
rusdmc1_common_to_dump_ (integer4 * long_output)
{
  return rusdmc1_common_to_dumpf_ (stdout, long_output);
}

integer4
rusdmc1_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  fprintf (fp, "%s :\n","rusdmc1");
  if ( *long_output == 0)
    {
      fprintf (fp, "xcore %f ycore %f t0 %f bdist %f tdist %f\n",
	       rusdmc1_.xcore,rusdmc1_.ycore,rusdmc1_.t0,rusdmc1_.bdist,rusdmc1_.tdist);
    }
  else
    {
      fprintf (fp, "xcore %f ycore %f t0 %f bdist %f tdistbr %f tdistlr %f tdistsk %f tdist %f\n",
	       rusdmc1_.xcore,rusdmc1_.ycore,rusdmc1_.t0,rusdmc1_.bdist,
	       rusdmc1_.tdistbr,rusdmc1_.tdistlr,rusdmc1_.tdistsk,
	       rusdmc1_.tdist);
    }
  return 0;
}
