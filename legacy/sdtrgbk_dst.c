/*
 *  C functions for sdtrgbk
 *  Dmitri Ivanov, <ivanov@physics.rutgers.edu>
 *  Jan 12, 2009
 *  Last Modified Feb 18, 2010
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "sdtrgbk_dst.h"

#ifdef __cplusplus
extern "C" integer4 eventNameFromId(integer4 bank_id, integer1 *name, integer4 len);
#else
integer4 eventNameFromId(integer4 bank_id, integer1 *name, integer4 len);
#endif

sdtrgbk_dst_common sdtrgbk_; /* allocate memory to sdtrgbk_common */

static integer4 sdtrgbk_blen = 0;
static integer4 sdtrgbk_maxlen = sizeof(integer4) * 2 + sizeof(sdtrgbk_dst_common);
static integer1 *sdtrgbk_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* sdtrgbk_bank_buffer_ (integer4* sdtrgbk_bank_buffer_size)
{
  (*sdtrgbk_bank_buffer_size) = sdtrgbk_blen;
  return sdtrgbk_bank;
}



static void sdtrgbk_bank_init()
{
  sdtrgbk_bank = (integer1 *) calloc(sdtrgbk_maxlen, sizeof(integer1));
  if (sdtrgbk_bank == NULL)
    {
      fprintf(stderr, "sdtrgbk_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    } /* else fprintf ( stderr,"sdtrgbk_bank allocated memory %d\n",sdtrgbk_maxlen); */
}

integer4 sdtrgbk_common_to_bank_()
{
  static integer4 id = SDTRGBK_BANKID, ver = SDTRGBK_BANKVERSION;
  integer4 rcode, nobj;
  int i, j;

  if (sdtrgbk_bank == NULL)
    sdtrgbk_bank_init();

  rcode = dst_initbank_(&id, &ver, &sdtrgbk_blen, &sdtrgbk_maxlen, sdtrgbk_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;
  rcode += dst_packi4_(&sdtrgbk_.raw_bankid, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.nsd, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.n_bad_ped, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.n_spat_cont, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.n_isol, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.n_pot_st_cont, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.n_l1_tg, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.dec_ped, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.inc_ped, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi1_(&sdtrgbk_.trigp, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi1_(&sdtrgbk_.igevent, &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  nobj = 3;
  rcode += dst_packi2_(&sdtrgbk_.il2sd[0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.il2sd_sig[0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  nobj = sdtrgbk_.nsd;
  rcode += dst_packi2_(&sdtrgbk_.xxyy[0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.wfindex_cal[0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi2_(&sdtrgbk_.nl1[0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_packi1_(&sdtrgbk_.ig[0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  for (i = 0; i < sdtrgbk_.nsd; i++)
    {
      nobj = sdtrgbk_.nl1[i];
      rcode += dst_packr8_(&sdtrgbk_.secf[i][0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
      rcode += dst_packi2_(&sdtrgbk_.ich[i][0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
      nobj = 2;
      rcode += dst_packr8_(&sdtrgbk_.tlim[i][0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
      for (j = 0; j < sdtrgbk_.nl1[i]; j++)
        rcode += dst_packi2_(&sdtrgbk_.q[i][j][0], &nobj, sdtrgbk_bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
    }
  return rcode;
}

integer4 sdtrgbk_bank_to_dst_(integer4 * NumUnit)
{
  integer4 rcode;
  rcode = dst_write_bank_(NumUnit, &sdtrgbk_blen, sdtrgbk_bank);
  free(sdtrgbk_bank);
  sdtrgbk_bank = NULL;
  return rcode;
}

integer4 sdtrgbk_common_to_dst_(integer4 * NumUnit)
{
  integer4 rcode;
  if ((rcode = sdtrgbk_common_to_bank_()))
    {
      fprintf(stderr, "sdtrgbk_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit(0);
    }
  if ((rcode = sdtrgbk_bank_to_dst_(NumUnit)))
    {
      fprintf(stderr, "sdtrgbk_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit(0);
    }
  return 0;
}

integer4 sdtrgbk_bank_to_common_(integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj;
  sdtrgbk_blen = 2 * sizeof(integer4); /* skip id and version  */
  int i, j;
  nobj = 1;
  rcode += dst_unpacki4_(&sdtrgbk_.raw_bankid, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.nsd, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.n_bad_ped, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.n_spat_cont, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.n_isol, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.n_pot_st_cont, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.n_l1_tg, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.dec_ped, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.inc_ped, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki1_(&sdtrgbk_.trigp, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki1_(&sdtrgbk_.igevent, &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  nobj = 3;
  rcode += dst_unpacki2_(&sdtrgbk_.il2sd[0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.il2sd_sig[0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  nobj = sdtrgbk_.nsd;
  rcode += dst_unpacki2_(&sdtrgbk_.xxyy[0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.wfindex_cal[0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki2_(&sdtrgbk_.nl1[0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  rcode += dst_unpacki1_(&sdtrgbk_.ig[0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
  for (i = 0; i < sdtrgbk_.nsd; i++)
    {
      nobj = sdtrgbk_.nl1[i];
      rcode += dst_unpackr8_(&sdtrgbk_.secf[i][0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
      rcode += dst_unpacki2_(&sdtrgbk_.ich[i][0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
      nobj = 2;
      rcode += dst_unpackr8_(&sdtrgbk_.tlim[i][0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
      for (j = 0; j < sdtrgbk_.nl1[i]; j++)
        rcode += dst_unpacki2_(&sdtrgbk_.q[i][j][0], &nobj, bank, &sdtrgbk_blen, &sdtrgbk_maxlen);
    }

  return rcode;
}

integer4 sdtrgbk_common_to_dump_(integer4 * long_output)
{
  return sdtrgbk_common_to_dumpf_(stdout, long_output);
}

integer4 sdtrgbk_common_to_dumpf_(FILE * fp, integer4 * long_output)
{
  fprintf(fp, "%s :\n", "sdtrgbk");
  int i, isd, isig;
  char bname[0x100];
  int bname_len=sizeof(bname);
  fprintf(fp,"\n");
  eventNameFromId(sdtrgbk_.raw_bankid,bname,bname_len);
  fprintf(fp, "raw waveform bank used: %s\n",bname);
  fprintf(fp, "igevent %d", (int) sdtrgbk_.igevent);
  fprintf(fp, " trigp %d", sdtrgbk_.trigp);
  fprintf(fp, " dec_ped %d", sdtrgbk_.dec_ped);
  fprintf(fp, " inc_ped %d", sdtrgbk_.inc_ped);
  fprintf(fp, " nsd %d", sdtrgbk_.nsd);
  fprintf(fp, " n_bad_ped %d", sdtrgbk_.n_bad_ped);
  fprintf(fp, " n_isol %d",sdtrgbk_.n_isol);
  fprintf(fp, " n_spat_cont %d",sdtrgbk_.n_spat_cont);
  fprintf(fp, " n_pot_st_cont %d",sdtrgbk_.n_pot_st_cont);
  fprintf(fp, " n_l1_tg %d",sdtrgbk_.n_l1_tg);
  // If event has triggered, print out the information
  // from the counters that took part in the event trigger
  if (sdtrgbk_.igevent>=1)
    {
      fprintf(fp," L2TRIG:");
      for (i=0; i<3; i++)
	{
	  isd=sdtrgbk_.il2sd[i];
	  isig=sdtrgbk_.il2sd_sig[i];
	  fprintf(fp," xxyy %04d",sdtrgbk_.xxyy[isd]);
	  fprintf(fp," secf %.6f",sdtrgbk_.secf[isd][isig]);
	  fprintf(fp," Ql %d",sdtrgbk_.q[isd][isig][0]);
	  fprintf(fp," Qu %d",sdtrgbk_.q[isd][isig][1]);
	}
    }
  fprintf(fp, "\n\n");
  
  if ((*long_output)==1)
    {
      fprintf(fp, "%3s \t %4s \t %2s \t %6s \n", "isd", "xxyy", "ig", "nl1sig"); 
      for (i = 0; i < sdtrgbk_.nsd; i++)
	fprintf(fp, "%3d \t %04d \t %2d \t %4d \n", i, sdtrgbk_.xxyy[i], (int) sdtrgbk_.ig[i], sdtrgbk_.nl1[i]);
    }
  
  return 0;
}
