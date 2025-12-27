#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"
#include "univ_dst.h"

#include "hyp1_dst.h"
#include "caldat.h"

hyp1_dst_common hyp1_;

integer4 hyp1_blen = 0; /* not static because it needs to be accessed by the c files of the derived banks */
static integer4 hyp1_maxlen =
  sizeof (integer4) * 2 + sizeof (hyp1_dst_common);
static integer1 *hyp1_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* hyp1_bank_buffer_ (integer4* hyp1_bank_buffer_size)
{
  (*hyp1_bank_buffer_size) = hyp1_blen;
  return hyp1_bank;
}

static void hyp1_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(hyp1_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
      fprintf (stderr,"hyp1_abank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

static void hyp1_bank_init(){
	hyp1_abank_init(&hyp1_bank);
}
integer4 hyp1_common_to_bank_() {
  if (hyp1_bank == NULL) hyp1_bank_init();
  return hyp1_struct_to_abank_(&hyp1_, &hyp1_bank, HYP1_BANKID, HYP1_BANKVERSION);
}
/*
integer4 hyp1_bank_to_dst_ (integer4 *unit) {
	return hyp1_abank_to_dst_(hyp1_bank, unit);
}
*/
integer4 hyp1_common_to_dst_(integer4 *unit) {
  if (hyp1_bank == NULL) hyp1_bank_init();
  return hyp1_struct_to_dst_(&hyp1_, hyp1_bank, unit, HYP1_BANKID, HYP1_BANKVERSION);
}

integer4 hyp1_bank_to_common_(integer1 *bank) {
	return hyp1_abank_to_struct_(bank, &hyp1_);
}

integer4 hyp1_struct_to_abank_(hyp1_dst_common *hyp1, integer1 *(*pbank), integer4 id, integer4 ver){
//  static integer4 id = HYP1_BANKID;
//  static integer4 ver = HYP1_BANKVERSION;

  integer4 i; // will do a little indexing...  
  integer4 rcode, nobj;
  integer1 *bank;

  if(*pbank == NULL){
    hyp1_bank_init();
  }

  bank = *pbank;

  rcode = dst_initbank_(&id, &ver, &hyp1_blen, &hyp1_maxlen, bank);
  
  nobj = 1;
  rcode += dst_packi2_(&hyp1->eventCode, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->tref, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->offset, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->fdEventNum, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->fdsiteid, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->julian, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->jsecond, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->jsecfrac, &nobj, bank, &hyp1_blen, &hyp1_maxlen);

  rcode += dst_packi4_(&hyp1->sdEventNum, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->sdsiteid, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->yymmdd, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->hhmmss, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->usec, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  
  rcode += dst_packi4_(&hyp1->nfit, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->ngtube, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  
  nobj = 3;
  for(i=0;i<hyp1->ngtube;i++){
    rcode += dst_packr8_(&hyp1->tubeVector[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = hyp1->ngtube;
  rcode += dst_packr8_(&hyp1->tubeAlt[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->tubeAzm[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->tubeSigma[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->fdplaneIndex[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->npe[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->fdTime[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->fdTimeRMS[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_packr8_(&hyp1->planeAlt[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_packr8_(&hyp1->planeAzm[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  nobj = 1;    
  rcode += dst_packi4_(&hyp1->nhits, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  nobj = 3;
  for(i=0;i<hyp1->nhits;i++){
    rcode += dst_packr8_(&hyp1->xyz[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  
  nobj = hyp1->nhits;
  rcode += dst_packr8_(&hyp1->rho[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packi4_(&hyp1->rufptnIndex[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_packr8_(&hyp1->sdTime[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_packr8_(&hyp1->sdTimeSigma[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_packr8_(&hyp1->sdPlaneAlt[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_packr8_(&hyp1->sdPlaneAzm[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  nobj = 3;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_packr8_(&hyp1->sdp[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = 128;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_packi1_(&hyp1->fitType[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = hyp1->nfit;
  rcode += dst_packr8_(&hyp1->rp[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dRp[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->psi[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dPsi[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->t0[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dT0[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->xcore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dXcore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->ycore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dYcore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->zen[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dZen[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->azm[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dAzm[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->tc[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_packr8_(&hyp1->dTc[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  
//  nobj = hyp1->nfit;
  nobj = 2;
  rcode += dst_packi4_(&hyp1->nComp[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  for(i=0;i<hyp1->nfit;i++){
  //	nobj = hyp1->nComp[i];
    nobj = 4;
  	rcode += dst_packr8_(&hyp1->chi2Comp[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = hyp1->nfit;
  rcode += dst_packr8_(&hyp1->chi2[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
	nobj = hyp1->ngtube;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_packr8_(&hyp1->fdResidual[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
	nobj = hyp1->nhits;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_packr8_(&hyp1->sdResidual[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  return rcode;
}

integer4 hyp1_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &hyp1_blen, bank);
}

integer4 hyp1_struct_to_dst_(hyp1_dst_common *hyp1, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = hyp1_struct_to_abank_(hyp1, &bank, id, ver)) ) {
      fprintf(stderr, "hyp1_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = hyp1_abank_to_dst_(bank, unit)) ) {
      fprintf(stderr, "hyp1_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 hyp1_bank_to_dst_(integer4 * unit) {
	return hyp1_abank_to_dst_(hyp1_bank, unit);
/*
  integer4 rcode;
  rcode = dst_write_bank_(unit, &hyp1_blen, hyp1_bank);
  free (hyp1_bank);
  hyp1_bank = NULL;
  return rcode;
*/
}
/*
 * The old verision
integer4 hyp1_common_to_dst_(integer4 * unit) {
  integer4 rcode;
    if ( (rcode = hyp1_common_to_bank_()) ){
      fprintf (stderr, "hyp1_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
    if ( (rcode = hyp1_bank_to_dst_(unit) )){
      fprintf (stderr, "hyp1_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}
*/
integer4 hyp1_abank_to_struct_(integer1 *bank, hyp1_dst_common *hyp1) {

  integer4 i; // will do a little indexing...
  integer4 rcode = 0;
  integer4 nobj;
  hyp1_blen = 2 * sizeof (integer4);  // skip id and version

  nobj = 1;
  rcode += dst_unpacki2_(&hyp1->eventCode, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->tref, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->offset, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->fdEventNum, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->fdsiteid, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->julian, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->jsecond, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->jsecfrac, &nobj, bank, &hyp1_blen, &hyp1_maxlen);

  rcode += dst_unpacki4_(&hyp1->sdEventNum, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->sdsiteid, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->yymmdd, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->hhmmss, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->usec, &nobj, bank, &hyp1_blen, &hyp1_maxlen);

  rcode += dst_unpacki4_(&hyp1->nfit, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->ngtube, &nobj, bank, &hyp1_blen, &hyp1_maxlen);

  nobj = 3;
  for(i=0;i<hyp1->ngtube;i++){
    rcode += dst_unpackr8_(&hyp1->tubeVector[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);

  }
  nobj = hyp1->ngtube;

  rcode += dst_unpackr8_(&hyp1->tubeAlt[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->tubeAzm[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->tubeSigma[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->fdplaneIndex[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->npe[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->fdTime[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->fdTimeRMS[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);

  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_unpackr8_(&hyp1->planeAlt[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_unpackr8_(&hyp1->planeAzm[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  nobj = 1;
  rcode += dst_unpacki4_(&hyp1->nhits, &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  nobj = 3;
  for(i=0;i<hyp1->nhits;i++){
    rcode += dst_unpackr8_(&hyp1->xyz[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  nobj = hyp1->nhits;
  rcode += dst_unpackr8_(&hyp1->rho[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpacki4_(&hyp1->rufptnIndex[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_unpackr8_(&hyp1->sdTime[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_unpackr8_(&hyp1->sdTimeSigma[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_unpackr8_(&hyp1->sdPlaneAlt[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  	rcode += dst_unpackr8_(&hyp1->sdPlaneAzm[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  nobj = 3;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_unpackr8_(&hyp1->sdp[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = 128;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_unpacki1_(&hyp1->fitType[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = hyp1->nfit;
  rcode += dst_unpackr8_(&hyp1->rp[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dRp[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->psi[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dPsi[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->t0[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dT0[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->xcore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dXcore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->ycore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dYcore[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->zen[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dZen[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->azm[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dAzm[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->tc[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  rcode += dst_unpackr8_(&hyp1->dTc[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  
//  nobj = hyp1->nfit;
  nobj = 2;
  rcode += dst_unpacki4_(&hyp1->nComp[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  for(i=0;i<hyp1->nfit;i++){
//  	nobj = hyp1->nComp[i];
    nobj = 4;
  	rcode += dst_unpackr8_(&hyp1->chi2Comp[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
  nobj = hyp1->nfit;
  rcode += dst_unpackr8_(&hyp1->chi2[0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
	nobj = hyp1->ngtube;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_unpackr8_(&hyp1->fdResidual[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }
	nobj = hyp1->nhits;
  for(i=0;i<hyp1->nfit;i++){
  	rcode += dst_unpackr8_(&hyp1->sdResidual[i][0], &nobj, bank, &hyp1_blen, &hyp1_maxlen);
  }

  return rcode;
} 

integer4 hyp1_common_to_dump_ (integer4 * long_output) {
  return hyp1_common_to_dumpf_ (stdout, long_output);
}

integer4 hyp1_common_to_dumpf_ (FILE * fp, integer4 * long_output) {

	hyp1_struct_to_dumpf_(&hyp1_, fp, long_output);
	return 0;
}

integer4 hyp1_struct_to_dumpf_ (hyp1_dst_common *hyp1, FILE * fp, integer4 * long_output) {

  char bank[100];
  if(hyp1->fdsiteid == BR){
    strcpy(bank, "BRHYP1");
  }
  else if(hyp1->fdsiteid == LR){
    strcpy(bank, "LRHYP1");
  }
  else if(hyp1->fdsiteid == MD){
    strcpy(bank, "MDHYP1");
  }
  else{
    strcpy(bank, "HYP1");
  }
  fprintf(fp, "%s Bank\n", bank);
  integer4 year = hyp1->yymmdd/1e4;
  integer4 month = (hyp1->yymmdd/100)%100;
  integer4 day =  hyp1->yymmdd%100;
  integer4 hour = hyp1->hhmmss/1e4;
  integer4 min = (hyp1->hhmmss/100)%100;
  integer4 sec = hyp1->hhmmss%100;
  fprintf(fp, "Timestamp: %d -- %02d/%02d/%02d -- %02d:%02d:%02d.%09d\n",
	  hyp1->julian, year, month, day, hour, min, sec, (int)(hyp1->tref));
  fprintf(fp, "FD/SD offset: %f ns", hyp1->offset);

  int i, j;
  for(i=0;i<hyp1->nfit;i++){
  	fprintf(fp, "\nFIT: %s\n", hyp1->fitType[i]);
  	fprintf(fp, "x_c, y_c = %7.5g, %7.5g [km North/East of CLF]\n", hyp1->xcore[i]/1000, hyp1->ycore[i]/1000);
  	fprintf(fp, "zen, azm = %7.5g, %7.5g [degrees]\n",
  			    hyp1->zen[i]*R2D, hyp1->azm[i]*R2D);
  	fprintf(fp, "tc = %7.5g [microsec after timestamp]\n", hyp1->tc[i]/1000);

  	fprintf(fp, "rp, psi = %7.5g km, %7.5g deg\n",
  			    hyp1->rp[i]/1000, hyp1->psi[i]*R2D);
  	fprintf(fp, "t0 = %7.5g usec\n", hyp1->t0[i]/1000);
//  	fprintf(fp, "sdp vector: (%6.3g, %6.3g, %6.3g)\n",
//  			    hyp1->sdp[i][0], hyp1->sdp[i][1], hyp1->sdp[i][2]);

	if (i == 0)
	{
	  fprintf(fp, "chi2 / dof = %7.5g / (%d + %d - 3)\n",
		  hyp1->chi2[i], hyp1->nhits, hyp1->ngtube);
	  fprintf(fp, "           = %7.5g\n",
		  hyp1->chi2[i]/(hyp1->nhits + hyp1->ngtube - 3));
	}
	else
	{
	  fprintf(fp, "chi2 / dof = %7.5g / (%d + %d - 5)\n",
		  hyp1->chi2[i], hyp1->nhits, hyp1->ngtube);
	  fprintf(fp, "           = %7.5g\n",
		  hyp1->chi2[i]/(hyp1->nhits + hyp1->ngtube - 5));
	}

     fprintf(fp, "chi2 components:\n");
     fprintf(fp, "%11s %11s %11s %11s\n", "SDP", "COC",
    		     "FDTiming", "SDTiming");

     fprintf(fp, "%11.3e %11.3e %11.3e %11.3e\n",
             hyp1->chi2Comp[i][2], hyp1->chi2Comp[i][3], hyp1->chi2Comp[i][0],
             hyp1->chi2Comp[i][1]);
  }

  if(*long_output == 1){

    for(i=0;i<hyp1->nfit;i++){
    	fprintf(fp, "FIT: %s\n", hyp1->fitType[i]);

      fprintf(fp, "sd hits: %d\n", hyp1->nhits);
      /*fprintf(fp, "sdpos      planeAlt  planeAzm  rho   time  ts\n");*/
      fprintf(fp, "%13s %13s %13s %13s %13s %13s %13s %13s\n",
	      "sdPlaneAlt", "sdPlaneAzm", "rho",
	      "sdTime", "sdTimeSigma", "sdResidual", "sdpos X", "sdpos Y");
    	for(j=0;j<hyp1->nhits;j++){
        fprintf(fp, "%13.5f %13.5f %13.5f %13.5f %13.5f %13.5f %13.5f %13.5f\n",
                hyp1->sdPlaneAlt[i][j]*180./M_PI,
		hyp1->sdPlaneAzm[i][j]*180./M_PI, hyp1->rho[j],
                hyp1->sdTime[i][j]/1000, hyp1->sdTimeSigma[i][j]/1000,
                hyp1->sdResidual[i][j], hyp1->xyz[j][0]/1000,
		hyp1->xyz[j][1]/1000);
    	}
    	fprintf(fp, "\nfd tubes: %d\n", hyp1->ngtube);
    	/* fprintf(fp, "planeAlt  planeAzm  rho   time  rms residual tubeVector\n"); */
	fprintf(fp, "%13s %13s %13s %13s %13s %13s %13s %13s %13s\n",
		"planeAlt", "planeAzm", "npe", "fdTime", "fdTimeRMS",
		"fdResidual", "tubeVector X", "tubeVector Y", "tubeVector Z");
    	for(j=0;j<hyp1->ngtube;j++){
    		fprintf(fp, "%13.5f %13.5f %13.5f %13.5f %13.5f %13.5f %13.5f "
			"%13.5f %13.5f\n",
      		      hyp1->planeAlt[i][j]*180./M_PI,
		      hyp1->planeAzm[i][j]*180./M_PI,
		      hyp1->npe[j], (real8)hyp1->fdTime[j]/1000,
		      hyp1->fdTimeRMS[j]/1000,
		      hyp1->fdResidual[i][j], hyp1->tubeVector[j][0],
		      hyp1->tubeVector[j][1], hyp1->tubeVector[j][2]);

	}
    }
  }
  return 0;
}
