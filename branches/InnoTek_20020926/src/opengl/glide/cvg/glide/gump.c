/* $Id: gump.c,v 1.2 2001-09-05 14:30:30 bird Exp $ */
/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com).
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.
**
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.
**
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
** $Header: /home/ktk/tmp/odin/2007/netlabs.cvs/odin32/src/opengl/glide/cvg/glide/gump.c,v 1.2 2001-09-05 14:30:30 bird Exp $
** $Log: gump.c,v $
** Revision 1.2  2001-09-05 14:30:30  bird
** Added $Id:$ keyword.
**
** Revision 1.1  2000/02/25 00:37:42  sandervl
** Created Voodoo 2 dir
**
 *
 * 10    12/09/97 12:20p Peter
 * mac glide port
 *
 * 9     11/12/97 5:12p Pgj
 * stubs for evil guFb{Read|Write}Region() calls
 *
 * 8     5/27/97 1:16p Peter
 * Basic cvg, w/o cmd fifo stuff.
 *
 * 7     5/21/97 6:05a Peter
 *
 * 6     3/09/97 10:31a Dow
 * Added GR_DIENTRY for di glide functions
 *
 * 5     12/23/96 1:37p Dow
 * chagnes for multiplatform glide
**
*/

/* Implements multipass drawing */

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"
#include "gump.h"

/* CHD BUG this belongs in fxglide.h */
void FX_CSTYLE
_gumpTexCombineFunction( int virtual_tmu );

GrMPState _gumpState;

GR_DIENTRY(guMPInit, void, ( void ))
{
  int i;

  GDBG_INFO(98,"guMPInit()\n");
  for (i = 0; i < GLIDE_NUM_VIRTUAL_TMU; i += 1)
  {
    _gumpState.mmid[i] = (GrMipMapId_t) GR_NULL_MIPMAP_HANDLE;
  }
  _gumpState.tc_fnc = GR_MPTEXTURECOMBINE_ADD;
}

GR_DIENTRY(guMPTexCombineFunction, void,
           ( GrMPTextureCombineFnc_t tc_fnc ))
{
  GDBG_INFO(98,"guMPTexCombineFunction(%d)\n",tc_fnc);
  _gumpState.tc_fnc = tc_fnc;
}

GR_DIENTRY(guMPTexSource, void,
           ( GrChipID_t virtual_tmu, GrMipMapId_t mmid ))
{
  GR_DCL_GC;
  FXUNUSED( gc );
  GDBG_INFO(98,"guMPTexSource(%d,%d)\n",virtual_tmu,mmid);
  GR_CHECK_TMU("guMPTexSource",virtual_tmu);
  _gumpState.mmid[virtual_tmu] = mmid;
}

extern FX_ENTRY void FX_CALL
guFbReadRegion( const int srcX, const int srcY, const int w, const int h, const void *dst, const int strideInBytes );

extern FX_ENTRY void FX_CALL
guFbWriteRegion( const int dstX, const int dstY, const int w, const int h, const void *src, const int strideInBytes);


/*---------------------------------------------------------------------------
**  guFbReadRegion XXX obsolete
*/
GR_ENTRY(guFbReadRegion, void, ( const int srcX, const int srcY, const int w, const int h, const void *dst, const int strideInBytes ) )
{
}

/*---------------------------------------------------------------------------
**  guFbWriteRegion XXX obsolete
*/
GR_ENTRY(guFbWriteRegion, void, ( const int dstX, const int dstY, const int w, const int h, const void *src, const int strideInBytes))
{
}
