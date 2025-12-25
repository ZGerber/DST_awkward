// Created 2010/01 LMS
// Last updated: 2017/12 DI

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"
#include "dst_sort.h"

#include "univ_dst.h"
#include "sttubeprofile_dst.h"

sttubeprofile_dst_common sttubeprofile_;

static integer4 sttubeprofile_blen = 0;
static integer4 sttubeprofile_maxlen = sizeof(integer4) * 2 + sizeof(sttubeprofile_dst_common);
static integer1 *sttubeprofile_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* sttubeprofile_bank_buffer_ (integer4* sttubeprofile_bank_buffer_size)
{
  (*sttubeprofile_bank_buffer_size) = sttubeprofile_blen;
  return sttubeprofile_bank;
}



static void sttubeprofile_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(sttubeprofile_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
      fprintf (stderr,"sttubeprofile_abank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

static void sttubeprofile_bank_init() {sttubeprofile_abank_init(&sttubeprofile_bank);}

integer4 sttubeprofile_common_to_bank_() {
  if (sttubeprofile_bank == NULL) sttubeprofile_bank_init();
  return sttubeprofile_struct_to_abank_(&sttubeprofile_, &sttubeprofile_bank, STTUBEPROFILE_BANKID, STTUBEPROFILE_BANKVERSION);
}
integer4 sttubeprofile_bank_to_dst_ (integer4 *unit) {return sttubeprofile_abank_to_dst_(sttubeprofile_bank, unit);}
integer4 sttubeprofile_common_to_dst_(integer4 *unit) {
  if (sttubeprofile_bank == NULL) sttubeprofile_bank_init();
  return sttubeprofile_struct_to_dst_(&sttubeprofile_, sttubeprofile_bank, unit, STTUBEPROFILE_BANKID, STTUBEPROFILE_BANKVERSION);
}
integer4 sttubeprofile_bank_to_common_(integer1 *bank) {return sttubeprofile_abank_to_struct_(bank, &sttubeprofile_);}
integer4 sttubeprofile_common_to_dump_(integer4 *opt) {return sttubeprofile_struct_to_dumpf_(&sttubeprofile_, stdout, opt);}
integer4 sttubeprofile_common_to_dumpf_(FILE* fp, integer4 *opt) {return sttubeprofile_struct_to_dumpf_(&sttubeprofile_, fp, opt);}

integer4 sttubeprofile_struct_to_abank_(sttubeprofile_dst_common *sttubeprofile, integer1 *(*pbank), integer4 id, integer4 ver) {
  integer4 rcode, nobj;
  integer1 *bank;

  int i;

  if (*pbank == NULL) sttubeprofile_abank_init(pbank);

  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &sttubeprofile_blen, &sttubeprofile_maxlen, bank);

// Initialize sttubeprofile_blen and pack the id and version to bank

  nobj = 2;
  rcode += dst_packi4_(&sttubeprofile->ntube[0],   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  nobj = 1;
  rcode	+= dst_packi4_(&sttubeprofile->ngtube,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode	+= dst_packi4_(&sttubeprofile->status,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  nobj = 2;
  rcode += dst_packr8_(&sttubeprofile->rp[0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->psi[0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->t0[0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&sttubeprofile->Xmax,    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->eXmax,   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->Nmax,    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->eNmax,   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->Energy,  &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->eEnergy, &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->chi2,    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  for (i=0; i<2; i++) {
    nobj = sttubeprofile->ntube[i];
    rcode += dst_packr8_(&sttubeprofile->x[i][0],         &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->npe[i][0],       &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->enpe[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->eacptfrac[i][0], &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->acpt[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->eacpt[i][0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->flux[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->eflux[i][0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->simnpe[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->nfl[i][0],       &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->ncvdir[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->ncvmie[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->ncvray[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->simflux[i][0],   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->ne[i][0],        &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->ene[i][0],       &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packr8_(&sttubeprofile->tres[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packr8_(&sttubeprofile->tchi2[i][0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_packi4_(&sttubeprofile->camera[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packi4_(&sttubeprofile->tube[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_packi4_(&sttubeprofile->tube_qual[i][0], &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  }

  nobj = 1;
  rcode += dst_packi4_(&sttubeprofile->mc,                &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  // new in version 1:
  rcode += dst_packr8_(&sttubeprofile->X0,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->eX0,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->Lambda,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_packr8_(&sttubeprofile->eLambda,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  
  nobj = 2;
  rcode += dst_packi4_(&sttubeprofile->siteid[0],   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  return rcode;
}

integer4 sttubeprofile_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &sttubeprofile_blen, bank);
}

integer4 sttubeprofile_struct_to_dst_(sttubeprofile_dst_common *sttubeprofile, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = sttubeprofile_struct_to_abank_(sttubeprofile, &bank, id, ver)) ) {
      fprintf(stderr, "sttubeprofile_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = sttubeprofile_abank_to_dst_(bank, unit)) ) {
      fprintf(stderr, "sttubeprofile_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 sttubeprofile_abank_to_struct_(integer1 *bank, sttubeprofile_dst_common *sttubeprofile) {
  integer4 rcode = 0 ;
  integer4 nobj = 1;
  int version;
  sttubeprofile_blen = 1 * sizeof(integer4);   /* skip id and version  */

  rcode += dst_unpacki4_(&version, &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  
  int i;

  nobj = 2;
  rcode += dst_unpacki4_(&sttubeprofile->ntube[0],   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  nobj = 1;
  rcode	+= dst_unpacki4_(&sttubeprofile->ngtube,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode	+= dst_unpacki4_(&sttubeprofile->status,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  nobj = 2;
  rcode += dst_unpackr8_(&sttubeprofile->rp[0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->psi[0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->t0[0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&sttubeprofile->Xmax,    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->eXmax,   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->Nmax,    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->eNmax,   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->Energy,  &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->eEnergy, &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  rcode += dst_unpackr8_(&sttubeprofile->chi2,    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  for (i=0; i<2; i++) {
    nobj = sttubeprofile->ntube[i];
    rcode += dst_unpackr8_(&sttubeprofile->x[i][0],         &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->npe[i][0],       &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->enpe[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->eacptfrac[i][0], &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->acpt[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->eacpt[i][0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->flux[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->eflux[i][0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->simnpe[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->nfl[i][0],       &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->ncvdir[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->ncvmie[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->ncvray[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->simflux[i][0],   &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->ne[i][0],        &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->ene[i][0],       &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpackr8_(&sttubeprofile->tres[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->tchi2[i][0],     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

    rcode += dst_unpacki4_(&sttubeprofile->camera[i][0],    &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpacki4_(&sttubeprofile->tube[i][0],      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpacki4_(&sttubeprofile->tube_qual[i][0], &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  }

  nobj = 1;
  rcode += dst_unpacki4_(&sttubeprofile->mc,        &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);

  // new in version 1:
  if (version >= 1) {
    rcode += dst_unpackr8_(&sttubeprofile->X0,      &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->eX0,     &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->Lambda,  &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    rcode += dst_unpackr8_(&sttubeprofile->eLambda, &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
    
    nobj = 2;
    rcode += dst_unpacki4_(&sttubeprofile->siteid[0], &nobj, bank, &sttubeprofile_blen, &sttubeprofile_maxlen);
  }
  else { // make some assumptions
    sttubeprofile_.X0 = -100;
    sttubeprofile_.eX0 = 0;
    sttubeprofile_.Lambda = 60;
    sttubeprofile_.eLambda = 0;
    sttubeprofile_.siteid[0] = 0;
    sttubeprofile_.siteid[1] = 1;
  }
  return rcode;
}

integer4 sttubeprofile_struct_to_dump_(sttubeprofile_dst_common *sttubeprofile, integer4 *long_output) {
  return sttubeprofile_struct_to_dumpf_(sttubeprofile, stdout, long_output);
}

integer4 sttubeprofile_struct_to_dumpf_(sttubeprofile_dst_common *sttubeprofile, FILE* fp, integer4 *long_output) {
  int i, j;
  int g[TA_UNIV_MAXTUBE];

  fprintf (fp, "\n\nSTTUBEPROFILE bank (TA FD shower profile information for Black Rock / Long Ridge FD)\n");
  if (sttubeprofile->mc == TRUE)
    fprintf (fp, "\nUses TRUMPMC geometry information\n\n");

  if      (sttubeprofile->status == -2)
    fprintf (fp, "\nBANK NOT FILLED : ");
  else if (sttubeprofile->status == -1)
    fprintf (fp, "\nBAD GEOMETRY : ");
  else if (sttubeprofile->status ==  0)
    fprintf (fp, "\nFIT DID NOT CONVERGE : ");
  else
    fprintf (fp, "\nFIT CONVERGED (status = %d ): ",sttubeprofile->status);
  fprintf (fp, "Total pmts : %d\n", sttubeprofile->ngtube);

  for (i=0; i<2; i++) {
    if (i == 0)
      fprintf (fp, "  BLACK ROCK : ");
    else if (i == 1)
      fprintf (fp, "  LONG RIDGE : ");
    fprintf (fp, "(Psi, Rp, t0) = (%8.3f, %8.1f, %8.1f), %3d pmts\n",
	     sttubeprofile->psi[i]*R2D, sttubeprofile->rp[i],
	     sttubeprofile->t0[i], sttubeprofile->ntube[i]);
  }
  fprintf (fp, "  Xmax   : %6.1f +/- %6.1f\n", sttubeprofile->Xmax, sttubeprofile->eXmax);
  fprintf (fp, "  Nmax   : %4.3e +/- %4.3e\n", sttubeprofile->Nmax, sttubeprofile->eNmax);
  fprintf (fp, "  X0     : %f +/- %f\n", sttubeprofile->X0, sttubeprofile->eX0);
  fprintf (fp, "  Lambda : %f +/- %f\n", sttubeprofile->Lambda, sttubeprofile->eLambda);
  fprintf (fp, "  Energy : %4.3e +/- %4.3e\n", sttubeprofile->Energy, sttubeprofile->eEnergy);
  fprintf (fp, "  Chi2   : %6.1f\n", sttubeprofile->chi2);

// Tube info
  if ( (*long_output) == 1) {
    if (sttubeprofile->status >= 0) {

      for (i=0; i<2; i++) {

	if (i == 0)
	  fprintf (fp, "\nTube information for BLACK ROCK\n");
	else if (i == 1)
	  fprintf (fp, "\nTube information for LONG RIDGE\n");

        dst_sort_real8 (sttubeprofile->ntube[i], sttubeprofile->x[i], g);

        fprintf (fp, " idx                            gram      npe     enpe   simnpe      flux     "
		     "eflux       nfl     cvdir     cvmie     cvray   simflux      tres  tchi2        "
		     "Ne       eNe  qual\n");
        for (j=0; j<sttubeprofile->ntube[i]; j++)
          fprintf (fp, "%4d [ cam %2d tube %3d ]   %9.4f %8.2f %8.2f %8.2f %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.2e %6.2f %9.3e %9.3e     %d\n", 
                   g[j], sttubeprofile->camera[i][g[j]], sttubeprofile->tube[i][g[j]], sttubeprofile->x[i][g[j]],
                   sttubeprofile->npe[i][g[j]], sttubeprofile->enpe[i][g[j]],
		   sttubeprofile->simnpe[i][g[j]], sttubeprofile->flux[i][g[j]]/R2D, sttubeprofile->eflux[i][g[j]]/R2D, 
                   sttubeprofile->nfl[i][g[j]]/R2D, sttubeprofile->ncvdir[i][g[j]]/R2D, sttubeprofile->ncvmie[i][g[j]]/R2D, 
	           sttubeprofile->ncvray[i][g[j]]/R2D, sttubeprofile->simflux[i][g[j]]/R2D, 
	           sttubeprofile->tres[i][g[j]], sttubeprofile->tchi2[i][g[j]],
                   sttubeprofile->ne[i][g[j]], sttubeprofile->ene[i][g[j]], sttubeprofile->tube_qual[i][g[j]]);
      }
    }
  }
  else
    fprintf (fp, "\nTube information not displayed in short output\n");

  fprintf (fp, "\n\n");

  return 0;
}
