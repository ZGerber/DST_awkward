// DST bank implementation for shower library entries.
// Based on shower library entries originally created by Andreas Zech for HiRes.
// The Gaisser-Hillas parameterization of a hadronic shower is
//    n(x) = nmax * ((x-x0)/(xmax-x0))^((xmax-x0)/lambda) * exp((xmax-x)/lambda)
//
// showlib_dst.c DRB - 2009/01/07

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_err_codes.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "showlib_dst.h"

showlib_dst_common showlib_;

static integer4 showlib_blen = 0;
static integer4 showlib_maxlen = sizeof(integer4) * 2 + sizeof(showlib_dst_common);
static integer1 *showlib_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* showlib_bank_buffer_ (integer4* showlib_bank_buffer_size)
{
  (*showlib_bank_buffer_size) = showlib_blen;
  return showlib_bank;
}



static void showlib_bank_init() {
  showlib_bank = (integer1 *)calloc(showlib_maxlen, sizeof(integer1));
  if (showlib_bank==NULL) {
      fprintf (stderr,"showlib_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 showlib_common_to_bank_() {
  static integer4 id = SHOWLIB_BANKID, ver = SHOWLIB_BANKVERSION;
  integer4 rcode, nobj;

  if (showlib_bank == NULL) showlib_bank_init();

  rcode = dst_initbank_(&id, &ver, &showlib_blen, &showlib_maxlen, showlib_bank);

// Initialize showlib_blen and pack the id and version to bank

  nobj = 1;
  rcode += dst_packi4_(&showlib_.code,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packi4_(&showlib_.number,   &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.angle,    &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packi4_(&showlib_.particle, &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.energy,   &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.first,    &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.nmax,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.x0,       &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.xmax,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.lambda,   &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_packr4_(&showlib_.chi2,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);

  return rcode;
}

integer4 showlib_bank_to_dst_ (integer4 *unit) {
  return dst_write_bank_(unit, &showlib_blen, showlib_bank);
}

integer4 showlib_common_to_dst_(integer4 *unit) {
  integer4 rcode;
    if ( (rcode = showlib_common_to_bank_()) ) {
      fprintf(stderr, "showlib_common_to_bank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
    if ( (rcode = showlib_bank_to_dst_(unit)) ) {
      fprintf(stderr, "showlib_bank_to_dst_ ERROR : %ld\n", (long)rcode);           
      exit(0);
  }
  return 0;
}

integer4 showlib_bank_to_common_(integer1 *showlib_bank) {
  integer4 rcode = 0 ;
  integer4 nobj;
  showlib_blen = 2 * sizeof(integer4);   /* skip id and version  */

  nobj = 1;
  rcode += dst_unpacki4_(&showlib_.code,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpacki4_(&showlib_.number,   &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.angle,    &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpacki4_(&showlib_.particle, &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.energy,   &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.first,    &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.nmax,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.x0,       &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.xmax,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.lambda,   &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);
  rcode += dst_unpackr4_(&showlib_.chi2,     &nobj, showlib_bank, &showlib_blen, &showlib_maxlen);

  return rcode;
}

integer4 showlib_common_to_dump_(integer4 *long_output) {
  return showlib_common_to_dumpf_(stdout, long_output);       
}

integer4 showlib_common_to_dumpf_(FILE* fp, integer4 *long_output) {
  char particle[0x20];
  char model[0x20];
  (void)(long_output);
  switch (showlib_.particle) {
  case 14:
    sprintf(particle,"prot");
    break;
  case 5626:
    sprintf(particle,"iron");
    break;
  default:
    sprintf(particle,"%4d",showlib_.particle);
    break;
  }

  switch (showlib_.code%10) {
  case 0:
    sprintf(model," QGSJet 01");
    break;
  case 1:
    sprintf(model,"SIBYLL 1.6");
    break;
  case 2:
    sprintf(model,"SIBYLL 2.1");
    break;
  default:
    sprintf(model,"   MODEL %d",showlib_.code%10);
    break;
  }
    
  fprintf (fp, "AZShower %6d %3d: %4s %9s %2.0f\n", 
	   showlib_.code,showlib_.number,particle,model,57.296*showlib_.angle);
  fprintf (fp, "         energy: %6.2f EeV, first int: %6.1f g/cm2\n",
	   showlib_.energy/1e9,showlib_.first);
  fprintf (fp, "         nMx: %10.3e x0: %6.1f g/cm2 xMx: %6.1f g/cm2 lam: %4.1f g/cm2\n",
	   showlib_.nmax,showlib_.x0,showlib_.xmax,showlib_.lambda);
  
  return SUCCESS;
}
