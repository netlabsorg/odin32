/* $Id: gerror.c,v 1.2 2001-09-05 14:30:25 bird Exp $ */
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
** $Header: /home/ktk/tmp/odin/2007/netlabs.cvs/odin32/src/opengl/glide/cvg/glide/gerror.c,v 1.2 2001-09-05 14:30:25 bird Exp $
** $Log: gerror.c,v $
** Revision 1.2  2001-09-05 14:30:25  bird
** Added $Id:$ keyword.
**
** Revision 1.1  2000/02/25 00:37:39  sandervl
** Created Voodoo 2 dir
**
**
** 28    3/14/98 1:07p Peter
** mac port happiness
**
** 27    2/11/98 5:23p Peter
** no more infinite recursion
 *
 * 26    12/18/97 2:13p Peter
 * cleaned up the error code
 *
 * 25    12/09/97 12:20p Peter
 * mac glide port
 *
 * 24    12/05/97 4:38p Peter
 * sli vs assertions
 *
 * 23    12/03/97 11:34a Peter
 * dos debugging
 *
 * 22    11/17/97 4:55p Peter
 * watcom warnings/chipfield stuff
 *
 * 21    11/15/97 8:55p Peter
 * Removed OutputDebugString
 *
 * 20    11/15/97 7:43p Peter
 * more comdex silliness
 *
 * 19    11/12/97 2:27p Peter
 *
 * 18    11/12/97 11:16a Peter
 * cleaned up assertions
 *
 * 17    11/04/97 5:04p Peter
 * cataclysm part deux
 *
 * 16    11/03/97 4:02p Peter
 * cataclysm fix
 *
 * 15    11/03/97 3:43p Peter
 * h3/cvg cataclysm
 *
 * 14    10/23/97 5:28p Peter
 * sli fifo thing
 *
 * 13    9/24/97 1:29p Peter
 * more assertion spewage
 *
 * 12    9/05/97 5:29p Peter
 * changes for direct hw
 *
 * 11    5/30/97 5:44p Peter
 * Version that does basic triangles/registers w/ command fifo. Does not
 * currently download textures correctly.
 *
 * 10    5/28/97 9:05a Peter
 * Merge w/ latest glide changes
 *
 * 9     5/27/97 1:16p Peter
 * Basic cvg, w/o cmd fifo stuff.
 *
 * 8     5/21/97 6:05a Peter
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
#ifdef __DOS__
#  include <malloc.h>
#endif

#ifdef __WIN32__
#  include <windows.h>
#endif

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"

void (*GrErrorCallback)( const char *string, FxBool fatal );

void _doGrErrorCallback( const char *name, const char *msg, FxBool fatal )
{
  char buf[1024];

#if GDBG_INFO_ON
  /* Neuter any fifo checking from the failing
   * call. Otherwise entries into the shutdown
   * calls cause spurious crap.
   */
  if (fatal) {
    GR_DCL_GC;

    gc->checkCounter     =
    gc->expected_counter = 0;

    gc->checkCounter =
    gc->checkPtr     = 0UL;
  }
#endif /* GDBG_INFO_ON */

  gdbg_printf("%s: %s.\n", name, msg);
  sprintf(buf,"%s: %s.\n", name, msg);
  GrErrorCallback(buf, fatal);

  if (fatal) exit(1);
}

GR_DIENTRY(grErrorSetCallback, void,
           ( void (*function) ( const char *string, FxBool fatal ) ))
{
  GDBG_INFO(80,"grErrorSetCallback(0x%x)",function);
  GrErrorCallback = function;
}

#ifdef __WIN32__
void
_grErrorWindowsCallback( const char *s, FxBool fatal )
{
  if ( fatal ) {
    GDBG_ERROR("glide", s);
    grGlideShutdown();
    MessageBox(NULL, s, NULL, MB_OK);
  }
}
#endif

