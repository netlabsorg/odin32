/* $Id: os2ddrawmodes.h,v 1.4 2001-03-18 21:44:46 mike Exp $ */

/*
 * Defintions of common videomodes we report back
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2DDRAWMODES__
#define __OS2DDRAWMODES__


// Modes reported back during enum
typedef struct _OS2_DXMODES
{
  int iXRes;    // # of Pixel X-Res
  int iYRes;    // # of Pixel Y-Res
  int iBits;    // # of Bits/Pixel
  int iRatio;   // Flag 1 if you must take care of aspect ratio, else 0
} OS2_DXMODES, *POS2_DXMODES;

#define NUM_MODES_DIVE 30
OS2_DXMODES ModesDive[NUM_MODES_DIVE] =
{
  {320,200,8,1},
  {320,240,8,0},
  {512,384,8,0},
  {640,400,8,1},
  {640,480,8,0},
  {800,600,8,0},
  {1024,768,8,0},
  {1280,1024,8,1},
  {1600,1200,8,0},
  {512,384,16,0},
  {640,400,16,1},
  {640,480,16,0},
  {800,600,16,0},
  {1024,768,16,0},
  {1280,1024,16,1},
  {1600,1200,16,0},
  {512,384,24,0},
  {640,400,24,1},
  {640,480,24,0},
  {800,600,24,0},
  {1024,768,24,0},
  {1280,1024,24,1},
  {1600,1200,24,0},
  {512,384,32,0},
  {640,400,32,1},
  {640,480,32,0},
  {800,600,32,0},
  {1024,768,32,0},
  {1280,1024,32,1},
  {1600,1200,32,0}
};

// Voodoo modes (640x400,640x480,800x600 all only 16 Bit)
// Voodoo mode 800x600 will only be Z-buffered for Voodoo II cards

#define NUM_MODES_VOODOO 4
OS2_DXMODES ModesVoodoo[NUM_MODES_VOODOO] =
{
  {512,384,16,0},
  {640,400,16,1},
  {640,480,16,0},
  {800,600,16,0}
};

#endif
