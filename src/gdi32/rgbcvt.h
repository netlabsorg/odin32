//$Id: rgbcvt.h,v 1.1 2000-02-03 18:59:04 sandervl Exp $
#ifndef __RGBCVT_H__
#define __RGBCVT_H__

void _Optlink RGB555to565(WORD *dest, WORD *src, ULONG num);
void _Optlink RGB555to565MMX(WORD *dest, WORD *src, ULONG num);

#endif //__RGBCVT_H__