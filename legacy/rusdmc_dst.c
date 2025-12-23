/*
 * C functions for rusdmc
 * Dmitri Ivanov, ivanov@physics.rutgers.edu
 * Jun 17, 2008
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "rusdmc_dst.h"
#define RADDEG 57.2957795131


rusdmc_dst_common rusdmc_;	/* allocate memory to rusdmc_common */

static integer4 rusdmc_blen;
static integer4 rusdmc_maxlen =
  sizeof (integer4) * 2 + sizeof (rusdmc_dst_common);
static integer1 *rusdmc_bank = NULL;
integer1* rusdmc_bank_buffer_ (integer4* rusdmc_bank_buffer_size)
{
  (*rusdmc_bank_buffer_size) = rusdmc_blen;
  return rusdmc_bank;
}
static void rusdmc_bank_init ()
{
  rusdmc_bank = (integer1 *) calloc (rusdmc_maxlen, sizeof (integer1));
  if (rusdmc_bank == NULL)
    {
      fprintf (stderr,
	       "rusdmc_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"rusdmc_bank allocated memory %d\n",rusdmc_maxlen); */
}

integer4 rusdmc_common_to_bank_ ()
{
  static integer4 id = RUSDMC_BANKID, ver = RUSDMC_BANKVERSION;
  integer4 rcode, nobj;

  if (rusdmc_bank == NULL)
    rusdmc_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &rusdmc_blen, &rusdmc_maxlen, rusdmc_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;

  rcode +=
    dst_packi4_ (&rusdmc_.event_num, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen); 
  rcode +=
    dst_packi4_ (&rusdmc_.parttype, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen); 
  rcode +=
    dst_packi4_ (&rusdmc_.corecounter, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen);
  rcode +=
    dst_packi4_ (&rusdmc_.tc, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen);
  rcode +=
    dst_packr4_ (&rusdmc_.energy, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen);
  rcode +=
    dst_packr4_ (&rusdmc_.height, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen);
  rcode +=
    dst_packr4_ (&rusdmc_.theta, &nobj, rusdmc_bank, &rusdmc_blen,
		 &rusdmc_maxlen);
  rcode +=
    dst_packr4_ (&rusdmc_.phi, &nobj,
		 rusdmc_bank, &rusdmc_blen, &rusdmc_maxlen);
  nobj = 3;

  rcode +=
    dst_packr4_ (&rusdmc_.corexyz[0], &nobj,
		 rusdmc_bank, &rusdmc_blen, &rusdmc_maxlen);
  return rcode;
}

integer4 rusdmc_bank_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (NumUnit, &rusdmc_blen, rusdmc_bank);
  free (rusdmc_bank);
  rusdmc_bank = NULL;
  return rcode;
}

integer4 rusdmc_common_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  if ((rcode = rusdmc_common_to_bank_ ()))
    {
      fprintf (stderr, "rusdmc_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  if ((rcode = rusdmc_bank_to_dst_ (NumUnit)))
    {
      fprintf (stderr, "rusdmc_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4 rusdmc_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj;
  rusdmc_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;

  rcode +=
    dst_unpacki4_ (&rusdmc_.event_num, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdmc_.parttype, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdmc_.corecounter, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdmc_.tc, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
   rcode +=
    dst_unpackr4_ (&rusdmc_.energy, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  rcode +=
    dst_unpackr4_ (&rusdmc_.height, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  rcode +=
    dst_unpackr4_ (&rusdmc_.theta, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  rcode +=
    dst_unpackr4_ (&rusdmc_.phi, &nobj, bank, &rusdmc_blen,
		   &rusdmc_maxlen);
  nobj = 3;

  rcode +=
    dst_unpackr4_ (&rusdmc_.corexyz[0], &nobj, bank,
		   &rusdmc_blen, &rusdmc_maxlen);
  return rcode;
}

integer4 rusdmc_common_to_dump_ (integer4 * long_output)
{
  return rusdmc_common_to_dumpf_ (stdout, long_output);
}

integer4 rusdmc_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  (void)(long_output);
  fprintf (fp, "%s :\n","rusdmc");
  fprintf (fp, "Event Number: %d\n", rusdmc_.event_num);
  fprintf (fp, "Corsika Particle ID: %d\n", rusdmc_.parttype);
  fprintf (fp, "Total Energy of Primary Particle: %g EeV\n", rusdmc_.energy);
  fprintf (fp, "Height of First Interaction: %g km\n", rusdmc_.height/1.e5);
  fprintf (fp, "Zenith Angle of Primary Particle Direction: %g Degrees\n", 
	   rusdmc_.theta*RADDEG);
  fprintf (fp, "Azimuth Angle of Primary Particle Direction: %g Degrees (N of E)\n",
	   rusdmc_.phi*RADDEG);
  fprintf (fp, "Counter ID Number for Counter Closest to Core: %d\n", 
	   rusdmc_.corecounter);
  fprintf (fp, "Position of the core in CLF reference frame: (%g,%g,%g) m\n",
	   rusdmc_.corexyz[0]/100., rusdmc_.corexyz[1]/100., rusdmc_.corexyz[2]/100.);
  fprintf (fp, "Time of shower front passing through core position: %d x 20 nsec\n",
	   rusdmc_.tc);
  return 0;
}
