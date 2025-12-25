// Created 2010/01 LMS
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
#include "fdtubeprofile_dst.h"

fdtubeprofile_dst_common fdtubeprofile_;

integer4 fdtubeprofile_blen = 0; /* not static because it needs to be accessed by the c files of the derived banks */
static integer4 fdtubeprofile_maxlen = sizeof(integer4) * 2 + sizeof(fdtubeprofile_dst_common);
static integer1 *fdtubeprofile_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* fdtubeprofile_bank_buffer_ (integer4* fdtubeprofile_bank_buffer_size)
{
  (*fdtubeprofile_bank_buffer_size) = fdtubeprofile_blen;
  return fdtubeprofile_bank;
}



static void fdtubeprofile_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(fdtubeprofile_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
      fprintf (stderr,"fdtubeprofile_abank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

static void fdtubeprofile_bank_init() {fdtubeprofile_abank_init(&fdtubeprofile_bank);}

integer4 fdtubeprofile_common_to_bank_() {
  if (fdtubeprofile_bank == NULL) fdtubeprofile_bank_init();
  return fdtubeprofile_struct_to_abank_(&fdtubeprofile_, &fdtubeprofile_bank, FDTUBEPROFILE_BANKID, FDTUBEPROFILE_BANKVERSION);
}
integer4 fdtubeprofile_bank_to_dst_ (integer4 *unit) {return fdtubeprofile_abank_to_dst_(fdtubeprofile_bank, unit);}
integer4 fdtubeprofile_common_to_dst_(integer4 *unit) {
  if (fdtubeprofile_bank == NULL) fdtubeprofile_bank_init();
  return fdtubeprofile_struct_to_dst_(&fdtubeprofile_, fdtubeprofile_bank, unit, FDTUBEPROFILE_BANKID, FDTUBEPROFILE_BANKVERSION);
}
integer4 fdtubeprofile_bank_to_common_(integer1 *bank) {return fdtubeprofile_abank_to_struct_(bank, &fdtubeprofile_);}
integer4 fdtubeprofile_common_to_dump_(integer4 *opt) {return fdtubeprofile_struct_to_dumpf_(&fdtubeprofile_, stdout, opt);}
integer4 fdtubeprofile_common_to_dumpf_(FILE* fp, integer4 *opt) {return fdtubeprofile_struct_to_dumpf_(&fdtubeprofile_, fp, opt);}

integer4 fdtubeprofile_struct_to_abank_(fdtubeprofile_dst_common *fdtubeprofile, integer1 *(*pbank), integer4 id, integer4 ver) {
  integer4 rcode, nobj;
  integer1 *bank;

  int i;

  if (*pbank == NULL) fdtubeprofile_abank_init(pbank);

  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &fdtubeprofile_blen, &fdtubeprofile_maxlen, bank);

// Initialize fdtubeprofile_blen and pack the id and version to bank

  nobj = 1;
  rcode += dst_packi4_(&fdtubeprofile->ntube,      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  nobj = 3;
  rcode	+= dst_packi4_(&fdtubeprofile->ngtube[0],  &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  rcode += dst_packr8_(&fdtubeprofile->rp[0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->psi[0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->t0[0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  rcode += dst_packr8_(&fdtubeprofile->Xmax[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->eXmax[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->Nmax[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->eNmax[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->Energy[0],  &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->eEnergy[0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->chi2[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  rcode += dst_packr8_(&fdtubeprofile->X0[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->eX0[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->Lambda[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packr8_(&fdtubeprofile->eLambda[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  for (i=0; i<3; i++) {
    nobj = fdtubeprofile->ntube;
    rcode += dst_packr8_(&fdtubeprofile->x[i][0],         &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->npe[i][0],       &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->enpe[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->eacptfrac[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->acpt[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->eacpt[i][0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->flux[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->eflux[i][0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->simnpe[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->nfl[i][0],       &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->ncvdir[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->ncvmie[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->ncvray[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->simflux[i][0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->ne[i][0],        &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->ene[i][0],       &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_packr8_(&fdtubeprofile->tres[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->tchi2[i][0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  }

  nobj = fdtubeprofile->ntube;
  rcode += dst_packi4_(&fdtubeprofile->camera[0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packi4_(&fdtubeprofile->tube[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  for (i=0; i<3; i++) {
    nobj = fdtubeprofile->ntube;
    rcode += dst_packi4_(&fdtubeprofile->tube_qual[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  }

  nobj = 3;
  rcode += dst_packi4_(&fdtubeprofile->status[0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  nobj = 1;
  rcode += dst_packi4_(&fdtubeprofile->siteid,    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_packi4_(&fdtubeprofile->mc,        &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  for (i=0; i<3; i++) {
    nobj = fdtubeprofile->ntube;
    rcode += dst_packr8_(&fdtubeprofile->simtime[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->simtrms[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->simtres[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_packr8_(&fdtubeprofile->timechi2[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  }
  return rcode;
}

integer4 fdtubeprofile_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &fdtubeprofile_blen, bank);
}

integer4 fdtubeprofile_struct_to_dst_(fdtubeprofile_dst_common *fdtubeprofile, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = fdtubeprofile_struct_to_abank_(fdtubeprofile, &bank, id, ver)) ) {
      fprintf(stderr, "fdtubeprofile_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = fdtubeprofile_abank_to_dst_(bank, unit)) ) {
      fprintf(stderr, "fdtubeprofile_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 fdtubeprofile_abank_to_struct_(integer1 *bank, fdtubeprofile_dst_common *fdtubeprofile) {
  integer4 rcode = 0 ;
  integer4 nobj;

  int i;

  fdtubeprofile_blen = 0;

  nobj = 1;

  /* check id and version */
  int id, ver;
  rcode += dst_unpacki4_(&id, &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpacki4_(&ver, &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  
  nobj = 1;
  rcode += dst_unpacki4_(&fdtubeprofile->ntube,      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  nobj = 3;
  rcode	+= dst_unpacki4_(&fdtubeprofile->ngtube[0],  &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  rcode += dst_unpackr8_(&fdtubeprofile->rp[0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->psi[0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->t0[0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  rcode += dst_unpackr8_(&fdtubeprofile->Xmax[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->eXmax[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->Nmax[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->eNmax[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->Energy[0],  &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->eEnergy[0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpackr8_(&fdtubeprofile->chi2[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  if ( ver >= 2 ) {
    rcode += dst_unpackr8_(&fdtubeprofile->X0[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->eX0[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->Lambda[0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->eLambda[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  }

  for (i=0; i<3; i++) {
    nobj = fdtubeprofile->ntube;
    rcode += dst_unpackr8_(&fdtubeprofile->x[i][0],         &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->npe[i][0],       &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->enpe[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->eacptfrac[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->acpt[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->eacpt[i][0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->flux[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->eflux[i][0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->simnpe[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->nfl[i][0],       &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->ncvdir[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->ncvmie[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->ncvray[i][0],    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->simflux[i][0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->ne[i][0],        &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->ene[i][0],       &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

    rcode += dst_unpackr8_(&fdtubeprofile->tres[i][0],      &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    rcode += dst_unpackr8_(&fdtubeprofile->tchi2[i][0],     &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  }

  nobj = fdtubeprofile->ntube;
  rcode += dst_unpacki4_(&fdtubeprofile->camera[0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpacki4_(&fdtubeprofile->tube[0],   &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  for (i=0; i<3; i++) {
    nobj = fdtubeprofile->ntube;
    rcode += dst_unpacki4_(&fdtubeprofile->tube_qual[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  }

  nobj = 3;
  rcode += dst_unpacki4_(&fdtubeprofile->status[0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  nobj = 1;
  rcode += dst_unpacki4_(&fdtubeprofile->siteid,    &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
  rcode += dst_unpacki4_(&fdtubeprofile->mc,        &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);

  if ( ver >= 3 ) {
    for (i=0; i<3; i++) {
      nobj = fdtubeprofile->ntube;
      rcode += dst_unpackr8_(&fdtubeprofile->simtime[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
      rcode += dst_unpackr8_(&fdtubeprofile->simtrms[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
      rcode += dst_unpackr8_(&fdtubeprofile->simtres[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
      rcode += dst_unpackr8_(&fdtubeprofile->timechi2[i][0], &nobj, bank, &fdtubeprofile_blen, &fdtubeprofile_maxlen);
    }
  }
  return rcode;
}

integer4 fdtubeprofile_struct_to_dump_(fdtubeprofile_dst_common *fdtubeprofile, integer4 *long_output) {
  return fdtubeprofile_struct_to_dumpf_(fdtubeprofile, stdout, long_output);
}

integer4 fdtubeprofile_struct_to_dumpf_(fdtubeprofile_dst_common *fdtubeprofile, FILE* fp, integer4 *long_output) {
  int i, j;
  int g[TA_UNIV_MAXTUBE];

  if (fdtubeprofile->siteid == BR)
    fprintf (fp, "\n\nBRTUBEPROFILE bank (TA FD shower profile information for Black Rock FD)\n");
  else if (fdtubeprofile->siteid == LR)
    fprintf (fp, "\n\nLRTUBEPROFILE bank (TA FD shower profile information for Long Ridge FD)\n");
  if (fdtubeprofile->mc == TRUE)
    fprintf (fp, "\nUses TRUMPMC geometry information\n\n");
  
  for (i=0; i<3; i++) {
    if      (fdtubeprofile->status[i] == -2) {
      fprintf (fp, "\n%d : BANK NOT FILLED\n", i);
      continue;
    }
    else if (fdtubeprofile->status[i] == -1)
      fprintf (fp, "\n%d : BAD GEOMETRY\n", i);
    else if (fdtubeprofile->status[i] ==  0)
      fprintf (fp, "\n%d : FIT DID NOT CONVERGE\n", i);
    else
      fprintf (fp, "\n%d : FIT CONVERGED (status=%d)\n", i, fdtubeprofile->status[i]);
    fprintf (fp, "  (Psi, Rp, t0) = (%8.3f, %8.1f, %8.1f)\n",
      fdtubeprofile->psi[i]*R2D, fdtubeprofile->rp[i], fdtubeprofile->t0[i]);
    fprintf (fp, "  %d good of %d tubes\n", fdtubeprofile->ngtube[i], fdtubeprofile->ntube);
    fprintf (fp, "  Xmax   : %6.1f +/- %6.1f\n", fdtubeprofile->Xmax[i], fdtubeprofile->eXmax[i]);
    fprintf (fp, "  Nmax   : %4.3e +/- %4.3e\n", fdtubeprofile->Nmax[i], fdtubeprofile->eNmax[i]);
    if (fdtubeprofile->Lambda[i]==-1) {
      fprintf (fp, "  Sigma  : %6.4f +/- %6.4f\n", fdtubeprofile->X0[i], fdtubeprofile->eX0[i]);
      fprintf (fp, "  (using Gaussian-in-age profile fit)\n");
    }
    else {
      fprintf (fp, "  X0     : %6.1f +/- %6.1f\n", fdtubeprofile->X0[i], fdtubeprofile->eX0[i]);
      fprintf (fp, "  Lambda : %6.1f +/- %6.1f\n", fdtubeprofile->Lambda[i], fdtubeprofile->eLambda[i]);
    }
    fprintf (fp, "  Energy : %4.3e +/- %4.3e\n", fdtubeprofile->Energy[i], fdtubeprofile->eEnergy[i]);
    fprintf (fp, "  Chi2   : %6.1f\n", fdtubeprofile->chi2[i]);

  }

// Tube info
  if ( (*long_output) == 1) {
    for (i=0; i<3; i++) {
      if (fdtubeprofile->status[i] >= 0) {

        dst_sort_real8(fdtubeprofile->ntube, fdtubeprofile->x[i], g);

        fprintf (fp, "\nTube information for %d : (Psi, Rp, t0) = (%8.3f, %8.1f, %8.1f)\n",
          i, fdtubeprofile->psi[i]*R2D, fdtubeprofile->rp[i], fdtubeprofile->t0[i]);
        fprintf (fp, " idx                            gram      npe     enpe   simnpe      flux     "
		     "eflux       nfl     cvdir     cvmie     cvray   simflux      tres  tchi2        "
		     "Ne       eNe   qual  simtime  simtrms  timeres timechi2\n");
        for (j=0; j<fdtubeprofile->ntube; j++)
          fprintf (fp, "%4d [ cam %2d tube %3d ]   %9.4f %8.2f %8.2f %8.2f %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.3e %9.2e %6.2f %9.3e %9.3e %6d %8.2f %8.2f %8.2f %.3e\n", 
                   g[j], fdtubeprofile->camera[g[j]], fdtubeprofile->tube[g[j]], fdtubeprofile->x[i][g[j]],
                   fdtubeprofile->npe[i][g[j]], fdtubeprofile->enpe[i][g[j]],
		   fdtubeprofile->simnpe[i][g[j]], fdtubeprofile->flux[i][g[j]]/R2D, fdtubeprofile->eflux[i][g[j]]/R2D, 
                   fdtubeprofile->nfl[i][g[j]]/R2D, fdtubeprofile->ncvdir[i][g[j]]/R2D, fdtubeprofile->ncvmie[i][g[j]]/R2D, 
	           fdtubeprofile->ncvray[i][g[j]]/R2D, fdtubeprofile->simflux[i][g[j]]/R2D, 
	           fdtubeprofile->tres[i][g[j]], fdtubeprofile->tchi2[i][g[j]],
                   fdtubeprofile->ne[i][g[j]], fdtubeprofile->ene[i][g[j]], fdtubeprofile->tube_qual[i][g[j]], fdtubeprofile->simtime[i][g[j]],fdtubeprofile->simtrms[i][g[j]],fdtubeprofile->simtres[i][g[j]],fdtubeprofile->timechi2[i][g[j]]);
      }
    }
  }
  else
    fprintf (fp, "\nTube information not displayed in short output\n");

  fprintf (fp, "\n\n");

  return 0;
}
