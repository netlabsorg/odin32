/* $Id: gerror.c,v 1.2 2001-09-05 14:30:51 bird Exp $ */
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
**
** $Header: /home/ktk/tmp/odin/2007/netlabs.cvs/odin32/src/opengl/glide/sst1/glide/gerror.c,v 1.2 2001-09-05 14:30:51 bird Exp $
** $Log: gerror.c,v $
** Revision 1.2  2001-09-05 14:30:51  bird
** Added $Id:$ keyword.
**
** Revision 1.1  2000/02/25 00:31:13  sandervl
** Created new Voodoo 1 Glide dir
**
 *
 * 10    6/03/97 11:57p Pgj
 * fix bug 558
 *
 * 9     5/28/97 2:10p Dow
 * Some mode setting, other cleanup
 *
 * 8     5/27/97 2:02p Dow
 * added _grAssert
 *
 * 7     5/20/97 9:47a Pgj
 * Use OutputDebugString for non-fatal errors under windows
 *
 * 6     5/19/97 7:35p Pgj
 * Print cogent error message if h/w not found
 *
 * 5     3/09/97 10:31a Dow
 * Added GR_DIENTRY for di glide functions
 *
 * 4     12/23/96 1:37p Dow
 * chagnes for multiplatform glide
**
*/
#include <stdio.h>

#if defined(__OS2__)
#  include <os2.h>
#endif

#ifdef __WIN32__
#  include <windows.h>
#endif

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

#if(GLIDE_PLATFORM & GLIDE_OS_DOS32)
#  include <malloc.h>
#if (GLIDE_PLATFORM & GLIDE_HW_SST96)
#  include <conio.h>
#  include <i86.h>
#endif
#endif

void (*GrErrorCallback)( const char *string, FxBool fatal );

void _doGrErrorCallback( const char *name, const char *msg, FxBool fatal )
{
  char buf[1024];
  sprintf(buf,"%s:  %s.\n", name, msg);
  GrErrorCallback(buf,fatal);
}

GR_DIENTRY(grErrorSetCallback, void,
           ( void (*function) ( const char *string, FxBool fatal ) ))
{
  GDBG_INFO((80,"grErrorSetCallback(0x%x)",function));
  GrErrorCallback = function;
}

#ifdef __WIN32__
void
_grErrorWindowsCallback( const char *s, FxBool fatal )
{
  if ( fatal )
  {
    GDBG_ERROR("glide",s);
    MessageBox(NULL, s, NULL, MB_OK);
    grSstWinClose();
    grGlideShutdown();
    exit(1);
  } else {
    OutputDebugString(s);
  }
}
#endif

#if defined(__OS2__)
void
_grErrorOs2Callback( const char *s, FxBool fatal )
{
  if ( fatal )
  {
    GDBG_ERROR("glide",s);
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, s, "glide error", 0, MB_OK);
    grSstWinClose();
    grGlideShutdown();
    exit(1);
  } else {
#ifdef GDBG_INFO_ON
    GDBG_PRINTF((s));
#else
    puts( s );
#endif
  }
}
#endif

void
_grErrorDefaultCallback( const char *s, FxBool fatal )
{
  if ( fatal )
  {
    grSstWinClose();
    grGlideShutdown();

#if (GLIDE_PLATFORM & GLIDE_HW_SST96) && (GLIDE_PLATFORM & GLIDE_OS_DOS32)
    {
      /* Set Mode 3 before going */
      union REGS
        inR, outR;

      inR.w.ax = 0x0003;
      int386(0x10, &inR, &outR);

    }
#endif
#ifdef GDBG_INFO_ON
    GDBG_ERROR("glide", s);
    GDBG_PRINTF((s));
#else
    puts( s );
#endif
    exit( 1 );
  }
  else
  {
#ifdef GDBG_INFO_ON
    GDBG_PRINTF((s));
#else
    puts( s );
#endif
  }
}

#ifdef __DOS__
int _guHeapCheck( void )
{
  int i = _heapchk();

  if ( i != _HEAPOK )
    printf( "heapchk: %d\n", i );

  return ( i == 0 );
}
#endif

void
_grAssert(char *exp, char *fileName, int lineNo)
{
  initRestoreVideo();

  GDBG_PRINTF(("ASSERTION FAILED:\n"));
  GDBG_PRINTF(("\tExpression:   %s\n", exp));
  GDBG_PRINTF(("\tFile:         %s\n", fileName));
  GDBG_PRINTF(("\tLine:         %d\n", lineNo));
  GDBG_PRINTF(("ABNORMAL TERMINATION\n"));

  exit(-1);

} /* _grAssert */
