//$Id: rgbcvt.h,v 1.4 2003-07-16 10:46:18 sandervl Exp $
#ifndef __RGBCVT_H__
#define __RGBCVT_H__

#define RGB555_RED_MASK  	  0x7c00
#define RGB555_GREEN_MASK  	  0x03e0
#define RGB555_BLUE_MASK   	  0x001f

#define RGB565_RED_MASK  	  0xF800
#define RGB565_GREEN_MASK  	  0x07e0
#define RGB565_BLUE_MASK   	  0x001f

//Default 16 bpp = RGB555
#define DEFAULT_16BPP_RED_MASK	  RGB555_RED_MASK
#define DEFAULT_16BPP_GREEN_MASK  RGB555_GREEN_MASK
#define DEFAULT_16BPP_BLUE_MASK   RGB555_BLUE_MASK

//Default 24/32 bpp = RGB888
#define DEFAULT_24BPP_RED_MASK	  0xff0000
#define DEFAULT_24BPP_GREEN_MASK  0x00ff00
#define DEFAULT_24BPP_BLUE_MASK   0x0000ff

#define DEFAULT_32BPP_RED_MASK	  DEFAULT_24BPP_RED_MASK
#define DEFAULT_32BPP_GREEN_MASK  DEFAULT_24BPP_GREEN_MASK
#define DEFAULT_32BPP_BLUE_MASK   DEFAULT_24BPP_BLUE_MASK


void _Optlink RGB555to565(WORD *dest, WORD *src, ULONG num);
void _Optlink RGB555to565MMX(WORD *dest, WORD *src, ULONG num);

void _Optlink RGB565to555(WORD *dest, WORD *src, ULONG num);
void _Optlink RGB565to555MMX(WORD *dest, WORD *src, ULONG num);

extern void (_Optlink *pRGB555to565)(WORD *dest, WORD *src, ULONG num);
extern void (_Optlink *pRGB565to555)(WORD *dest, WORD *src, ULONG num);

#endif //__RGBCVT_H__