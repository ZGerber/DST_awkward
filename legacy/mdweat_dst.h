/* Created 2016/01/21 DI */
/* Last Updated 2020/03/09 DI */

#ifndef _MDWEAT_DST_
#define _MDWEAT_DST_

#include <stdio.h>
#include "dst_std_types.h"

#define MDWEAT_BANKID		15008
#define MDWEAT_BANKVERSION	0

#ifdef __cplusplus
extern "C" {
#endif
integer4 mdweat_common_to_bank_ ();
integer4 mdweat_bank_to_dst_ (integer4 * NumUnit);
integer4 mdweat_common_to_dst_ (integer4 * NumUnit);   /* combines above 2 */
integer4 mdweat_bank_to_common_ (integer1 * bank);
integer4 mdweat_common_to_dump_ (integer4 * opt1);
integer4 mdweat_common_to_dumpf_ (FILE * fp, integer4 * opt2);
/* get (packed) buffer pointer and size */
integer1* mdweat_bank_buffer_ (integer4* mdweat_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  
  integer4 part_num;  // MD Part number for the MD event
  // n e s w o t h
  // n = 1,  0 Clouds North?
  // e = 1,  0 Clouds East?
  // s = 1,  0 Clouds South?
  // w = 1,  0 Clouds West?
  // o = 0 - 4 Overhead cloud thickness? 5 - weat code invalid
  // t = 1,  0 Stars visible? 
  // h = 1,  0 Was it hazy? 2 - can't tell 
  integer4 code;      // 7-digit weather code recorded by runners
  
} mdweat_dst_common;

extern mdweat_dst_common mdweat_;


#endif
