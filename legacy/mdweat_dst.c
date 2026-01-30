/*
 * C functions for mdweat
 * Dmitri Ivanov, dmiivanov@gmail.com
 * Jan 21, 2015
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "mdweat_dst.h"



mdweat_dst_common mdweat_;	/* allocate memory to mdweat_common */

static integer4 mdweat_blen = 0;
static integer4 mdweat_maxlen =
  sizeof (integer4) * 2 + sizeof (mdweat_dst_common);
static integer1 *mdweat_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* mdweat_bank_buffer_ (integer4* mdweat_bank_buffer_size)
{
  (*mdweat_bank_buffer_size) = mdweat_blen;
  return mdweat_bank;
}



static void
mdweat_bank_init ()
{
  mdweat_bank = (integer1 *) calloc (mdweat_maxlen, sizeof (integer1));
  if (mdweat_bank == NULL)
    {
      fprintf (stderr,
	       "mdweat_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"mdweat_bank allocated memory %d\n",mdweat_maxlen); */
}

integer4
mdweat_common_to_bank_ ()
{
  
  static integer4 id = MDWEAT_BANKID, ver = MDWEAT_BANKVERSION;
  integer4 rcode, nobj;
  
  if (mdweat_bank == NULL)
    mdweat_bank_init ();
  
  rcode =
    dst_initbank_ (&id, &ver, &mdweat_blen, &mdweat_maxlen, mdweat_bank);
  /* Initialize test_blen, and pack the id and version to bank */
  
  nobj = 1;
  rcode += dst_packi4_ (&mdweat_.part_num, &nobj, mdweat_bank, &mdweat_blen,&mdweat_maxlen);
  rcode += dst_packi4_ (&mdweat_.code, &nobj, mdweat_bank, &mdweat_blen,&mdweat_maxlen);
  
  return rcode;
}

integer4
mdweat_bank_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode = 0;
  rcode = dst_write_bank_ (NumUnit, &mdweat_blen, mdweat_bank);
  free (mdweat_bank);
  mdweat_bank = NULL;
  return rcode;
}

integer4
mdweat_common_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  if ((rcode = mdweat_common_to_bank_ ()))
    {
      fprintf (stderr, "mdweat_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  if ((rcode = mdweat_bank_to_dst_ (NumUnit)))
    {
      fprintf (stderr, "mdweat_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4
mdweat_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj;
  mdweat_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;

  rcode += dst_unpacki4_ (&mdweat_.part_num, &nobj, bank, &mdweat_blen, &mdweat_maxlen);
  rcode += dst_unpacki4_ (&mdweat_.code, &nobj, bank, &mdweat_blen, &mdweat_maxlen);

  return rcode;
}

integer4
mdweat_common_to_dump_ (integer4 * long_output)
{
  return mdweat_common_to_dumpf_ (stdout, long_output);
}

integer4
mdweat_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
 
  fprintf (fp, "%s :\n","mdweat");
  
  if(*long_output == 0)
    fprintf (fp,"part_num %02d code %07d\n",mdweat_.part_num,mdweat_.code);
  else
    {
      fprintf(fp,"Part number: %02d\n", mdweat_.part_num);
      fprintf(fp,"Part Weather code: %07d\n", mdweat_.code);
      fprintf(fp,"n e s w o t h 7-digit weather code recorder by runners\n");
      fprintf(fp,"n = 1,  0 Clouds North?\n");
      fprintf(fp,"e = 1,  0 Clouds East?\n");
      fprintf(fp,"s = 1,  0 Clouds South?\n");
      fprintf(fp,"w = 1,  0 Clouds West?\n");
      fprintf(fp,"o = 0 - 4 Overhead cloud thickness? 5 - weat code invalid\n");
      fprintf(fp,"t = 1,  0 Stars visible?\n");
      fprintf(fp,"h = 1,  0 Was it hazy? 2 - can't tell\n");
    }
  return 0;
}
