/*
 * C functions for fdraw
 * DRB 2008/09/23
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "fdraw_dst.h"  
#include "caldat.h"  

fdraw_dst_common fdraw_;  /* allocate memory to fdraw_common */

integer4 fdraw_blen = 0; /* not static because it needs to be accessed by the c files of the derived banks */ 
static integer4 fdraw_maxlen = sizeof(integer4) * 2 + sizeof(fdraw_dst_common);
static integer1 *fdraw_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* fdraw_bank_buffer_ (integer4* fdraw_bank_buffer_size)
{
  (*fdraw_bank_buffer_size) = fdraw_blen;
  return fdraw_bank;
}



static void fdraw_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(fdraw_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
    fprintf (stderr,"fdraw_abank_init: fail to assign memory to bank. Abort.\n");
    exit(0);
  } 
}    
static void fdraw_bank_init() {fdraw_abank_init(&fdraw_bank);}

integer4 fdraw_common_to_bank_() {
  if (fdraw_bank == NULL) fdraw_bank_init();
  return fdraw_struct_to_abank_(&fdraw_, &fdraw_bank, FDRAW_BANKID, FDRAW_BANKVERSION);
}
integer4 fdraw_bank_to_dst_ (integer4 *unit) {return fdraw_abank_to_dst_(fdraw_bank, unit);}
integer4 fdraw_common_to_dst_(integer4 *unit) {
  if (fdraw_bank == NULL) fdraw_bank_init();
  return fdraw_struct_to_dst_(&fdraw_, &fdraw_bank, unit, FDRAW_BANKID, FDRAW_BANKVERSION);
}
integer4 fdraw_bank_to_common_(integer1 *bank) {return fdraw_abank_to_struct_(bank, &fdraw_);}
integer4 fdraw_common_to_dump_(integer4 *opt) {return fdraw_struct_to_dumpf_(-1, &fdraw_, stdout, opt);}
integer4 fdraw_common_to_dumpf_(FILE* fp, integer4 *opt) {return fdraw_struct_to_dumpf_(-1, &fdraw_, fp, opt);}

