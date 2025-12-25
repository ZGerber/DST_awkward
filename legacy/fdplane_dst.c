// Created 2008/09/23 DRB LMS
// Last updated: 2017/11 DI

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"
#include "dst_sort.h"

#include "univ_dst.h"
#include "fdplane_dst.h"
#include "caldat.h"

fdplane_dst_common fdplane_;

integer4 fdplane_blen = 0; /* not static because it needs to be accessed by the c files of the derived banks */
static integer4 fdplane_maxlen = sizeof(integer4) * 2 + sizeof(fdplane_dst_common);
static integer1 *fdplane_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* fdplane_bank_buffer_ (integer4* fdplane_bank_buffer_size)
{
  (*fdplane_bank_buffer_size) = fdplane_blen;
  return fdplane_bank;
}



static void fdplane_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(fdplane_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
      fprintf (stderr,"fdplane_abank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

static void fdplane_bank_init() {fdplane_abank_init(&fdplane_bank);}

integer4 fdplane_common_to_bank_() {
  if (fdplane_bank == NULL) fdplane_bank_init();
  return fdplane_struct_to_abank_(&fdplane_, &fdplane_bank, FDPLANE_BANKID, FDPLANE_BANKVERSION);
}
integer4 fdplane_bank_to_dst_ (integer4 *unit) {return fdplane_abank_to_dst_(fdplane_bank, unit);}
integer4 fdplane_common_to_dst_(integer4 *unit) {
  if (fdplane_bank == NULL) fdplane_bank_init();
  return fdplane_struct_to_dst_(&fdplane_, fdplane_bank, unit, FDPLANE_BANKID, FDPLANE_BANKVERSION);
}
integer4 fdplane_bank_to_common_(integer1 *bank) {return fdplane_abank_to_struct_(bank, &fdplane_);}
integer4 fdplane_common_to_dump_(integer4 *opt) {return fdplane_struct_to_dumpf_(&fdplane_, stdout, opt);}
integer4 fdplane_common_to_dumpf_(FILE* fp, integer4 *opt) {return fdplane_struct_to_dumpf_(&fdplane_, fp, opt);}

integer4 fdplane_struct_to_abank_(fdplane_dst_common *fdplane, integer1 *(*pbank), integer4 id, integer4 ver) {
  integer4 rcode, nobj, i;
  integer1 *bank;

  if (*pbank == NULL) fdplane_abank_init(pbank);

  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &fdplane_blen, &fdplane_maxlen, bank);

// Initialize fdplane_blen and pack the id and version to bank

  nobj = 1;
  rcode += dst_packi4_(&fdplane->part,      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->event_num, &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->julian,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->jsecond,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->jsecfrac,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->second,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->secfrac,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->ntube,     &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->uniqID,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->fmode,     &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  nobj = fdplane->ntube;
  rcode += dst_packr8_(&fdplane->npe[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->adc[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->ped[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->time[0],      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->time_rms[0],  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->sigma[0],      &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_packr8_(&fdplane->sdp_n[0],  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->sdp_en[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_packr8_(&fdplane->sdp_n_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&fdplane->sdp_the,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->sdp_phi,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->sdp_chi2, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_packr8_(&fdplane->alt[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->azm[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->plane_alt[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->plane_azm[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&fdplane->linefit_slope,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->linefit_eslope, &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->linefit_int,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->linefit_eint,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->linefit_chi2,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 2;
  for (i=0; i<2; i++)
    rcode += dst_packr8_(&fdplane->linefit_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_packr8_(&fdplane->linefit_res[0],   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->linefit_tchi2[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&fdplane->ptanfit_rp,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->ptanfit_erp,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->ptanfit_t0,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->ptanfit_et0,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->ptanfit_chi2, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 2;
  for (i=0; i<2; i++)
    rcode += dst_packr8_(&fdplane->ptanfit_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_packr8_(&fdplane->ptanfit_res[0],   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->ptanfit_tchi2[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&fdplane->rp,          &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->erp,         &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->psi,         &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->epsi,        &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->t0,          &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->et0,         &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->tanfit_chi2, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_packr8_(&fdplane->tanfit_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_packr8_(&fdplane->tanfit_res[0],   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->tanfit_tchi2[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&fdplane->azm_extent,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->time_extent,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  rcode += dst_packr8_(&fdplane->shower_zen, &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packr8_(&fdplane->shower_azm, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_packr8_(&fdplane->shower_axis[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_packr8_(&fdplane->rpuv[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_packr8_(&fdplane->core[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_packi4_(&fdplane->camera[0],    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->tube[0],      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->it0[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->it1[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->knex_qual[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->tube_qual[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_packi4_(&fdplane->ngtube,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->seed,      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->type,      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->status,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_packi4_(&fdplane->siteid,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  return rcode;
}

integer4 fdplane_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &fdplane_blen, bank);
}

integer4 fdplane_struct_to_dst_(fdplane_dst_common *fdplane, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = fdplane_struct_to_abank_(fdplane, &bank, id, ver)) ) {
      fprintf(stderr, "fdplane_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = fdplane_abank_to_dst_(bank, unit)) ) {
      fprintf(stderr, "fdplane_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 fdplane_abank_to_struct_(integer1 *bank, fdplane_dst_common *fdplane) {
  integer4 rcode = 0 ;
  integer4 nobj, i;
  
  integer4 ver;
  
  fdplane_blen = 1 * sizeof(integer4);   /* skip id and version  */

  nobj = 1;
  rcode += dst_unpacki4_(&ver, &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  if (ver == 0)
    fprintf(stderr,"Warning: old FDPLANE version (%d) may not contain accurate history.\n",ver);
  
  rcode += dst_unpacki4_(&fdplane->part,      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->event_num, &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->julian,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->jsecond,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->jsecfrac,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->second,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->secfrac,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->ntube,     &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  if (ver >= 1) {
    rcode += dst_unpacki4_(&fdplane->uniqID, &nobj, bank, &fdplane_blen,
                           &fdplane_maxlen);
    rcode += dst_unpacki4_(&fdplane->fmode, &nobj, bank, &fdplane_blen,
                           &fdplane_maxlen);
  }
  else {
    fdplane->uniqID = -1;
    fdplane->fmode = 0;
  }
  
  
  nobj = fdplane->ntube;
  rcode += dst_unpackr8_(&fdplane->npe[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  if (ver >= 2)
    {
      rcode += dst_unpackr8_(&fdplane->adc[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
      rcode += dst_unpackr8_(&fdplane->ped[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
    }
  else
    {
      for (i=0; i<GEOFD_MAXTUBE; i++)
	{
	  fdplane->adc[i] = 0;
	  fdplane->ped[i] = 0;
	}
    }
  rcode += dst_unpackr8_(&fdplane->time[0],      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->time_rms[0],  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->sigma[0],     &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_unpackr8_(&fdplane->sdp_n[0],  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->sdp_en[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_unpackr8_(&fdplane->sdp_n_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&fdplane->sdp_the,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->sdp_phi,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->sdp_chi2, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_unpackr8_(&fdplane->alt[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->azm[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->plane_alt[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->plane_azm[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&fdplane->linefit_slope,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->linefit_eslope, &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->linefit_int,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->linefit_eint,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->linefit_chi2,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 2;
  for (i=0; i<2; i++)
    rcode += dst_unpackr8_(&fdplane->linefit_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_unpackr8_(&fdplane->linefit_res[0],   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->linefit_tchi2[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&fdplane->ptanfit_rp,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->ptanfit_erp,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->ptanfit_t0,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->ptanfit_et0,  &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->ptanfit_chi2, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 2;
  for (i=0; i<2; i++)
    rcode += dst_unpackr8_(&fdplane->ptanfit_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_unpackr8_(&fdplane->ptanfit_res[0],   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->ptanfit_tchi2[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&fdplane->rp,          &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->erp,         &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->psi,         &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->epsi,        &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->t0,          &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->et0,         &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->tanfit_chi2, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_unpackr8_(&fdplane->tanfit_cov[i][0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_unpackr8_(&fdplane->tanfit_res[0],   &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->tanfit_tchi2[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&fdplane->azm_extent,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpackr8_(&fdplane->time_extent,   &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  rcode += dst_unpackr8_(&fdplane->shower_zen, &nobj, bank, &fdplane_blen, &fdplane_maxlen);             
  rcode += dst_unpackr8_(&fdplane->shower_azm, &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_unpackr8_(&fdplane->shower_axis[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_unpackr8_(&fdplane->rpuv[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 3;
  rcode += dst_unpackr8_(&fdplane->core[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = fdplane->ntube;
  rcode += dst_unpacki4_(&fdplane->camera[0],    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->tube[0],      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->it0[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->it1[0],       &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->knex_qual[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->tube_qual[0], &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  nobj = 1;
  rcode += dst_unpacki4_(&fdplane->ngtube,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->seed,      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->type,      &nobj, bank, &fdplane_blen, &fdplane_maxlen);
  rcode += dst_unpacki4_(&fdplane->status,    &nobj, bank, &fdplane_blen, &fdplane_maxlen); 
  rcode += dst_unpacki4_(&fdplane->siteid,    &nobj, bank, &fdplane_blen, &fdplane_maxlen);

  return rcode;
}

integer4 fdplane_struct_to_dump_(fdplane_dst_common *fdplane, integer4 *long_output) {
  return fdplane_struct_to_dumpf_(fdplane, stdout, long_output);
}

integer4 fdplane_struct_to_dumpf_(fdplane_dst_common *fdplane, FILE* fp, integer4 *long_output) {
  int i;
  integer4 yr=0,mo=0,day=0;
  integer4 hr, min, sec, nano;

  hr = fdplane->jsecond / 3600 + 12;

  if (hr >= 24) {
    caldat((double)fdplane->julian+1., &mo, &day, &yr);
    hr -= 24;
  }
  else
    caldat((double)fdplane->julian, &mo, &day, &yr);

  min = ( fdplane->jsecond / 60 ) % 60;
  sec = fdplane->jsecond % 60;
  nano = fdplane->jsecfrac;

  int t[TA_UNIV_MAXTUBE];

  if (fdplane->siteid == BR)
    fprintf (fp, "\n\nBRPLANE bank (TA plane- and time-fit data for Black Rock Mesa FD)\n");
  else if (fdplane->siteid == LR)
    fprintf (fp, "\n\nLRPLANE bank (TA plane- and time-fit data for Long Ridge FD)\n");
  else if (fdplane->siteid == TL)
    fprintf (fp, "\n\nTLPLANE bank (TA plane- and time-fit data for TALE FD)\n");
  
  if (fdplane->uniqID != -1) 
    fprintf (fp, "Processed with GEOFD UniqID %d and filter mode %d.\n\n",
             fdplane->uniqID, fdplane->fmode);
  
  fprintf (fp, "%4d/%02d/%02d %02d:%02d:%02d.%09d | Part %6d Event %6d\t", 
    yr, mo, day, hr, min, sec, nano, fdplane->part, fdplane->event_num);
  if      (fdplane->type == 2)
    fprintf (fp, "downward-going event\n");
  else if (fdplane->type == 3)
    fprintf (fp, "  upward-going event\n");
  else if (fdplane->type == 4)
    fprintf (fp, "       in-time event\n");
  else if (fdplane->type == 5)
    fprintf (fp, "         noise event\n");
  fprintf (fp, "Run start     : %9d %5d.%09d\n", fdplane->julian, fdplane->jsecond, fdplane->jsecfrac);
  fprintf (fp, "Event Start   : %5d.%09d\n", fdplane->second, fdplane->secfrac);
  fprintf (fp, "Number of tubes                   : %4d\n", fdplane->ntube);
  fprintf (fp, "Number of tubes in fit            : %4d\n", fdplane->ngtube);
  fprintf (fp, "Seed for track                    : %4d [ cam %2d tube %3d ]\n\n", 
    fdplane->seed, fdplane->camera[fdplane->seed], fdplane->tube[fdplane->seed]);

  fprintf (fp, "Norm vector to SDP ( chi2 = %7.4f )\n", fdplane->sdp_chi2);
  fprintf (fp, "  nx        %9.6f +/- %9.6f\n", fdplane->sdp_n[0], fdplane->sdp_en[0]);
  fprintf (fp, "  ny        %9.6f +/- %9.6f\n", fdplane->sdp_n[1], fdplane->sdp_en[1]);
  fprintf (fp, "  nz        %9.6f +/- %9.6f\n", fdplane->sdp_n[2], fdplane->sdp_en[2]);
  fprintf (fp, "covariance: %9.6f %9.6f %9.6f\n",
    fdplane->sdp_n_cov[0][0], fdplane->sdp_n_cov[0][1], fdplane->sdp_n_cov[0][2]);
  fprintf (fp, "            %9.6f %9.6f %9.6f\n",
    fdplane->sdp_n_cov[1][0], fdplane->sdp_n_cov[1][1], fdplane->sdp_n_cov[1][2]);
  fprintf (fp, "            %9.6f %9.6f %9.6f\n\n",
    fdplane->sdp_n_cov[2][0], fdplane->sdp_n_cov[2][1], fdplane->sdp_n_cov[2][2]);
  fprintf (fp, "  SDP theta, phi: %f %f\n",fdplane->sdp_the * R2D,fdplane->sdp_phi * R2D);
  
  fprintf (fp, "Angular extent (degrees)           : %7.4f\n", R2D*fdplane->azm_extent);
  fprintf (fp, "Duration (ns)                      : %7.4f\n", fdplane->time_extent);
  fprintf (fp, "Shower zenith, azimuth             : %7.4f %7.4f\n",
    R2D*fdplane->shower_zen, R2D*fdplane->shower_azm);
  fprintf (fp, "Shower axis vector                 : %9.6f %9.6f %9.6f\n", 
    fdplane->shower_axis[0], fdplane->shower_axis[1], fdplane->shower_axis[2]);
  fprintf (fp, "Rp unit vector                     : %9.6f %9.6f %9.6f\n",
    fdplane->rpuv[0], fdplane->rpuv[1], fdplane->rpuv[2]);
  fprintf (fp, "Shower core location (site coords) : %9.2f %9.2f %9.2f\n\n",
    fdplane->core[0], fdplane->core[1], fdplane->core[2]);

  fprintf (fp, "time-fit status [linear][pseudotangent][tangent] = %03d\n\n", fdplane->status);

  fprintf (fp, "Linear fit : ");
  if ( (fdplane->status/100) )
    fprintf (fp, "GOOD\n");
  else
    fprintf (fp, "BAD\n");
  fprintf (fp, "Linear fit         ( chi2 = %7.4f )\n", fdplane->linefit_chi2);
  fprintf (fp, "int   : %10.3f +/- %10.3f (ns)\n", fdplane->linefit_int, fdplane->linefit_eint);
  fprintf (fp, "slope : %10.3f +/- %10.3f (ns/deg)\n", D2R*fdplane->linefit_slope, D2R*fdplane->linefit_eslope);
  fprintf (fp, "covariance: %10.3f %10.3f\n", fdplane->linefit_cov[0][0], fdplane->linefit_cov[0][1]);
  fprintf (fp, "            %10.3f %10.3f\n\n", fdplane->linefit_cov[1][0], fdplane->linefit_cov[1][1]);

  fprintf (fp, "Pseudo-tangent fit : ");
  if ( ((fdplane->status%100)/10) )
    fprintf (fp, "GOOD\n");
  else
    fprintf (fp, "BAD\n");
  fprintf (fp, "Pseudo-tangent fit ( chi2 = %7.4f )\n", fdplane->ptanfit_chi2);
  fprintf (fp, "Rp    : %10.3f +/- %10.3f (m)\n", fdplane->ptanfit_rp, fdplane->ptanfit_erp);
  fprintf (fp, "T0    : %10.3f +/- %10.3f (ns)\n", fdplane->ptanfit_t0, fdplane->ptanfit_et0);
  fprintf (fp, "covariance: %10.3f %10.3f\n", fdplane->ptanfit_cov[0][0], fdplane->ptanfit_cov[0][1]);
  fprintf (fp, "            %10.3f %10.3f\n\n", fdplane->ptanfit_cov[1][0], fdplane->ptanfit_cov[1][1]);

  fprintf (fp, "Tangent fit : ");
  if ( (fdplane->status%10) )
    fprintf (fp, "GOOD\n");
  else
    fprintf (fp, "BAD\n");
  fprintf (fp, "Tangent fit        ( chi2 = %7.4f )\n", fdplane->tanfit_chi2);
  fprintf (fp, "Rp    : %10.3f +/- %10.3f (m)\n", fdplane->rp, fdplane->erp);
  fprintf (fp, "Psi   : %10.3f +/- %10.3f (degrees)\n", R2D*fdplane->psi, R2D*fdplane->epsi);
  fprintf (fp, "T0    : %10.3f +/- %10.3f (ns)\n", fdplane->t0, fdplane->et0);
  fprintf (fp, "covariance: %10.3f %10.3f %10.3f\n",
    fdplane->tanfit_cov[0][0], fdplane->tanfit_cov[0][1], fdplane->tanfit_cov[0][2]);
  fprintf (fp, "            %10.3f %10.3f %10.3f\n",
    fdplane->tanfit_cov[1][0], fdplane->tanfit_cov[1][1], fdplane->tanfit_cov[1][2]);
  fprintf (fp, "            %10.3f %10.3f %10.3f\n\n",
    fdplane->tanfit_cov[2][0], fdplane->tanfit_cov[2][1], fdplane->tanfit_cov[2][2]);

// Tube info
  if ( (*long_output) == 1) {

    dst_sort_real8 (fdplane->ntube, fdplane->time, t);

    fprintf (fp, "Time-ordered tube information:\n");
    fprintf (fp, "indx                            npe       time       trms      alt      azm     palt     pazm        res         chi2   sigma knex    qual  it0 it1\n");
    for (i=0; i<fdplane->ntube; i++) {
      fprintf (fp, "%4d [ cam %2d tube %3d ] %10.3f %10.3f %10.3f %8.3f %8.3f %8.3f %8.3f %10.3f %12.3f %7.3f    %d  ",
        t[i], fdplane->camera[t[i]], fdplane->tube[t[i]], fdplane->npe[t[i]], fdplane->time[t[i]],
        fdplane->time_rms[t[i]], R2D*fdplane->alt[t[i]], R2D*fdplane->azm[t[i]], R2D*fdplane->plane_alt[t[i]], R2D*fdplane->plane_azm[t[i]], 
        fdplane->tanfit_res[t[i]], fdplane->tanfit_tchi2[t[i]], fdplane->sigma[t[i]], fdplane->knex_qual[t[i]]);
      if (fdplane->tube_qual[t[i]] == 1)
        fprintf (fp, "%6d  ", fdplane->tube_qual[t[i]]);
      else
        fprintf (fp, "-%05d  ", -fdplane->tube_qual[t[i]]);

      fprintf(fp, "%3d %3d\n", fdplane->it0[t[i]], fdplane->it1[t[i]]);
    }
  }
  else
    fprintf (fp, "Tube information not displayed in short output\n");

  fprintf (fp, "\n\n");

  return 0;
}
