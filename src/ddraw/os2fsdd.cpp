/*
 * Few routines for full-screen DirectDraw on OS/2
 *
 * Copyright 2000 Michal Necasek
 * Copyright 2000 Przemyslaw Dobrowolski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_DOS
#define INCL_WINWINDOWMGR
#include <os2wrap.h>
#include <misc.h>

#include <svgadefs.h>

#include "os2fsdd.h"

/* DirectX palette entry */
typedef struct tagPALETTEENTRY
{
   BYTE        peRed;
   BYTE        peGreen;
   BYTE        peBlue;
   BYTE        peFlags;
} PALETTEENTRY;

#define DLLNAME                 "VIDEOPMI"

#define REQUEST_ENTRYPOINT      "VIDEOPMI32Request"

#define FAIL_LENGTH             256

char PMIFILE[256] = "\\os2\\svgadata.pmi";

/*
 * Adapter instance.
 */
VIDEO_ADAPTER AdapterInstance;

/*
 * Entry point to videopmi
 */
PFNVIDEOPMIREQUEST __pfnPMIRequest;

//
// mode table. It is an array of VIDEOMODEINFOs.
//
static PVIDEOMODEINFO ModeTable;

static ULONG ulTotalModes;

static bIsInFS = FALSE;

/*
 * PMI request wrapper
 */
inline ULONG PMI_Request(PVIDEO_ADAPTER a, ULONG b, PVOID c, PVOID d)
{
   ULONG yyrc;
   USHORT sel = RestoreOS2FS();

   yyrc = __pfnPMIRequest(a, b, c, d);
   SetFS(sel);

   return yyrc;
}


/************************************************************
 * Load the .PMI file, get the hardware information about the
 * adapter and the entry point to videopmi.
 *
 * Returns 0 if successful; DOS error token, otherwise.
 ************************************************************/

ULONG LoadPMIService (VOID)
{
   APIRET        rc;
   char          sFail[FAIL_LENGTH] = {0};
   HMODULE       hmodVIDEOPMI;


/************************************************************
 * Load videopmi.dll
 ************************************************************/

   if (!(rc = DosLoadModule (sFail, FAIL_LENGTH, DLLNAME,
                            &hmodVIDEOPMI)))
   {

/************************************************************
 * Get PMIREQUEST entry point
 ************************************************************/

      rc = DosQueryProcAddr (hmodVIDEOPMI,
                             0,
                             REQUEST_ENTRYPOINT,
                             (PFN*)&__pfnPMIRequest);

      if (!rc)

         /*
          * Load PMI file.
          * If PMI file is successfully loaded,
          * adapter in AdapterInstance will be filled with the
          * information in .PMI file.
          *
          * Remember to set up the size information for ADAPTERINFO
          * and VIDEOMODEINFO.
          */

//          AdapterInstance.AdapterInfo.cb    = sizeof (ADAPTERINFO);
//          AdapterInstance.VideoModeInfo.cb  = sizeof (VIDEOMODEINFO);

         rc = PMI_Request (&AdapterInstance,
                           PMIREQUEST_LOADPMIFILE,
//                         PMIREQUEST_IDENTIFYADAPTER,
                           PMIFILE,
                           NULL);
//      if (rc)
         DosFreeModule (hmodVIDEOPMI);
   }

   return rc;
}


/************************************************************
 *
 * This function sets up the mode table.
 *
 * Copy the mode table from videopmi. It is an arrary of modes.
 * All the information in [ModeInfo] and
 * [MonitorModeInfo], if any, is included.
 *
 * Returns 0 if successful; DOS error token, otherwise.
 ************************************************************/
ULONG SetUpModeTable (VOID)
{
   APIRET        rc;
   /*
    * Get the total number of modes
    */
   if (!(rc = PMI_Request (&AdapterInstance,
                           PMIREQUEST_QUERYMAXMODEENTRIES,
                           NULL,
                           &ulTotalModes)))

      /*
       * Allocate memory for mode table
       */
      if (!(rc = DosAllocSharedMem ((PPVOID)&ModeTable,
                                    NULL,
                                    ulTotalModes *
                                    sizeof (VIDEOMODEINFO),
                                    OBJ_GETTABLE | PAG_COMMIT |
                                    PAG_WRITE)))

         /*
          * Copy mode table.
          * Please check svgadefs.h for the fields in VIDEOMODEINFO.
          */
         rc = PMI_Request (&AdapterInstance,
                           PMIREQUEST_QUERYMODEINFODATA,
                           NULL,
                           ModeTable);

   return rc;
}

ULONG FreeModeTable (VOID)
{
  DosFreeMem (ModeTable);
  return 0;
}


/************************************************************
 *
 * This function sets the graphic mode.
 *
 * You can select the mode based on any information in the VIDEOMODEINFO
 * structure. The following is only an example to set the graphics mode
 * based on resolution and refresh rate.
 * PM driver should not call videopmi to set the mode directly.
 * It should call BVH to set the mode as before, such that
 * the mode can be set based on the current monitor capability
 * handled by BVH.
 *
 * Returns 0 if successful; DOS error token, otherwise.
 ************************************************************/

