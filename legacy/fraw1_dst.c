/*
 * C functions for fadc raw 1
 * MRM July 18
 * Modified Oct 4 1995 by JHB: pack/unpack only filled quantities
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "fraw1_dst.h"  

fraw1_dst_common fraw1_;  /* allocate memory to fraw1_common */

static integer4 fraw1_blen = 0; 
static integer4 fraw1_maxlen = sizeof(integer4) * 2 + sizeof(fraw1_dst_common);
static integer1 *fraw1_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* fraw1_bank_buffer_ (integer4* fraw1_bank_buffer_size)
{
  (*fraw1_bank_buffer_size) = fraw1_blen;
  return fraw1_bank;
}



static void fraw1_bank_init()
{
  fraw1_bank = (integer1 *)calloc(fraw1_maxlen, sizeof(integer1));
  if (fraw1_bank==NULL)
    {
      fprintf (stderr,"fraw1_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
    } /* else fprintf ( stderr,"fraw1_bank allocated memory %d\n",fraw1_maxlen); */
}    

integer4 fraw1_common_to_bank_()
{
  static integer4 id = FRAW1_BANKID, ver = FRAW1_BANKVERSION;
  integer4 rcode, nobj, i, j;

  if (fraw1_bank == NULL) fraw1_bank_init();
    
  rcode = dst_initbank_(&id, &ver, &fraw1_blen, &fraw1_maxlen, fraw1_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj=1;
  rcode += dst_packi2_(&fraw1_.event_code, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi2_(&fraw1_.site, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi2_(&fraw1_.part, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi2_(&fraw1_.num_mir, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);

  rcode += dst_packi4_(&fraw1_.event_num, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi4_(&fraw1_.julian, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi4_(&fraw1_.jsecond, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi4_(&fraw1_.jclkcnt, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);

  nobj=fraw1_.num_mir;

  rcode += dst_packi4_(&fraw1_.second[0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi4_(&fraw1_.clkcnt[0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);

  rcode += dst_packi2_(&fraw1_.mir_num[0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi2_(&fraw1_.num_chan[0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  
  for(i=0;i<fraw1_.num_mir;i++) {
    nobj=fraw1_.num_chan[i];
    rcode += dst_packi2_(&fraw1_.channel[i][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
    rcode += dst_packi2_(&fraw1_.it0_chan[i][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
    rcode += dst_packi2_(&fraw1_.nt_chan[i][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  }  

  for(i=0;i<fraw1_.num_mir;i++) {
    for(j=0;j<fraw1_.num_chan[i];j++) {
      nobj=fraw1_.nt_chan[i][j];
      rcode += dst_packi1_(&fraw1_.m_fadc[i][j][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
    }
  }  
  
  return rcode ;
}

integer4 fraw1_bank_to_dst_ (integer4 *unit)
{
  integer4 rcode;
  rcode = dst_write_bank_(unit, &fraw1_blen, fraw1_bank);
  free(fraw1_bank);
  fraw1_bank = NULL; 
  return rcode;
}

integer4 fraw1_common_to_dst_(integer4 *unit)
{
  integer4 rcode;
    if ( (rcode = fraw1_common_to_bank_()) )
    {
      fprintf(stderr, "fraw1_common_to_bank_ ERROR : %ld\n", (long)rcode);
      exit(0);
    }
    if ( (rcode = fraw1_bank_to_dst_(unit) ))
    {
      fprintf(stderr, "fraw1_bank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
    }
  return 0;
}

integer4 fraw1_bank_to_common_(integer1 *bank)
{
  integer4 rcode = 0 ;
  integer4 nobj ,i ,j;
  fraw1_blen = 2 * sizeof(integer4);	/* skip id and version  */

  nobj=1;
  rcode += dst_unpacki2_(&fraw1_.event_code, &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki2_(&fraw1_.site, &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki2_(&fraw1_.part, &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki2_(&fraw1_.num_mir, &nobj, bank, &fraw1_blen, &fraw1_maxlen);

  rcode += dst_unpacki4_(&fraw1_.event_num, &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki4_(&fraw1_.julian, &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki4_(&fraw1_.jsecond, &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki4_(&fraw1_.jclkcnt, &nobj, bank, &fraw1_blen, &fraw1_maxlen);

  nobj=fraw1_.num_mir;
  rcode += dst_unpacki4_(&fraw1_.second[0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki4_(&fraw1_.clkcnt[0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);

  rcode += dst_unpacki2_(&fraw1_.mir_num[0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_unpacki2_(&fraw1_.num_chan[0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  
  for(i=0;i<fraw1_.num_mir;i++) {
    nobj=fraw1_.num_chan[i];
    rcode += dst_unpacki2_(&fraw1_.channel[i][0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
    rcode += dst_unpacki2_(&fraw1_.it0_chan[i][0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
    rcode += dst_unpacki2_(&fraw1_.nt_chan[i][0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
  }  

  for(i=0;i<fraw1_.num_mir;i++) {
    for(j=0;j<fraw1_.num_chan[i];j++) {
      nobj=fraw1_.nt_chan[i][j];
      rcode += dst_unpacki1_(&fraw1_.m_fadc[i][j][0], &nobj, bank, &fraw1_blen, &fraw1_maxlen);
    }
  }  

  return rcode ;
}

integer4 fraw1_common_to_dump_(integer4 *long_output)
{
  return fraw1_common_to_dumpf_(stdout,long_output);
}

integer4 fraw1_time_fprint_(FILE* fp,integer4 *second,integer4 *clkcnt)
{
  integer4 hh,mm,ss;
  double cc;
  ss = fraw1_.jsecond + *second;
  cc =  (((double) 50) * ((double) *clkcnt))/((double) 3);
  cc += (double) fraw1_.jclkcnt;
  if (cc > (double) 999999999) {
    cc -= (double) 1000000000;
    ss ++;
  }
  hh = ss/3600;
  mm = (ss/60)%60;
  ss = ss%60;
  /*
  fprintf (fp," second %d clkcnt %d\n",*second,*clkcnt);
  fprintf (fp," jsecond %d jclkcnt %d\n",fraw1_.jsecond,fraw1_.jclkcnt);
  */
  fprintf (fp," event store start time -- %d:%02d:%02d.%09.0f\n",hh,mm,ss,cc);
  return 0;
}

integer4 fraw1_time_print_(integer4 *second,integer4 *clkcnt)
{
  return fraw1_time_fprint_(stdout,second,clkcnt);
}

integer4 fraw1_common_to_dumpf_(FILE* fp,integer4 *long_output)
{
  integer4 i,j,k,counter;
  integer4 yr,mo,day,hr,min,sec;
  // union { integer1 i1; unsigned char c1; } i_or_c;

  counter=0;
  fprintf (fp, "FRAW1 :\n");
  yr = fraw1_.julian/10000;
  mo = (fraw1_.julian/100)%100;
  day = fraw1_.julian%100;
  hr = fraw1_.jsecond/3600;
  min = (fraw1_.jsecond/60)%60;
  sec = fraw1_.jsecond%60;
  fprintf (fp, " evt_code %4d run start: %d/%d/%d %d:%02d:%02d.%09d\n",fraw1_.event_code,mo,day,yr,hr,min,sec,fraw1_.jclkcnt);
  fprintf (fp, " site  %4d part  %4d event_num  %4d num_mir  %4d\n",fraw1_.site,fraw1_.part,fraw1_.event_num,fraw1_.num_mir);
  if (*long_output==0) {
    for(i=0;i<fraw1_.num_mir;i++) {
      fprintf (fp, " m  %4d num_chan  %4d\n",fraw1_.mir_num[i],fraw1_.num_chan[i]);
      fraw1_time_fprint_(fp,&fraw1_.second[i],&fraw1_.clkcnt[i]);
      fprintf (fp, " nt_store = %4d\n",fraw1_.nt_chan[0][0]);
    }
  }
  else if (*long_output==1) {  union { integer1 i1; unsigned char c1; } i_or_c;
    for(i=0;i<fraw1_.num_mir;i++) {
      fprintf (fp, " m  %4d num_chan  %4d\n",fraw1_.mir_num[i],fraw1_.num_chan[i]);
      fraw1_time_fprint_(fp,&fraw1_.second[i],&fraw1_.clkcnt[i]);
      for(j=0;j<fraw1_.num_chan[i];j++) {
	fprintf (fp, " hit %3d chan(HI=00-FF, LO=100-11F, TR=200-21F)  %3.2X it0  %4d nt  %3d\n",j+1,fraw1_.channel[i][j]-1,fraw1_.it0_chan[i][j],fraw1_.nt_chan[i][j]);
	for(k=0;k<fraw1_.nt_chan[i][j];k++) {
          i_or_c.i1 = fraw1_.m_fadc[i][j][k];
          fprintf (fp, " %02X",i_or_c.c1);
	  counter++;
	  if (counter==20) {
	    fprintf(fp, "\n");
	    counter=0;
	  }
	}
	if (counter != 0) {
	  fprintf(fp, "\n");
	  counter = 0;
	}
      }
    }    
  }
  
  return 0;
} 

