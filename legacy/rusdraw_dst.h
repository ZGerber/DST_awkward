/*
 *     Bank for SD raw data
 *     Dmitri Ivanov (dmiivanov@gmail.com)
 *     Jun 17, 2008

 *     Last modified: Mar. 6, 2019

*/

#ifndef _RUSDRAW_
#define _RUSDRAW_

#include <stdio.h>
#include "dst_std_types.h"

#define RUSDRAW_BANKID  13101
#define RUSDRAW_BANKVERSION   001

#ifdef __cplusplus
extern "C" {
#endif
integer4 rusdraw_common_to_bank_ ();
integer4 rusdraw_bank_to_dst_ (integer4 * NumUnit);
integer4 rusdraw_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 rusdraw_bank_to_common_ (integer1 * bank);
integer4 rusdraw_common_to_dump_ (integer4 * opt1);
integer4 rusdraw_common_to_dumpf_ (FILE * fp, integer4 * opt2);
integer1* rusdraw_bank_buffer_ (integer4* rusdraw_bank_buffer_size);
integer4 rusdraw_site_from_bitflag(integer4 tower_bitflag); /* rusdraw site ID given tower bit flag */
integer4 rusdraw_bitflag_from_site(integer4 rusdraw_site); /* tower bit flag give rusdraw site ID */

#ifdef __cplusplus
} //end extern "C"
#endif

#define RUSDRAW_BR 0
#define RUSDRAW_LR 1
#define RUSDRAW_SK 2
#define RUSDRAW_BRLR 3
#define RUSDRAW_BRSK 4
#define RUSDRAW_LRSK 5
#define RUSDRAW_BRLRSK 6

#define RUSDRAWMWF 0x400
#define rusdraw_nchan_sd 128	/*  128 fadc channels per SD counter */

#define RUSDRAW_DST_GZ ".rusdraw.dst.gz" /* output suffix */


typedef struct
{
  integer4 event_num;		                        /* event number */
  integer4 event_code;                                  /* 1=data, 0=Monte Carlo */
  integer4 site;                                        /* BR=0,LR=1,SK=2,BRLR=3,BRSK=4,LRSK=5,BRLRSK=6 */
  integer4 run_id[3];                                   /* run number for [0]-BR,[1]-LR,[2]-SK, -1 if irrelevant */
  integer4 trig_id[3];		                        /* event trigger id for each tower, -1 if irrelevant */ 
  integer4 errcode;                                     /* should be zero if there were no readout problems */
  integer4 yymmdd;		                        /* event year, month, day */
  integer4 hhmmss;		                        /* event hour minut second */
  integer4 usec;		                        /* event micro second */
  integer4 monyymmdd;                                   /* yymmdd at the beginning of the mon. cycle used in this event */
  integer4 monhhmmss;                                   /* hhmmss at the beginning of the mon. cycle used in this event */
  integer4 nofwf;		                        /* number of waveforms in the event */

  /* These arrays contain the waveform information */
  integer4 nretry[RUSDRAWMWF];                          /* number of retries to get the waveform */
  integer4 wf_id[RUSDRAWMWF];                           /* waveform id in the trigger */
  integer4 trig_code[RUSDRAWMWF];                       /* level 1 trigger code */
  integer4 xxyy[RUSDRAWMWF];	                        /* detector that was hit (XXYY) */
  integer4 clkcnt[RUSDRAWMWF];	                        /* Clock count at the waveform beginning */
  integer4 mclkcnt[RUSDRAWMWF];	                        /* max clock count for detector, around 50E6 */ 
  /* 2nd index: [0] - lower, [1] - upper layers */
  integer4 fadcti[RUSDRAWMWF][2];	                /* fadc trace integral, for upper and lower */
  integer4 fadcav[RUSDRAWMWF][2];                       /* FADC average */
  integer4 fadc[RUSDRAWMWF][2][rusdraw_nchan_sd];	/* fadc trace for upper and lower */

  /* Useful calibration information  */
  integer4 pchmip[RUSDRAWMWF][2];     /* peak channel of 1MIP histograms */
  integer4 pchped[RUSDRAWMWF][2];     /* peak channel of pedestal histograms */
  integer4 lhpchmip[RUSDRAWMWF][2];   /* left half-peak channel for 1mip histogram */
  integer4 lhpchped[RUSDRAWMWF][2];   /* left half-peak channel of pedestal histogram */
  integer4 rhpchmip[RUSDRAWMWF][2];   /* right half-peak channel for 1mip histogram */
  integer4 rhpchped[RUSDRAWMWF][2];   /* right half-peak channel of pedestal histograms */

  /* Results from fitting 1MIP histograms */
  integer4 mftndof[RUSDRAWMWF][2]; /* number of degrees of freedom in 1MIP fit */
  real8 mip[RUSDRAWMWF][2];        /* 1MIP value (ped. subtracted) */
  real8 mftchi2[RUSDRAWMWF][2];    /* chi2 of the 1MIP fit */
  
  /* 
     1MIP Fit function: 
     [3]*(1+[2]*(x-[0]))*exp(-(x-[0])*(x-[0])/2/[1]/[1])/sqrt(2*PI)/[1]
     4 fit parameters:
     [0]=Gauss Mean
     [1]=Gauss Sigma
     [2]=Linear Coefficient
     [3]=Overall Scalling Factor
  */
  real8 mftp[RUSDRAWMWF][2][4];    /* 1MIP fit parameters */
  real8 mftpe[RUSDRAWMWF][2][4];   /* Errors on 1MIP fit parameters */
  

} rusdraw_dst_common;

extern rusdraw_dst_common rusdraw_;

#endif
