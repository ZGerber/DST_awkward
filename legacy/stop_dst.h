/*
 * $Source: /hires_soft/uvm2k/bank/stop_dst.h,v $
 * $Log: stop_dst.h,v $
 * Revision 1.3  1995/08/22 23:33:24  vtodd
 * provided for conditional inclusion:  #ifndef _STOP_
 *
 * Revision 1.2  1995/03/18  00:35:23  jeremy
 * *** empty log message ***
 *
 * modified by jT at 5:10 PM on 3/13/95
*/

#ifndef _STOP_
#define _STOP_

#define STOP_BANKID 1400000101 
#define STOP_BANKVERSION 0 

integer4 stop_to_dst_(integer4 *NumUnit);

#endif