void
_grErrorDefaultCallback( const char *s, FxBool fatal )
{
  if ( fatal ) {
    GDBG_ERROR("glide",s);
    grGlideShutdown();

#if (GLIDE_PLATFORM & GLIDE_OS_MACOS)
        {
            Str255 errBuf;

            errBuf[0] = sprintf((char*)(errBuf + 1), "%s", s);
            DebugStr(errBuf);
        }
#endif /* (GLIDE_PLATFORM * GLIDE_OS_MACOS) */
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
  GR_DCL_GC;
  GR_DCL_HW;

  /* No recursive re-entry thank you very much */
  {
    static FxBool inProcP = FXFALSE;
    if (inProcP) return;
    inProcP = FXTRUE;
  }

  /* dpc - 28 may 1997 - FixMe!
   * What to do here?
   */
#if GLIDE_INIT_HAL

#else /* !GLIDE_INIT_HAL */
  /* initRestoreVideo(); */
  sst1InitShutdown(gc->base_ptr);
#endif /* !GLIDE_INIT_HAL */

  gdbg_printf("ASSERTION FAILED:\n");
  gdbg_printf("\tExpression:   %s\n", exp);
  gdbg_printf("\tFile:         %s\n", fileName);
  gdbg_printf("\tLine:         %d\n", lineNo);

#if (GLIDE_PLATFORM & GLIDE_HW_CVG) && USE_PACKET_FIFO
  /* Spew about the state of the fifo since that's what most of the
   * assertions are about anyway.
   */
  {
    gdbg_printf("Command Fifo:\n");
    gdbg_printf("\tfifoPtr:    0x%X\n"
                "\tfifoParams: 0x%X : (0x%X : 0x%X)\n"
                "\tfifoRoom:  0x%X : (0x%X : 0x%X)\n"
                "\tfifo hw: (0x%X : 0x%X) : (0x%X : 0x%X : 0x%X)\n"
                "\tStatus: 0x%X\n",
                gc->cmdTransportInfo.fifoPtr,
                gc->cmdTransportInfo.fifoStart, gc->cmdTransportInfo.fifoEnd,
                gc->cmdTransportInfo.fifoSize,
                gc->cmdTransportInfo.fifoRoom,
                gc->cmdTransportInfo.roomToReadPtr, gc->cmdTransportInfo.roomToEnd,
                HW_FIFO_PTR(FXTRUE), gc->cmdTransportInfo.fifoRead,
                GR_GET(hw->cmdFifoDepth), GR_GET(hw->cmdFifoHoles), GR_GET(hw->cmdFifoBase),
                GR_GET(hw->status));

#if !GLIDE_INIT_HAL
    if (gc->scanline_interleaved) {
      SstRegs* slaveHw = (SstRegs*)gc->slave_ptr;

      GDBG_INFO_MORE(gc->myLevel, "\tsli: 0x%X : (0x%X : 0x%X : 0x%X)\n",
                     HW_FIFO_PTR(FXFALSE),
                     GR_GET(slaveHw->cmdFifoDepth),
                     GR_GET(slaveHw->cmdFifoHoles),
                     GR_GET(slaveHw->status));
    }
#endif /* !GLIDE_INIT_HAL */

#if GLIDE_USE_SHADOW_FIFO
    if (gc->cmdTransportInfo.fifoShadowBase != NULL) {
      const FxU32* fifoPtr = gc->cmdTransportInfo.fifoShadowBase;

      GDBG_PRINTF("Shadow Fifo: 0x%X\n", gc->cmdTransportInfo.fifoShadowPtr);

#if 0
      while(fifoPtr != gc->cmdTransportInfo.fifoShadowPtr) GDBG_PRINTF("0x%X\n", *fifoPtr++);
      GDBG_PRINTF("\n");

      GDBG_PRINTF("Up to fifo wrap: \n");
      while(fifoPtr < gc->cmdTransportInfo.fifoShadowBase + (kDebugFifoSize >> 2))
        GDBG_PRINTF("0x%X\n", *fifoPtr++);
#endif

      free(gc->cmdTransportInfo.fifoShadowBase);
    }
#endif /* GLIDE_USE_SHADOW_FIFO */
  }
#endif /* (GLIDE_PLATFORM & GLIDE_HW_CVG) && USE_PACKET_FIFO */

  gdbg_printf("ABNORMAL TERMINATION\n");

  exit(-1);

} /* _grAssert */