ULONG SetSVGAMode (ULONG     ulHorRes,
                   ULONG     ulVerRes,
                   ULONG     ulColors,
                   ULONG     ulVerRefr,
                   PULONG    pulModeInd,
                   PVOID     pCLUTData)
{
    APIRET rc=0xFFFF;
    ULONG  i;

    *pulModeInd = -1;
    /* Search mode */
    /* Assuming that the modes are sorted by refresh mode in ascending */
    /* order, we'll find the highest refresh mode                      */
    if (ulVerRefr >= 0xFFL)
    {
      for(i=0; i < ulTotalModes; i++)
         if ((ModeTable[i].usXResolution == (USHORT)ulHorRes) &&
             (ModeTable[i].usYResolution == (USHORT)ulVerRes) &&
             (ModeTable[i].bBitsPerPixel == (BYTE)ulColors)) {
            *pulModeInd = i;
//            break;
         }
   }

   else    /* verify all including the refresh parameter */
   {
      for(i=0; i < ulTotalModes; i++)
         if ((ModeTable[i].usXResolution  == (USHORT) ulHorRes) &&
             (ModeTable[i].usYResolution  == (USHORT) ulVerRes) &&
             (ModeTable[i].bBitsPerPixel  == (BYTE) ulColors) &&
             ((ModeTable[i].bVrtRefresh   == 0xFF) ||
              (ModeTable[i].bVrtRefresh   == (BYTE) ulVerRefr)))
             *pulModeInd = i;
   }

   if (*pulModeInd == -1)
      return rc;              /* mode not found */

   /* Unlock first */
   rc = PMI_Request (&AdapterInstance,
                     PMIREQUEST_UNLOCKREGISTERS,
                     NULL,
                     NULL);

   /*
    * Copy VIDEOMODEINFO of the selected mode to AdapterInstance.
    * Depending on the .PMI file, this information may be needed.
    */
   AdapterInstance.ModeInfo = ModeTable[*pulModeInd];
   /*
    * Call videopmi to set the mode.
    */
   rc = PMI_Request (&AdapterInstance,
                     PMIREQUEST_SETMODE,
                     &ModeTable[*pulModeInd].miModeId,
                     NULL);

   if (rc)
      return rc;

   else
      /* Load Color Lookup Table */
      if (ModeTable[*pulModeInd].bBitsPerPixel <= 8 && pCLUTData != NULL)
         rc = PMI_Request (&AdapterInstance,
                           PMIREQUEST_SETCLUT,
                           pCLUTData,
                           NULL);


   return rc;
}

//******************************************************************************
//******************************************************************************
ULONG SetSVGAPalette (PVOID pal) {
   PALETTEENTRY *pDirectXPal = (PALETTEENTRY *)pal;
   struct {
      CLUTDATA  clData;
      SVGARGB   svgaRGB[255];
   } clutData;

   ULONG i, rc;

   dprintf(("DDRAW-OS2FSDD: Setting physical palette\n"));

   /* We have to clamp down the values - physical palette only goes to 64 */
   /* on standard VGA                                                     */
   for(i = 0; i < 256; i++) {
      clutData.clData.aRGB[i].bR      = pDirectXPal[i].peRed   >> 2;
      clutData.clData.aRGB[i].bG      = pDirectXPal[i].peGreen >> 2;
      clutData.clData.aRGB[i].bB      = pDirectXPal[i].peBlue  >> 2;
      clutData.clData.aRGB[i].bUnused = 0;
   }
   clutData.clData.ulRGBCount = 256;
   clutData.clData.ulRGBStart = 0;

   rc = PMI_Request (&AdapterInstance,
                     PMIREQUEST_SETCLUT,
                     &clutData,
                     NULL);
   return rc;
}

//******************************************************************************
//******************************************************************************
VOID KillPM(VOID)
{
  HAB  hab;
  HWND hwnd;
  HDC  hdc;
  ULONG rc;
  USHORT sel = RestoreOS2FS();

  hab = WinQueryAnchorBlock(HWND_DESKTOP);

  hwnd = WinQueryDesktopWindow(hab, NULL);

  // Grab the handle to the Desktop DC
  hdc = WinOpenWindowDC(hwnd);

  dprintf(("*** Killing the PM screen ***"));

  // Call GreDeath
  rc=GreDeath(hdc);

  SetFS(sel);

  // Set a flag so that a subsequent restore works
  bIsInFS = TRUE;

  dprintf(("GreDeath() rc= %x",rc));
}

//******************************************************************************
//******************************************************************************
VOID RestorePM(VOID)
{
  HAB   hab;
  HWND  hwnd;
  HDC   hdc;
  ULONG rc;
  USHORT sel = RestoreOS2FS();

  // Bail out if we aren't in FS mode
  if (!bIsInFS) {
      SetFS(sel);
      return;
  }

  // Get the anchor block
  hab = WinQueryAnchorBlock(HWND_DESKTOP);

  // Get the window handle to the Desktop
  hwnd = WinQueryDesktopWindow(hab, NULL);

  // Grab the handle to the Desktop DC
  hdc = WinQueryWindowDC(hwnd);

  dprintf(("*** Restore the PM screen ***"));

  rc=GreResurrection(hdc, 0L, NULL);

  // Repaint the desktop
  WinInvalidateRect(hwnd, NULL, TRUE);

  // Set a flag so that we know we're back in PM
  bIsInFS = FALSE;

  SetFS(sel);
  dprintf(("GreResurrection() rc=%x",rc));
}

//******************************************************************************
//******************************************************************************
BOOL ModeInSVGAModeTable(ULONG resX,ULONG resY, ULONG bpp)
{
  int i;
  for (i=0;i<ulTotalModes;i++)
  {
    if ((ModeTable[i].usXResolution==resX) &&
        (ModeTable[i].usYResolution==resY) &&
        (ModeTable[i].bBitsPerPixel==bpp)) return TRUE;
  }
  return FALSE;
}