integer4 fdraw_struct_to_abank_(fdraw_dst_common *fdraw, integer1* (*pbank), integer4 id, integer4 ver)
{
  integer4 rcode, nobj, i, j;
  integer1 *bank;

  if ( *pbank == NULL ) fdraw_abank_init(pbank);
    
  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &fdraw_blen, &fdraw_maxlen, bank);

  /* Initialize test_blen, and pack the id and version to bank */

  nobj=1;
  rcode += dst_packi2_(&fdraw->event_code,            &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi2_(&fdraw->part,                  &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->num_mir,               &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_packi4_(&fdraw->event_num,             &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->julian,                &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->jsecond,               &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->gps1pps_tick,          &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->ctdclock,              &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_packi4_(&fdraw->ctd_version,           &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->tf_version,            &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->sdf_version,           &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  nobj=fdraw->num_mir;

  rcode += dst_packi4_(&fdraw->trig_code[0],          &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->second[0],             &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->microsec[0],           &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->clkcnt[0],             &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_packi2_(&fdraw->mir_num[0],            &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi2_(&fdraw->num_chan[0],           &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->tf_mode[0],            &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_packi4_(&fdraw->tf_mode2[0],           &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  for ( i=0; i<fdraw->num_mir; i++ ) {
    nobj = fdraw_nchan_mir + 1;
    rcode += dst_packi2_(&fdraw->hit_pt[i][0],        &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  }
  
  for(i=0;i<fdraw->num_mir;i++) {
    nobj=fdraw->num_chan[i];
    rcode += dst_packi2_(&fdraw->channel[i][0],       &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_packi2_(&fdraw->sdf_peak[i][0],      &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_packi2_(&fdraw->sdf_tmphit[i][0],    &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_packi2_(&fdraw->sdf_mode[i][0],      &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_packi2_(&fdraw->sdf_ctrl[i][0],      &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_packi2_(&fdraw->sdf_thre[i][0],      &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  }

  for ( i=0; i<fdraw->num_mir; i++ ) {
    for ( j=0; j<fdraw->num_chan[i]; j++ ) {
      nobj = 4;
      rcode += dst_packi2_((integer2 *)&fdraw->mean[i][j][0],     &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    }
  }

  for ( i=0; i<fdraw->num_mir; i++ ) {
    for ( j=0; j<fdraw->num_chan[i]; j++ ) {
      nobj = 4;
      rcode += dst_packi2_((integer2 *)&fdraw->disp[i][j][0],     &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    }
  }

  for(i=0;i<fdraw->num_mir;i++) {
    for(j=0;j<fdraw->num_chan[i];j++) {
      nobj=fdraw_nt_chan_max;
      rcode += dst_packi2_(&fdraw->m_fadc[i][j][0],   &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    }
  }  
  
  return rcode ;
}

integer4 fdraw_abank_to_dst_(integer1 *bank, integer4 *unit) {return dst_write_bank_(unit, &fdraw_blen, bank);}

integer4 fdraw_struct_to_dst_(fdraw_dst_common *fdraw, integer1* (*pbank), integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = fdraw_struct_to_abank_(fdraw, pbank, id, ver)) ) {
      fprintf(stderr, "fdraw_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = fdraw_abank_to_dst_(*pbank, unit)) ) {
      fprintf(stderr, "fdraw_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 fdraw_abank_to_struct_(integer1 *bank, fdraw_dst_common *fdraw) {
  integer2 junky;
  integer4 rcode = 0 ;
  integer4 nobj ,i ,j, one=1;
  integer4 id, ver;

  /*  fdraw_blen = 2 * sizeof(integer4);        skip id and version  */
  fdraw_blen = 0;

  nobj = 1;

  /* check id and version */
  rcode += dst_unpacki4_(&id, &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&ver, &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_unpacki2_(&fdraw->event_code,          &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki2_(&fdraw->part,                &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  if ( id == 12201 && ver == 0 ) {
    rcode += dst_unpacki2_(&junky,             &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    fdraw->num_mir = (integer4) junky;
  }
  else
    rcode += dst_unpacki4_(&fdraw->num_mir,             &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_unpacki4_(&fdraw->event_num,           &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->julian,              &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->jsecond,             &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->gps1pps_tick,        &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->ctdclock,            &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_unpacki4_(&fdraw->ctd_version,         &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->tf_version,          &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->sdf_version,         &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  nobj=fdraw->num_mir;

  if ( id == 12201 && ver == 0 ) {
    for ( i=0; i<nobj; i++ ) {
      rcode += dst_unpacki2_(&junky,        &one, bank, &fdraw_blen, &fdraw_maxlen);
      fdraw->trig_code[i] = (integer4) junky;
    }
  }
  else 
    rcode += dst_unpacki4_(&fdraw->trig_code[0],        &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_unpacki4_(&fdraw->second[0],           &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->microsec[0],         &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->clkcnt[0],           &nobj, bank, &fdraw_blen, &fdraw_maxlen);

  rcode += dst_unpacki2_(&fdraw->mir_num[0],          &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki2_(&fdraw->num_chan[0],         &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->tf_mode[0],          &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  rcode += dst_unpacki4_(&fdraw->tf_mode2[0],         &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  
  for ( i=0; i<fdraw->num_mir; i++ ) {
    nobj = fdraw_nchan_mir + 1;
    rcode += dst_unpacki2_(&fdraw->hit_pt[i][0],      &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  }
  
  for(i=0;i<fdraw->num_mir;i++) {
    nobj=fdraw->num_chan[i];
    rcode += dst_unpacki2_(&fdraw->channel[i][0],     &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_unpacki2_(&fdraw->sdf_peak[i][0],    &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_unpacki2_(&fdraw->sdf_tmphit[i][0],  &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_unpacki2_(&fdraw->sdf_mode[i][0],    &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_unpacki2_(&fdraw->sdf_ctrl[i][0],    &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    rcode += dst_unpacki2_(&fdraw->sdf_thre[i][0],    &nobj, bank, &fdraw_blen, &fdraw_maxlen);
  }  

  for ( i=0; i<fdraw->num_mir; i++ ) {
    for ( j=0; j<fdraw->num_chan[i]; j++ ) {
      nobj = 4;
      rcode += dst_unpacki2_((integer2 *)&fdraw->mean[i][j][0],   &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    }
  }

  for ( i=0; i<fdraw->num_mir; i++ ) {
    for ( j=0; j<fdraw->num_chan[i]; j++ ) {
      nobj = 4;
      rcode += dst_unpacki2_((integer2 *)&fdraw->disp[i][j][0],   &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    }
  }

  for(i=0;i<fdraw->num_mir;i++) {
    for(j=0;j<fdraw->num_chan[i];j++) {
      nobj=fdraw_nt_chan_max;
      rcode += dst_unpacki2_(&fdraw->m_fadc[i][j][0], &nobj, bank, &fdraw_blen, &fdraw_maxlen);
    }
  }  
  
  return rcode ;
}

integer4 fdraw_struct_to_dump_(integer4 siteid, fdraw_dst_common *fdraw, integer4 *long_output) {
  return fdraw_struct_to_dumpf_(siteid, fdraw, stdout, long_output);
}

integer4 fdraw_struct_to_dumpf_(integer4 siteid, fdraw_dst_common *fdraw, FILE* fp,integer4 *long_output)
{
  integer4 i,j,k;
  integer4 yr=0,mo=0,day=0;
  integer4 hr, min, sec, nano;
  integer1 sitestr[2][11] = { "BLACK_ROCK", "LONG_RIDGE" };
  int wf[64];
  int minwf, maxwf;
  double row;
  int thresh1, thresh2;
  if ( siteid == 0 )
    fprintf(fp, "BRRAW :\n");
  else if ( siteid == 1 )
    fprintf(fp, "LRRAW :\n");
  else
    fprintf (fp, "FDRAW :\n");

  hr = fdraw->jsecond / 3600 + 12;

  if (hr >= 24) {
    caldat((double)fdraw->julian+1., &mo, &day, &yr);
    hr -= 24;
  }
  else
    caldat((double)fdraw->julian, &mo, &day, &yr);

  min = ( fdraw->jsecond / 60 ) % 60;
  sec = fdraw->jsecond % 60;
  // nano = ( fdraw->ctdclock - fdraw->gps1pps_tick ) * 25; Commented out by Shin 2015/10/16

  if((fdraw->ctd_version >> 30 & 0x1) == 1){
    nano = (int) (1e9 * fdraw->ctdclock / fdraw->gps1pps_tick);}
  else{
    nano = (fdraw->ctdclock - fdraw->gps1pps_tick) * 25;} //Calculate nano in alternate way if higher CTD version

  fprintf(fp, "  %s site:  part %02d  event_code: %d\n", 
	  (siteid>=0)?sitestr[siteid]:"UNDEFINED", 
	  fdraw->part, fdraw->event_code);

  fprintf(fp, "  firmware: CTD ver %d  TF ver %d  SDF ver %d\n", 
	  fdraw->ctd_version, fdraw->tf_version, fdraw->sdf_version);

  fprintf(fp, "  trigger %6d  %d/%02d/%4d  %02d:%02d:%02d.%09d\n", 
	  fdraw->event_num, mo, day, yr, hr, min, sec, nano);

  fprintf(fp, "  gps tick: %9d  ctdclock: %9d\n", 
	  fdraw->gps1pps_tick, fdraw->ctdclock);

  fprintf(fp, "  number of participating cameras: %2d\n", 
	  fdraw->num_mir);

  for ( i=0; i<fdraw->num_mir; i++ ) {
    hr = fdraw->second[i] / 3600;
    min = ( fdraw->second[i] / 60 ) % 60;
    sec = fdraw->second[i] % 60;

    fprintf(fp, "  camera %2d  code: %1d  store time: %02d:%02d:%02d.%06d  "
	    "clkcnt: %9d\n", fdraw->mir_num[i], fdraw->trig_code[i], hr, min, 
	    sec, fdraw->microsec[i], fdraw->clkcnt[i]);
    fprintf(fp, "    tf mode: %d  mode2: %d\n", fdraw->tf_mode[i], 
	    fdraw->tf_mode2[i]);
    fprintf(fp, "    have waveform data for %3d tubes\n", fdraw->num_chan[i]);
    fprintf(fp, "      hit_pt: (last entry=%d)\n", fdraw->hit_pt[i][256]);
    for ( j=0; j<fdraw_nchan_mir; j++ ) {
      if ( j%16 == 0 )
	fprintf(fp, "        ");
      fprintf(fp, " %d", fdraw->hit_pt[i][j]);
      if ( (j+1)%16 == 0 )
	fprintf(fp, "\n");
    }

    if ( *(long_output) == 1 ) {
      for ( j=0; j<fdraw->num_chan[i]; j++ ) {
        maxwf = 0;
        minwf = 1000000;
        for (k=0; k<64; k++)
          wf[k] = 0;
	fprintf(fp, "    cam %2d tube %3d:  peak: %d  tmphit: %d\n", 
		fdraw->mir_num[i],fdraw->channel[i][j], fdraw->sdf_peak[i][j], 
		fdraw->sdf_tmphit[i][j]);
	fprintf(fp, "      mode: %d  ctrl: %d  thre: %d\n", 
		fdraw->sdf_mode[i][j], fdraw->sdf_ctrl[i][j], 
		fdraw->sdf_thre[i][j]);
	fprintf(fp, "      mean: %5d %5d %5d %5d  "
		"disp: %5d %5d %5d %5d\n", 
		fdraw->mean[i][j][0], fdraw->mean[i][j][1], 
		fdraw->mean[i][j][2], fdraw->mean[i][j][3], 
		fdraw->disp[i][j][0], fdraw->disp[i][j][1], 
		fdraw->disp[i][j][2], fdraw->disp[i][j][3]);

	fprintf(fp, "      waveform data:\n");
	for ( k=0; k<fdraw_nt_chan_max; k++ ) {
          wf[k/8] += fdraw->m_fadc[i][j][k];
	  if ( k%12 == 0 )
	    fprintf(fp, "       ");
	  fprintf(fp, " %04X", fdraw->m_fadc[i][j][k]);
	  if ( (k+1)%12 == 0 )
	    fprintf(fp, "\n");
	}
	if ( k%12 != 0 )
	  fprintf(fp, "\n");
        
        for (k=0; k<64; k++) {
          if (wf[k] > maxwf)
            maxwf = wf[k];
          if (wf[k] < minwf)
            minwf = wf[k];
        }
        printf("visualization (8-bin sums): c %02d t %03d: min %6d max %6d\n",
               fdraw->mir_num[i],fdraw->channel[i][j],minwf,maxwf);
        for (row=3; row>=0; row--) {
          thresh1 = (int)minwf + ((row/4.)*(double)(maxwf - minwf));
          thresh2 = (int)minwf + (((row+0.5)/4)*(double)(maxwf - minwf));
          printf("   ");
          for (k=0; k<64; k++) {
            if (wf[k] >= thresh2)
              printf(":");
            else if (wf[k] >= thresh1)
              printf(".");
            else
              printf(" ");
          }
          printf("\n");
        }
        
          
            
      }
    }
  }

  return 0;
} 

