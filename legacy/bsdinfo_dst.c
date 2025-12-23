/*
 * C functions for bsdinfo
 * Dmitri Ivanov, dmiivanov@gmail.com
 * Aug 8, 2017
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "bsdinfo_dst.h"



bsdinfo_dst_common bsdinfo_;	/* allocate memory to bsdinfo_common */

static integer4 bsdinfo_blen;
static integer4 bsdinfo_maxlen =
  sizeof (integer4) * 2 + sizeof (bsdinfo_dst_common);
static integer1 *bsdinfo_bank = NULL;

integer1* bsdinfo_bank_buffer_ (integer4* bsdinfo_bank_buffer_size)
{
  (*bsdinfo_bank_buffer_size) = bsdinfo_blen;
  return bsdinfo_bank;
}

static void bsdinfo_bank_init ()
{
  bsdinfo_bank = (integer1 *) calloc (bsdinfo_maxlen, sizeof (integer1));
  if (bsdinfo_bank == NULL)
    {
      fprintf (stderr,
	       "bsdinfo_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"bsdinfo_bank allocated memory %d\n",bsdinfo_maxlen); */
}

integer4 bsdinfo_common_to_bank_ ()
{
  static integer4 id = BSDINFO_BANKID, ver = BSDINFO_BANKVERSION;
  integer4 rcode, nobj;
  if (bsdinfo_bank == NULL)
    bsdinfo_bank_init ();
  /* Initialize test_blen, and pack the id and version to bank */
  rcode = dst_initbank_ (&id, &ver, &bsdinfo_blen, &bsdinfo_maxlen, bsdinfo_bank);
  nobj = 1;
  rcode += dst_packi4_ (&bsdinfo_.yymmdd, &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_packi4_ (&bsdinfo_.hhmmss, &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_packi4_ (&bsdinfo_.usec, &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_packi4_ (&bsdinfo_.nbsds, &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  nobj = bsdinfo_.nbsds;
  rcode += dst_packi4_ (&bsdinfo_.xxyy[0], &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_packi4_ (&bsdinfo_.bitf[0], &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  nobj = 1;
  rcode += dst_packi4_ (&bsdinfo_.nsdsout, &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  nobj = bsdinfo_.nsdsout;
  rcode += dst_packi4_ (&bsdinfo_.xxyyout[0], &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_packi4_ (&bsdinfo_.bitfout[0], &nobj, bsdinfo_bank, &bsdinfo_blen, &bsdinfo_maxlen);
  return rcode;
}

integer4 bsdinfo_bank_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (NumUnit, &bsdinfo_blen, bsdinfo_bank);
  free (bsdinfo_bank);
  bsdinfo_bank = NULL;
  return rcode;
}

integer4 bsdinfo_common_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  if ((rcode = bsdinfo_common_to_bank_ ()))
    {
      fprintf (stderr, "bsdinfo_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  if ((rcode = bsdinfo_bank_to_dst_ (NumUnit)))
    {
      fprintf (stderr, "bsdinfo_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4 bsdinfo_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 bankid, bankversion;
  integer4 nobj;
  bsdinfo_blen = 0;  
  nobj = 1;
  rcode += dst_unpacki4_ (&bankid, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_unpacki4_ (&bankversion, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_unpacki4_ (&bsdinfo_.yymmdd, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_unpacki4_ (&bsdinfo_.hhmmss, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_unpacki4_ (&bsdinfo_.usec, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_unpacki4_ (&bsdinfo_.nbsds, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  nobj = bsdinfo_.nbsds;
  rcode += dst_unpacki4_ (&bsdinfo_.xxyy[0], &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  rcode += dst_unpacki4_ (&bsdinfo_.bitf[0], &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
  if(bankversion >= 1)
    {
      nobj = 1;
      rcode += dst_unpacki4_ (&bsdinfo_.nsdsout, &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
      nobj = bsdinfo_.nsdsout;
      rcode += dst_unpacki4_ (&bsdinfo_.xxyyout[0], &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
      if(bankversion >= 2)
	rcode += dst_unpacki4_ (&bsdinfo_.bitfout[0], &nobj, bank, &bsdinfo_blen, &bsdinfo_maxlen);
      else
	memset(&bsdinfo_.bitfout[0],0xFF,bsdinfo_.nsdsout*sizeof(integer4));
    }
  else
    bsdinfo_.nsdsout = 0;
  return rcode;
}

integer4 bsdinfo_common_to_dump_ (integer4 * long_output)
{
  return bsdinfo_common_to_dumpf_ (stdout, long_output);
}


static char* bsdinfo_binrep(int n, int nbits)
{
  int i=0;
  static char buf[8*sizeof(int)];
  if (nbits > (int)(8*sizeof(int)-1))
    {
      sprintf(buf,"error: nbits > %d (max)",(int)(8*sizeof(int)-1));
      return buf;
    }
  buf[nbits] = '\0';
  for (i=0; i < nbits; i++)
    buf[nbits-1-i] = (n & (1<<i) ? '1' : '0');
  return buf;
}

integer4 bsdinfo_common_to_dumpf_ (FILE * fp, integer4* long_output)
{

  int i;
  fprintf (fp, "%s :\n","bsdinfo");
  fprintf (fp, "%06d %06d.%06d nbsds %d nsdsout %d\n",
	   bsdinfo_.yymmdd,
	   bsdinfo_.hhmmss,
	   bsdinfo_.usec,
	   bsdinfo_.nbsds,
	   bsdinfo_.nsdsout);
  for (i=0; i<bsdinfo_.nbsds; i++)
    {
      fprintf(fp,"%06d %06d.%06d %04d %s -BAD\n",
	      bsdinfo_.yymmdd,bsdinfo_.hhmmss,bsdinfo_.usec,
	      bsdinfo_.xxyy[i],bsdinfo_binrep(bsdinfo_.bitf[i],BSDINFO_NBITS));	
    }
  if(*long_output == 1)
    {
      fprintf(fp,"Bit flag description (if certain bit is set,there's a problem):\n");
      fprintf(fp,"bit 0:  ICRR calibration issue, failed ICRR don't use criteria\n");
      fprintf(fp,"bit 1:  ICRR calibration issue, Mev2pe problem\n");
      fprintf(fp,"bit 2:  ICRR calibration issue, Mev2cnt problem\n");
      fprintf(fp,"bit 3:  ICRR calibration issue, bad pedestal mean values\n");
      fprintf(fp,"bit 4:  ICRR calibration issue, bad pedestal standard deviation\n");
      fprintf(fp,"bit 5:  ICRR calibration issue, saturation information not available\n"); 
      fprintf(fp,"bit 6:  Rutgers calibration issue, bad mip values\n");
      fprintf(fp,"bit 7:  Rutgers calibration issue, bad pedestal peak channel\n");
      fprintf(fp,"bit 8:  Rutgers calibration issue, bad pedestal right half peak channel\n");
      fprintf(fp,"bit 9:  Rutgers calibration issue, bad 1-MIP peak fit number of degrees of freedom\n");
      fprintf(fp,"bit 10: Rutgers calibration issue, bad 1-MIP peak fit chi2\n");
      fprintf(fp,"bit 11: Rutgers calibration issue, peak channel of pedestal histogram\n");
      fprintf(fp,"bit 12: Rutgers calibration issue, peak channel of 1-MIP histogram\n");
      fprintf(fp,"bit 13: Rutgers calibration issue, 1-MIP histogram fit number of degrees of freedom\n");
      fprintf(fp,"bit 14: Rutgers calibration issue, 1-MIP histogram chi2 / dof\n");
      fprintf(fp,"bit 15: Rutgers calibration issue, FADC counts per VEM\n");
    }
  for (i=0; i<bsdinfo_.nsdsout; i++)
    {
      fprintf(fp,"%06d %06d.%06d %04d %s -OUT\n",
	      bsdinfo_.yymmdd,
	      bsdinfo_.hhmmss,
	      bsdinfo_.usec,
	      bsdinfo_.xxyyout[i],
	      bsdinfo_binrep(bsdinfo_.bitfout[i],BSDINFO_NBITS));
    }
  return 0;
}
