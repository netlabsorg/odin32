//$Id: rgbcvt.h,v 1.2 2000-08-18 18:14:58 sandervl Exp $
#ifndef __RGBCVT_H__
#define __RGBCVT_H__

void _Optlink RGB555to565(WORD *dest, WORD *src, ULONG num);
void _Optlink RGB555to565MMX(WORD *dest, WORD *src, ULONG num);

void _Optlink RGB565to555(WORD *dest, WORD *src, ULONG num);
void _Optlink RGB565to555MMX(WORD *dest, WORD *src, ULONG num);

#endif //__RGBCVT_H__