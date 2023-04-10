/* $Id: clipboard.cpp,v 1.19 2004-05-03 12:09:00 sandervl Exp $ */

/*
 * Win32 Clipboard API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 2004 InnoTek Systemberatung GmbH
 *
 * NOTE: Use OS/2 frame handles for Open32 functions
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/** @page   user32_clipboard    Clipboard
 *
 * The clipboard implemenation is based on using the OS/2 clipboard, sharing
 * the few compatible formats while registering private formats for the rest.
 *
 * The OS/2 and Windows clipboards are very similar although the Windows one
 * have evolved a bit further than the OS/2 one. Still, they send basically
 * the same window messages to the clipboard owner and viewer. They both support
 * having multiple formats present at the same time.
 *
 * Windows however, have a feature for automatic synthesizing of alternative
 * data formats. For instance if there is one of the CF_TEXT, CF_UNICODETEXT,
 * CF_OEMTEXT formats put onto the clipboard, the other two will be synthesized.
 * Similar for CF_BITMAP, CF_DIB and CF_DIBV5. CF_DIB and CF_DIBV5 also causes
 * CF_PALETTE to be synthesized. CF_METAFILEPICT and CF_ENHMETAFILE are also
 * synthesized when the other is present.
 *
 * The Odin32 clipboard implementation will support synthesizing all but the
 * metafiles and palette in it's current incarnation.
 *
 * CF_LOCALE is a special format which is generated when text is put onto the
 * clipboard. It contains a MAKELCID DWORD. It's will be generated when the
 * clipboard is closed since no other process (or thread for that matter) can
 * access the clipboard when it's opened by someone else. CF_LOCALE will be
 * faked if OS/2 put some text onto the clipboard.
 *
 * The ranges of private and gdi formats are registered with PM on demand
 * to limit resource waste and minimize startup penalty.
 *
 * The synthesized format are reported, and we store a special fake handle
 * for those which haven't yet been converted. This can of course only be
 * done for formats which we do not share with PM; CF_TEXT, CF_BITMAP,
 * and CF_PALETTE.
 *
 * When data is retrieved from the clipboard a copy is made for the calling
 * process. In windows EmptyClipboard() will cleanup only in the global
 * data and the local copies in the calling process. That means other
 * processes which have retrieved data from the clipboard will not have
 * their data invalidated till they them self call EmptyClipboard().
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <string.h>
#include <algorithm>

#include "win32wbase.h"
#include <unicode.h>
#include <stdio.h>
#include <ctype.h>
#include "oslibclipbrd.h"
#include "oslibutil.h"
#include "oslibwin.h"

/* missing somewhere */
#ifndef LCS_sRGB
#define LCS_sRGB        0x73524742  //'RGBs'
#endif

#define DBG_LOCALLOG    DBG_clipboard
#include "dbglocal.h"



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/** Define this to return the V5 DIB headers to the application.
 * If not defined we will not convert anything to DIBV5 headers, but we'll still
 * accept DIBV5 headers from the application.
 */
#define USE_DIBV5   1


/**
 * Debug assertion macro.
 * @param   expr    Assert that this expression is true.
 * @param   msg     Message to print if expr isn't true. It's given to dprintf,
 *                  and must be inclosed in paratheses.
 * @todo move this to some header in /include.
 */
#ifdef DEBUG
    #define DebugAssert(expr, msg) \
    do { if (expr) break; \
         dprintf(("!!!ASSERTION FAILED!!!\nFILE=%s\nLINE=%d\nFUNCTION=%s\n", __FILE__, __LINE__, __FUNCTION__)); \
         dprintf(msg); DebugInt3(); \
       } while (0)
#else
    #define DebugAssert(expr, msg) do {} while (0)
#endif

/**
 * Debug assertion failed macro.
 * @param   msg     Message to print if expr isn't true. It's given to dprintf,
 *                  and must be inclosed in paratheses.
 * @todo move this to some header in /include.
 */
#ifdef DEBUG
    #define DebugAssertFailed(msg) \
    do { dprintf(("!!!ASSERTION FAILED!!!\nFILE=%s\nLINE=%d\nFUNCTION=%s\n", __FILE__, __LINE__, __FUNCTION__)); \
         dprintf(msg); DebugInt3(); \
       } while (0)
#else
    #define DebugAssertFailed(msg) do {} while (0)
#endif


/** Format name strings.
 * @{ */
#define SZFMT_PRIVATE_PREFIX    "Odin32 Private #"
#define SZFMT_GDIOBJ_PREFIX     "Odin32 GDI Object #"
/** @} */

/** Dummy handle used for synthesized formats. */
#define DUMMY_HANDLE            0xbeef0001


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/** Dynamically registered formats, and the PRIVATE and GDI predefined ranges. */
typedef struct _ClipboardDynamicFormatMapping
{
    /** Odin32 Format. */
    UINT        uOdin;
    /** OS/2 format. */
    ULONG       ulPM;
    /** Name for registered formats. (NULL for predefined) */
    const char *pszName;
    /** Pointer to the next in the list. */
    struct _ClipboardDynamicFormatMapping *pNext;
} CLIPDYNFORMMAP, *PCLIPDYNFORMMAP;

/** Node in the list of local clipboard data. (the get-cache) */
typedef struct _ClipboardLocalData
{
    /** Data type. */
    enum
    {
        /** GlobalAlloc'ed object. */
        enmGlobalMem,
        /** GDI data - bitmap or palette. */
        enmGDI,
        /** Private data owner cleans up. */
        enmPrivate
    }   enmType;
    /** Clipboard format Format. */
    UINT        uFormat;
    /** Handle / pointer to data. */
    HANDLE      h;
    /** Pointer to the next node in the list. */
    struct _ClipboardLocalData *pNext;
} CLIPLOCALDATA, *PCLIPLOCALDATA;


/** Header for Odin32 specific clipboard entries.
 * (Used to get the correct size of the data.)
 */
typedef struct _Odin32ClipboardHeader
{
    /** magic number */
    char        achMagic[8];
    /** Size of the following data.
     * (The interpretation depends on the type.) */
    unsigned    cbData;
    /** Odin32 format number. */
    unsigned    uFormat;
} CLIPHEADER, *PCLIPHEADER;

#define CLIPHEADER_MAGIC "Odin\1\0\1"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** Static mappings. */
static struct
{
    /** Odin32 Format. */
    UINT        uOdin;
    /** OS/2 format. */
    ULONG       ulPM;
    /** Format name for registration. */
    const char *pszRegString;
    /** Description. (debug) */
    const char *pszDescr;
}                   gaFormats[] =
{
    { CF_TEXT           , OSLIB_CF_TEXT     , NULL                                          , "CF_TEXT"},
    { CF_BITMAP         , OSLIB_CF_BITMAP   , NULL                                          , "CF_BITMAP"},
    { CF_PALETTE        , OSLIB_CF_PALETTE  , NULL                                          , "CF_PALETTE"},
    { CF_LOCALE         , ~0                , "Odin32 Locale"                               , "CF_LOCALE"},
    { CF_UNICODETEXT    , ~0                , "Odin32 UnicodeText"                          , "CF_UNICODETEXT"},
    { CF_OEMTEXT        , ~0                , "Odin32 OEM Text"                             , "CF_OEMTEXT"},
    { CF_DIB            , ~0                , "Odin32 Device Independent Bitmap (DIB)"      , "CF_DIB"},
    { CF_DIBV5          , ~0                , "Odin32 Device Independent Bitmap V5 (DIBV5)" , "CF_DIBV5"},
    { CF_METAFILEPICT   , ~0                , "Odin32 MetaFilePict"                         , "CF_METAFILEPICT"},
    { CF_ENHMETAFILE    , ~0                , "Odin32 Enh. MetaFile"                        , "CF_ENHMETAFILE"},
    { CF_TIFF           , ~0                , "Odin32 Tagged Image Format (TIFF)"           , "CF_TIFF"},
    { CF_DIF            , ~0                , "Odin32 Data Interchange Format (SYLK)"       , "CF_DIF"},
    { CF_HDROP          , ~0                , "Odin32 HDROP"                                , "CF_HDROP"},
    { CF_OWNERDISPLAY   , ~0                , "Odin32 OwnerDisplay"                         , "CF_OWNERDISPLAY"},
    { CF_PENDATA        , ~0                , "Odin32 PenData"                              , "CF_PENDATA"},
    { CF_SYLK           , ~0                , "Odin32 Symlink Format"                       , "CF_SYLK"},
    { CF_RIFF           , ~0                , "Odin32 RIFF"                                 , "CF_RIFF"},
    { CF_WAVE           , ~0                , "Odin32 Wave"                                 , "CF_WAVE"},
    { CF_DSPMETAFILEPICT, ~0                , "Odin32 DSP MetaFilePict"                     , "CF_DSPMETAFILEPICT"},
    { CF_DSPENHMETAFILE , ~0                , "Odin32 DSP Enh. MetaFile"                    , "CF_DSPENHMETAFILE"},
    { CF_DSPTEXT        , OSLIB_CF_DSPTEXT  , NULL                                          , "CF_DSPTEXT"},
    { CF_DSPBITMAP      , OSLIB_CF_DSPBITMAP, NULL                                          , "CF_DSPBITMAP"},
};

/** List of dynamically registered formats. (should perhaps be protected for updates...)
 * This is used for the CF_PRIVATE, CF_GDIOBJ and registered formats. */
PCLIPDYNFORMMAP     gpDynFormats;

/** List of locally allocated objects.
 * SetClipboardData and GetClipboardData adds to this list.
 * This list is freed by EmptyClipboard.
 * (Access to this list is serialized by the clipboard semaphore in PM.)
 */
PCLIPLOCALDATA      gpLocalData;

/** The Odin window handle for the last OpenClipboard() call.
 * We need to keep this around for the EmptyClipboard() so we can set
 * the correct clipboard owner.
 */
HWND                ghwndOpenClipboard;
/** The OS/2 PM Window handle for ghwndOpenClipboard. */
HWND                ghwndOS2OpenClipboard;
/** The Odin TID of the current clipboard (Odin only) Read/Write Owner.
 * OpenClipboard and CloseClipboard updates while being behind the PM Clipboard
 * sempahore. If ASSUMES noone calls Open32 or PM apis directly to close the
 * clipboard. */
DWORD               gtidOpenClipboardThreadId;

/** Whether or not the current OpenClipboard session have changed the clipboard.
 * Used by CloseClipboard(). */
BOOL                gfClipboardChanged;

/** Whether or not the current OpenClipboard session have put text onto the clipboard.
 * Used to determin if CF_LOCALE can be generated correctly or not.
 */
BOOL                gfClipboardChangedText;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#ifdef DEBUG_LOGGING
static const char *dbgGetFormatName(UINT uFormat);
static void        dbgDumpClipboardData(const char *pszPrefix, UINT uFormat, HANDLE hData);
#else
    #define     dbgGetFormatName(uFormat) \
                ("<!non debug logging mode!>")
    #define     dbgDumpClipboardData(pszPrefix, uFormat, hClibObj) \
                do { } while(0)
#endif
extern "C" {
UINT WIN32API   clipboardPMToOdinFormat(ULONG ulPMFormat);
ULONG WIN32API  clipboardOdinToPMFormat(UINT uOdinFormat);
}
static ULONG    clipboardAddDynFormat(UINT uFormat, const char *pszFormatName);
static BOOL     clipboardIsRWOwner(void);
static BOOL     clipboardAddPM(UINT uOdinFormat, void *pv, unsigned cb);
static BOOL     clipboardAddPMHeader(UINT uOdinFormat, void *pv, unsigned cb);
static BOOL     clipboardAddPMHandle(UINT uOdinFormat, HANDLE h);
static void *   clipboardAllocPMBuffer(unsigned cb);
static BOOL     clipboardAddPMDummy(UINT uOdinFormat);
static ULONG    clipboardGetPMDataSize(void *pvData);
static BOOL     clipboardIsAvailable(UINT uFormat);
static BOOL     clipboardIsAvailableReal(UINT uFormat);
static void     clipboardSynthesize(BOOL fHaveModifiedIt);
static BOOL     clipboardShouldBeSynthesized(UINT uFormat);
static HANDLE   clipboardSynthesizeText(UINT uFormat);
static HANDLE   clipboardSynthesizeLocale(UINT uFormat);
static HANDLE   clipboardSynthesizeBitmap(UINT uFormat);
static HANDLE   clipboardSynthesizeDIB(UINT uFormat);
#if 0 // not implemented yet
static HANDLE   clipboardSynthesizePalette(UINT uFormat);
static HANDLE   clipboardSynthesizeMetaFile(UINT uFormat);
static HANDLE   clipboardSynthesizeEnhMetaFile(UINT uFormat);
#endif
static unsigned clipboardTextCalcSize(void *pvData, unsigned cb);
unsigned        clipboardGetCodepage(UINT uType);
static HANDLE   clipboardCacheAllocGlobalDup(UINT uFormat, const void *pv, unsigned cb, PCLIPLOCALDATA *ppClip);
static HANDLE   clipboardCacheAllocGlobalAlloc(UINT uFormat, unsigned cb, PCLIPLOCALDATA *ppClip);
static BOOL     clipboardCacheAllocGlobalMem(UINT uFormat, HANDLE hMem, PCLIPLOCALDATA *ppLocalClip);
static BOOL     clipboardCacheAllocGDI(UINT uFormat, HANDLE hGDI, PCLIPLOCALDATA *ppLocalClip);
//static HANDLE   clipboardCacheAllocPrivate(UINT uFormat, HANDLE hPrivate);
static HANDLE   clipboardCacheInsertNode(PCLIPLOCALDATA p);
static void     clipboardCacheDeleteNode(PCLIPLOCALDATA p);
static void     clipboardPMBitmapDelete(HBITMAP hbmp);
static HBITMAP  clipboardPMBitmapDuplicate(HBITMAP hbmp);
static UINT     clipboardEnumClipboardFormats(UINT uFormat);



/**
 * Converts a PM clipboard format number to an Odin format.
 *
 * @returns Odin format number on success.
 * @returns 0 on failure.
 * @param   ulPMFormat  PM Clipboard format number.
 */
UINT WIN32API clipboardPMToOdinFormat(ULONG ulPMFormat)
{
    for (int i = 0; i < sizeof(gaFormats) / sizeof(gaFormats[0]); i++)
        if (gaFormats[i].ulPM == ulPMFormat)
            return gaFormats[i].uOdin;
    for (PCLIPDYNFORMMAP p = gpDynFormats; p; p = p->pNext)
        if (p->ulPM == ulPMFormat)
            return p->uOdin;

    /*
     * Now this becomes interesting.
     *
     * We need to check if this is one of the dynamically registered
     * GDI formats or private formats. Another odin process can have
     * started using it and we should see it.
     *
     * We can tell by the atom name.
     */
    char    szAtomName[64];
    ULONG   cch;
    cch = OSLibWinQueryAtomName(OSLibWinQuerySystemAtomTable(), ulPMFormat, &szAtomName[0], sizeof(szAtomName));
    if (cch > 0)
    {
         char *psz;
         if (!strncmp(szAtomName, SZFMT_GDIOBJ_PREFIX, sizeof(SZFMT_GDIOBJ_PREFIX) - 1))
             psz = szAtomName + sizeof(SZFMT_GDIOBJ_PREFIX) - 1;
         else if (!strncmp(szAtomName, SZFMT_PRIVATE_PREFIX, sizeof(SZFMT_PRIVATE_PREFIX) - 1))
             psz = szAtomName + sizeof(SZFMT_PRIVATE_PREFIX) - 1;
         else
             return 0;                  /* not found! */
         /* parse the number. */
         UINT uRet = atoi(psz);
         if (uRet >= CF_PRIVATEFIRST && uRet <= CF_GDIOBJLAST)
         {
             ULONG ulPMAdded = clipboardAddDynFormat(uRet, NULL);
             DebugAssert(ulPMAdded == ulPMFormat,
                         ("ARG! Didn't get the same value for the same atoms! odin32=%d pm=%d pmadd=%d name='%s'\n",
                          uRet, ulPMFormat, ulPMAdded, szAtomName));
             return uRet;
         }
    }

    return 0; /* not found. */
}


/**
 * Adds a dynamicly managed Odin clipboard format to the internal list.
 *
 * @returns PM format number.
 * @returns 0 on failure.
 * @param   uFormat         Odin clipboard format number.
 * @param   pszFormatName   The format name.
 *                          For the two predefined ranges this argument is NULL.
 * @todo    serialize this!
 */
ULONG clipboardAddDynFormat(UINT uFormat, const char *pszFormatName)
{
    char szFormat[64];

    /*
     * Generate name for the predefined ones.
     */
    if (!pszFormatName)
    {
        if (uFormat >= CF_PRIVATEFIRST && uFormat <= CF_PRIVATELAST)
            sprintf(szFormat, SZFMT_PRIVATE_PREFIX "%u", uFormat);
        else if (uFormat >= CF_GDIOBJFIRST && uFormat <= CF_GDIOBJLAST)
            sprintf(szFormat, SZFMT_GDIOBJ_PREFIX "%u", uFormat);
        else
        {
            DebugAssertFailed(("Invalid format range %u\n", uFormat));
            return 0;
        }
        pszFormatName = szFormat;
    }

    /*
     * Register atom
     */
    ULONG ulPMFormat = OSLibWinAddAtom(OSLibWinQuerySystemAtomTable(), pszFormatName);
    if (ulPMFormat == 0)
    {
        DebugAssertFailed(("Faild to add atom '%s'. last error=%d\n", pszFormatName, OSLibWinGetLastError()));
        return 0;
    }

    /*
     * Create a dynamic structure and insert it into the LIFO.
     */
    PCLIPDYNFORMMAP p = (PCLIPDYNFORMMAP)malloc(sizeof(*p));
    if (!p)
        return 0;
    p->pszName = pszFormatName != szFormat ? strdup(pszFormatName) : NULL;
    p->ulPM    = ulPMFormat;
    p->uOdin   = uFormat;
    p->pNext   = gpDynFormats;
    gpDynFormats = p;

    dprintf(("USER32: clipboardAddDynFormat: Added odin=%d pm=%d name='%s'\n", uFormat, ulPMFormat, pszFormatName));
    return ulPMFormat;
}


/**
 * Converts a PM clipboard format number to an Odin format.
 *
 * @returns Odin format number on success.
 * @returns 0 on failure.
 * @param   ulPMFormat  PM Clipboard format number.
 */
ULONG WIN32API clipboardOdinToPMFormat(UINT uOdinFormat)
{
    if (uOdinFormat == 0)
        return 0;
    for (int i = 0; i < sizeof(gaFormats) / sizeof(gaFormats[0]); i++)
        if (gaFormats[i].uOdin == uOdinFormat)
            return gaFormats[i].ulPM;
    for (PCLIPDYNFORMMAP p = gpDynFormats; p; p = p->pNext)
        if (p->uOdin == uOdinFormat)
            return p->ulPM;

    /*
     * Now this becomes interesting.
     *
     * A format showed up which we don't know yet. It could be one
     * of the built in GDIOBJ or PRIVATE ones, if so we should
     * register them in the dynamic list.
     */
    if (uOdinFormat >= CF_PRIVATEFIRST && uOdinFormat <= CF_GDIOBJLAST)
        return clipboardAddDynFormat(uOdinFormat, NULL);
    return 0; /* not found. */
}


#ifdef DEBUG_LOGGING
/**
 * Gets the name of a format.
 *
 * @returns Pointer to const
 * @param   uFormat     Format number.
 * @remark  Note that for non standard formats a pointer to a static buffer is
 *          returned. This may be messed up if several threads are doing this
 *          at the same time. (fat chance)
 */
const char *dbgGetFormatName(UINT uFormat)
{
    static char szBuffer[256];
    for (int i = 0; i < sizeof(gaFormats) / sizeof(gaFormats[0]); i++)
        if (gaFormats[i].uOdin == uFormat)
            return gaFormats[i].pszDescr;

    for (PCLIPDYNFORMMAP p = gpDynFormats; p; p = p->pNext)
        if (p->uOdin == uFormat)
        {
            if (p->pszName)
                return p->pszName;     /* registered */
            if (uFormat >= CF_PRIVATEFIRST && uFormat <= CF_PRIVATELAST)
                sprintf(szBuffer, "CF_PRIVATE no.%d", uFormat - CF_PRIVATEFIRST);
            else if (uFormat >= CF_GDIOBJFIRST && uFormat <= CF_GDIOBJLAST)
                sprintf(szBuffer, "CF_PRIVATE no.%d", uFormat - CF_GDIOBJFIRST);
            DebugInt3();
            break;
        }

    sprintf(szBuffer, "Unknown format %#x", uFormat);
    return szBuffer;
}

/**
 * Debug helper which dumps clipboard data.
 *
 * @param   pszPrefix   Message prefix.
 * @param   uFormat     Data format of hData.
 * @param   hData       Global memory handle.
 */
void dbgDumpClipboardData(const char *pszPrefix, UINT uFormat, HANDLE hData)
{
    LPVOID pv = NULL;
    DWORD  cb = 0;

    /*
     * Get the data.
     */
    switch (uFormat)
    {
        /* handles */
        case CF_BITMAP:
        case CF_METAFILEPICT:
        case CF_DSPBITMAP:
        case CF_DSPMETAFILEPICT:
        case CF_ENHMETAFILE:
        case CF_HDROP:
        case CF_PALETTE:
            pv = NULL;
            break;

            /* all the rest */
        default:
            if (uFormat >= CF_PRIVATEFIRST && uFormat <= CF_PRIVATELAST)
                pv = NULL;
            else
            {
                if (hData)
                {
                    pv = GlobalLock(hData);
                    cb = GlobalSize(hData);
                }
                else
                    pv = NULL;
            }
            break;
    }
    dprintf(("%s: uFormat=%#x (%s) hData=%#x {locked=%p size=%d}\n",
             pszPrefix, uFormat, dbgGetFormatName(uFormat), hData, pv, cb));

    /*
     * Dump the data.
     */
    if (pv)
    {
        switch (uFormat)
        {
            case CF_TEXT:
            case CF_DSPTEXT:
                dprintf(("%s: '%s'", pszPrefix, (char*)pv));
                if (strlen((char *)pv) + 1 != cb)
                    dprintf(("%s: not properly null terminated? strlen()->%d", pszPrefix, strlen((char *)pv)));
                break;
            case CF_UNICODETEXT:
                dprintf(("%s: '%ls'", pszPrefix, (wchar_t*)pv));
                if (lstrlenW((LPCWSTR)pv) + 1 != cb / sizeof(wchar_t))
                    dprintf(("%s: not properly null terminated? wcslen()->%d", pszPrefix, lstrlenW((LPCWSTR)pv)));
                break;

            default:
                {
                    /* quick and somewhat ugly memory dump routine. */
                    LPBYTE pb;
                    DWORD off;
                    for (off = 0, pb = (LPBYTE)pv; off < cb; off += 16, pb += 16)
                    {
                        char sz[80];
                        char *pszHex = &sz[sprintf(sz, "%08x  ", off)];
                        char *pszBin = pszHex + 16*3 + 2;
                        for (int i = 0; i < 16; i++, pszHex += 3, pszBin++)
                        {
                            if (i + off < cb)
                            {
                                static const char szHexDigits[17] = "0123456789abcdef";
                                pszHex[0] = szHexDigits[pb[i] >> 4];
                                pszHex[1] = szHexDigits[pb[i] & 0xf];
                                pszHex[2] = i != 7 ? ' ' : '-';
                                if (isprint(pb[i]))
                                    *pszBin = pb[i];
                                else
                                    *pszBin = '.';
                            }
                            else
                            {
                                memset(pszHex, ' ', 3);
                                *pszBin = '\0';
                            }
                        }
                        pszHex[0] = ' ';
                        pszHex[1] = ' ';
                        *pszBin = '\0';
                        dprintf(("%s\n", sz));
                    }
                    break;
                }
        }
    }
}
#endif


/**
 * Initalizes the clipboard formats Odin32 uses.
 *
 * These are not the same as the Open32 ones because Odin will put
 * different data out on the clipboard (the CLIPHEADER header).
 *
 * @returns Success indicator.
 */
extern "C"
BOOL WIN32API InitClipboardFormats(void)
{
    dprintf2(("USER32: InitClipboardFormats\n"));
    BOOL    fRc = TRUE;
    HANDLE  hSysAtomTable = OSLibWinQuerySystemAtomTable();
    for (int i = 0; i < sizeof(gaFormats) / sizeof(gaFormats[0]); i++)
    {
        if (gaFormats[i].pszRegString)
        {
            gaFormats[i].ulPM = OSLibWinAddAtom(hSysAtomTable, gaFormats[i].pszRegString);
            DebugAssert(gaFormats[i].ulPM > 20 /* MAX OS/2 predefined format */,
                        ("WinAddAtom failed or returns to low value, %d, for '%s'.\n",
                         gaFormats[i].ulPM, gaFormats[i].pszRegString));
            if (gaFormats[i].ulPM <= 20)
                fRc = FALSE;
        }
        dprintf2(("odin=0x%04x os2=0x%04x %s\n", gaFormats[i].uOdin, gaFormats[i].ulPM, gaFormats[i].pszDescr));
    }
    return fRc;
}



//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//
//
// C L I P B O A R D   V I E W E R S
//
//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//



/**
 * Adds a window to the clipboard viewer list.
 *
 * @returns HWND of the next window.
 * @returns 0 on failure, last error set.
 * @param   hwndNewViewer     Window to add.
 * @status  partially implemented
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  PM manages the list, not the caller, therefor the return is allways NULL.
 */
HWND WIN32API SetClipboardViewer(HWND hwndNewViewer)
{
    /*
     * Validate and translate hwndNewViewer to an OS/2 handle.
     */
    HWND hwndOS2 = OSLIB_HWND_DESKTOP;  /* open32 does this it seems. */
    if (hwndNewViewer)
    {
        Win32BaseWindow * pWnd = Win32BaseWindow::GetWindowFromHandle(hwndNewViewer);
        if (!pWnd)
        {
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            dprintf(("USER32: SetClipboardViewer: returns 0 (invalid window)\n"));
            return NULL;
        }
        hwndOS2 = pWnd->getOS2WindowHandle();
        RELEASE_WNDOBJ(pWnd);
    }

    /*
     * Add the viewer.
     */
    if (!OSLibWin32AddClipbrdViewer(hwndOS2))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY); // just something.
        dprintf(("USER32: Win32AddClipbrdViwer failed. lasterr=%#x\n", OSLibWinGetLastError()));
    }
    dprintf(("USER32: SetClipboardViewer returns 0 (allways)\n"));
    return NULL;
}


/**
 * Gets the current clipboard viewer.
 *
 * @returns Window handle of the clipboard viewer.
 * @returns NULL if no viewer or non odin window.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  Hides OS/2 windows.
 */
HWND WIN32API GetClipboardViewer(void)
{
    /*
     * Query the viewer from PM and translate it to an Odin window handle.
     */
    HWND hwnd = NULL;
    HWND hwndOS2 = OSLibWin32QueryClipbrdViewerChain();
    if (hwndOS2)
    {
        Win32BaseWindow *pWnd = Win32BaseWindow::GetWindowFromOS2Handle(hwndOS2);
        if (pWnd)
        {
            hwnd = pWnd->getWindowHandle();
            RELEASE_WNDOBJ(pWnd);
        }
    }
    dprintf(("USER32: GetClipboardViewer: returns %# (os2=%#x)\n", hwnd, hwndOS2));
    return hwnd;
}


/**
 * Removes a specific window from the chain of clipboard viewers.
 *
 * @returns Success indicator. Last error set on failure.
 * @param   hwndRemove  Handle of viewer window to remove.
 * @param   hwndNext    Handle to the next window in the chain.
 *                      OS/2 doesn't support this, so we ignore it.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL WIN32API ChangeClipboardChain(HWND hwndRemove, HWND hwndNext)
{
    dprintf(("USER32: ChangeClipboardChain: hwndRemove=%#x hwndNext=%#x\n", hwndRemove, hwndNext));

    /*
     * Validate input handles and translate them to OS/2 windows.
     */
    Win32BaseWindow *pWnd = Win32BaseWindow::GetWindowFromHandle(hwndRemove);
    if (!pWnd)
    {
        dprintf(("USER32: ChangeClipboardChain: window %x not found\n", hwndRemove));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    HWND hwndOS2Remove = pWnd->getOS2WindowHandle();
    RELEASE_WNDOBJ(pWnd);

    if (hwndNext)
    {
        /*
         * SetClipboardViewer never returns a hwndNext, so this must
         * be an program trying to alter things in a way other than we like...
         * We might find that we have to support this using the Wn32AddClipbrdViewer
         * or something later, but for now we fail.
         */

        /* validate it, to come up with a more believable story. */
        pWnd = Win32BaseWindow::GetWindowFromHandle(hwndNext);
        if (!pWnd)
        {
            dprintf(("USER32: ChangeClipboardChain: window %x not found\n", hwndNext));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return FALSE;
        }
        HWND hwndOS2Next = pWnd->getOS2WindowHandle();
        RELEASE_WNDOBJ(pWnd);
        DebugAssertFailed(("USER32: ChangeClipboardChain: hwndNext=%#x/%#x is not supported!!!\n", hwndNext, hwndOS2Next));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    /*
     * Change the chain.
     */
    BOOL fRc = OSLibWin32RemoveClipbrdViewer(hwndOS2Remove);
    dprintf(("USER32: ChangeClipboardChain returns %d\n", fRc));
    return fRc;
}




//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//
//
// C L I P B O A R D   N O N - O P E N   O P E R A T I O N S
//
//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

/**
 * Checks if a format is available on the PM clipboard.
 *
 * @returns TRUE if it's there, FALSE if it isn't
 * @param   uFormat     Odin format number.
 */
BOOL     clipboardIsAvailable(UINT uFormat)
{
    ULONG ulPMFormat = clipboardOdinToPMFormat(uFormat);
    if (!ulPMFormat)
        return FALSE;
    ULONG  flInfo;
    return OSLibWinQueryClipbrdFmtInfo(GetThreadHAB(), ulPMFormat, &flInfo);
}


/**
 * Checks if a real representation of a format is available on the PM
 * clipboard. (Real representation means ignoring dummy entries.)
 *
 * @returns TRUE if it's there, FALSE if it isn't
 * @param   uFormat     Odin format number.
 */
BOOL     clipboardIsAvailableReal(UINT uFormat)
{
    ULONG ulPMFormat = clipboardOdinToPMFormat(uFormat);
    if (!ulPMFormat)
        return FALSE;
    ULONG  flInfo = 0;
    HANDLE hab = GetThreadHAB();
    if (!OSLibWinQueryClipbrdFmtInfo(hab, ulPMFormat, &flInfo))
        return FALSE;
    if (!(flInfo & OSLIB_CFI_HANDLE))
        return TRUE;
    return OSLibWinQueryClipbrdData(hab, ulPMFormat) != DUMMY_HANDLE;
}


/**
 * Count the number of available clipboard formats.
 *
 * @returns Number of clipboard forats on the clipboard.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  This call can be done without opening the clipboard.
 */
int WIN32API CountClipboardFormats(void)
{
    int     cRet = 0;
    UINT    uFormat = 0;
    HANDLE  hab = GetThreadHAB();

    while ((uFormat = clipboardEnumClipboardFormats(uFormat)) != 0)
        cRet++;

    dprintf(("USER32: CountClipboardFormats: returns %d\n", cRet));
    return cRet;
}


/**
 * Checks if a given format is available on the clipboard.
 *
 * @returns TRUE if it's availble, else false.
 * @param   uFormat     Format to check for.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL WIN32API IsClipboardFormatAvailable(UINT uFormat)
{
    ULONG   ulPMFormat = clipboardOdinToPMFormat(uFormat);
    if (!ulPMFormat)
    {
        dprintf(("USER32: IsClipboardFormatAvailable: uFormat=%d returns FALSE (unknown format)\n", uFormat));
        return FALSE;
    }

    ULONG   flInfo;
    BOOL    fRc = OSLibWinQueryClipbrdFmtInfo(GetThreadHAB(), ulPMFormat, &flInfo);
    if (!fRc && (!clipboardIsRWOwner() || !gfClipboardChanged))
        fRc = clipboardShouldBeSynthesized(uFormat);
    dprintf(("USER32: IsClipboardFormatAvailable: %x (%s) returning %d\n",
             uFormat, dbgGetFormatName(uFormat), fRc));
    return fRc;
}


/**
 * Get the format with the lowest index in the passed in list.
 *
 * @returns format number with highest priority.
 * @returns 0 if no formats in the list.
 * @returns 0 if no data on the clipboard.
 * @returns -1 if non of the formats was found.
 * @param   paFormatPriorityList
 * @param   cFormats
 * @status  partially implemented
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @todo    We don't check if the thread is owning the clipboard.
 */
int WIN32API GetPriorityClipboardFormat(UINT *paFormatPriorityList, int cFormats)
{
    int     i;
    dprintf(("USER32: GetPriorityClipboardFormat: paFormatPriorityList=%p cFormat=%d\n",
             paFormatPriorityList, cFormats));
    /*
     * Validate input.
     */
    if ((paFormatPriorityList == NULL && cFormats > 0) || cFormats < 0)
    {
        SetLastError(ERROR_NOACCESS);
        dprintf(("USER32: GetPriorityClipboardFormat: returns 0 (bad list pointer)\n"));
        return 0;
    }
#ifdef DEBUG_LOGGING
    for (i = 0; i < cFormats; i++)
        dprintf2(("      %d - %#x (%s)\n", i, paFormatPriorityList[i],
                  dbgGetFormatName(paFormatPriorityList[i])));
#endif

    /*
     * Check if list is empty.
     */
    if (cFormats <= 0)
    {
        dprintf(("USER32: GetPriorityClipboardFormat: return -1 (list is empty)\n"));
        return -1;
    }

    /*
     * Check if clipboard is empty.
     */
    HANDLE hab = GetThreadHAB();
    if (!OSLibWinEnumClipbrdFmts(hab, 0))
    {
        dprintf(("USER32: GetPriorityClipboardFormat: return 0 (clipboard is empty)\n"));
        return 0;
    }

    /*
     * Walk the list.
     */
    for (i = 0; i < cFormats; i++)
    {
        if (IsClipboardFormatAvailable(paFormatPriorityList[i]))
        {
            dprintf(("USER32: GetPriorityClipboardFormat: return %d (%s), index %d\n",
                     paFormatPriorityList[i], dbgGetFormatName(paFormatPriorityList[i]), i));
            return paFormatPriorityList[i];
        }
    }

    dprintf(("USER32: GetPriorityClipboardFormat: return -1 (not found)\n"));
    return -1;
}

/**
 * Gets the format name of a registered clipboard format.
 *
 * @returns number of bytes in lpszFormatName which was used not counting the terminator.
 * @returns 0 on failure, last error set.
 * @param   uFormat         Format number.
 * @param   lpszFormatName  Where to store the name.
 * @param   cchMaxCount     Size of the buffer.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
int WIN32API GetClipboardFormatNameA(UINT uFormat, LPSTR lpszFormatName, int cchMaxCount)
{
    dprintf(("USER32: GetClipboardFormatNameA: uFormat=%d buf=%p cchBuf=%d\n",
             uFormat, lpszFormatName, cchMaxCount));

    /*
     * Validate input.
     */
    if (uFormat < MAXINTATOM)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        dprintf(("USER32: GetClipboardFormatNameA: returns 0 (builtin format)\n"));
        return 0;
    }

    /*
     * Do work.
     */
    int rc = GetAtomNameA((ATOM)uFormat, lpszFormatName, cchMaxCount);
    dprintf(("USER32: GetClipboardFormatNameA: returns %d (%s)\n", rc, rc > 0 ? lpszFormatName : NULL));
    return rc;
}


/**
 * Gets the format name of a registered clipboard format.
 *
 * @returns number of bytes in lpszFormatName which was used not counting the terminator.
 * @returns 0 on failure, last error set.
 * @param   uFormat         Format number.
 * @param   lpszFormatName  Where to store the name.
 * @param   cchMaxCount     Size of the buffer.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
int WIN32API GetClipboardFormatNameW(UINT uFormat, LPWSTR lpszFormatName, int cchMaxCount)
{
    dprintf(("USER32: GetClipboardFormatNameW: uFormat=%d buf=%p cchBuf=%d\n",
             uFormat, lpszFormatName, cchMaxCount));

    /*
     * Validate input.
     */
    if (uFormat < MAXINTATOM)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        dprintf(("USER32: GetClipboardFormatNameW: returns 0 (builtin format)\n"));
        return 0;
    }

    /*
     * Do work.
     */
    int rc = GetAtomNameW((ATOM)uFormat, lpszFormatName, cchMaxCount);
    dprintf(("USER32: GetClipboardFormatNameW: returns %d (%ls)\n", rc, rc > 0 ? lpszFormatName : NULL));
    return rc;
}


/**
 * Register a name application specific clipboard format.
 *
 * @returns The clipboard number for the format.
 * @returns 0 on failure, last error set.
 * @param   pszName     Clipboard format name.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
UINT WIN32API RegisterClipboardFormatA(LPCSTR pszName)
{
    dprintf(("USER32: RegisterClipboardFormatA: pszName=%s\n", pszName));

    /*
     * Custom formats are atoms.
     *  And for some reason which isn't too obvious to me, they are local
     *  atoms, not global atoms like in OS/2. I might be wrong here...
     */
    UINT uFormat = AddAtomA(pszName);
    if (uFormat)
    {
        char szName[256];
        sprintf(szName, "odin32 dyn: %.242s", pszName);
        clipboardAddDynFormat(uFormat, szName);
    }
    dprintf(("USER32: RegisterClipboardFormatA: returns %u\n", uFormat));
    return uFormat;
}


/**
 * Register a name application specific clipboard format.
 *
 * @returns The clipboard number for the format.
 * @returns 0 on failure, last error set.
 * @param   pszName     Clipboard format name.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
UINT WIN32API RegisterClipboardFormatW(LPCWSTR pszName)
{
    dprintf(("USER32: RegisterClipboardFormatW: pszName=%ls\n", pszName));

    /*
     * Custom formats are atoms.
     *  And for some reason which isn't too obvious to me, they are local
     *  atoms, not global atoms like in OS/2. I might be wrong here...
     */
    UINT uFormat = AddAtomW(pszName);
    if (uFormat)
    {
        char szName[256];
        sprintf(szName, "odin32 dyn: %.242ls", pszName); /* bogus to use crt for unicode->ascii conversion? */
        clipboardAddDynFormat(uFormat, szName);
    }
    dprintf(("USER32: RegisterClipboardFormatW: returns %u\n", uFormat));
    return uFormat;
}


/**
 * Gets the window handle of the current clipboard owner.
 *
 * @returns HWND of current owner.
 * @returns NULL if no owner.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  We do not report back any OS/2 windows.
 */
HWND WIN32API GetClipboardOwner(void)
{
    /*
     * Query the owner from PM and translate it to an Odin window handle.
     */
    HWND    hwnd = NULL;
    HWND    hwndOS2 = OSLibWinQueryClipbrdOwner(GetThreadHAB());
    if (hwndOS2)
    {
        Win32BaseWindow *pWnd = Win32BaseWindow::GetWindowFromOS2Handle(hwndOS2);
        if (pWnd)
        {
            hwnd = pWnd->getWindowHandle();
            RELEASE_WNDOBJ(pWnd);
        }
    }

    dprintf(("USER32: GetClipboardOwner: returns %#x (os2=%#x)\n", hwnd, hwndOS2));
    return hwnd;
}


/**
 * Gets the hwnd of the window which currently have the clipboard open.
 *
 * @returns the HWND of the window.
 * @returns NULL if the clipboard is not opened by anyone or the open
 *          didn't supply a window handle (like PM).
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
HWND WIN32API GetOpenClipboardWindow(void)
{
    /*
     * Get the open windown handle and translate it.
     */
    HWND hwnd = NULL;
    HWND hwndOS2 = OSLibWin32QueryOpenClipbrdWindow();
    if (hwndOS2)
    {
        Win32BaseWindow *pWnd = Win32BaseWindow::GetWindowFromOS2Handle(hwndOS2);
        if (pWnd)
        {
            hwnd = pWnd->getWindowHandle();
            RELEASE_WNDOBJ(pWnd);
        }
    }
#if 0
    DebugAssert(ghwndOpenClipboard == hwnd, ("ghwndOpenClipboard=%#x actual=%#x\n", ghwndOpenClipboard, hwnd));
    DebugAssert((!hwnd && !ghwndOS2OpenClipboard) || (hwnd && ghwndOS2OpenClipboard != hwndOS2),
                ("ghwndOS2OpenClipboard=%#x actual=%#x\n", ghwndOS2OpenClipboard, hwndOS2));
#endif
    dprintf(("USER32: GetOpenCliboardWindow: returning %#x (os2=%#x)\n", hwnd, hwndOS2));
    return hwnd;
}






//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//
//
// C L I P B O A R D   O P E N E D   O P E R A T I O N S
//
//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//


/**
 * Checks if the current thread is the clipboard owner with read/write access.
 *
 * @returns TRUE if we're the owner, FALSE if not.
 */
BOOL clipboardIsRWOwner(void)
{
#if 1
    /*
     * The simple way.
     */
    return gtidOpenClipboardThreadId == GetCurrentThreadId();
#else
    /*
     * This is gonna be a bit hacky.
     * We'll try to open the clipboard and then determin whether or not
     * we already owned it by the return code and such.
     */
    BOOL    fRc = FALSE;
    HAB     hab = GetThreadHAB();
    OSLibWinSetErrorInfo(0, 0);
    OSLibWinGetLastError();
    if (!OSLib_OpenClipbrd(hab, (HWND)0xdeadface))
    {
        /* check why we failed. */
        if ((OSLibWinGetLastError() & 0xffff) == OSLIB_PMERR_CALL_FROM_WRONG_THREAD)
            fRc = TRUE;
    }
    else
    {
        /* we might have to close it ... */
        ULONG ulRc = OSLibWinGetLastError();
        if ((ulRc & 0xffff) == OSLIB_PMERR_CALL_FROM_WRONG_THREAD)
        {
            fRc = TRUE;
            DebugAssertFailed(("Open returned TRUE while last error was PMERR_CALL_FROM_WRONG_THREAD. bad heuristics!!!\n"));
        }
        else
        {
            DebugAssert(ulRc == 0, ("Last error is %#x and not 0x0 as we expected!!!\n", ulRc));
            OSLibWinCloseClipbrd(hab);
        }
    }
    return fRc;
#endif
}


/**
 * Adds an item to the PM clipboard.
 *
 * @returns success indicator.
 * @param   uOdinFormat     Odin format number.
 * @param   pv              Pointer to a buffer containing the data.
 *                          If fAllocate is clear this is a buffer allocated
 *                          using clipboardAllocPMBuffer()
 * @param   cb              Size of the data in the buffer.
 */
BOOL     clipboardAddPM(UINT uOdinFormat, void *pv, unsigned cb)
{
    DebugAssert(uOdinFormat == CF_TEXT || uOdinFormat == CF_BITMAP || uOdinFormat == CF_PALETTE,
                ("%d should have a header. fixme!!!\n", uOdinFormat));
    /*
     * Validate and translate the format number.
     */
    ULONG ulPMFormat = clipboardOdinToPMFormat(uOdinFormat);
    DebugAssert(ulPMFormat > 0, ("failed to convert odin format %d to PM!!!\n", uOdinFormat));
    if (ulPMFormat == 0)
        return FALSE;

    /*
     * Allocate PM Clipboard buffer.
     */
    void *pvBuf = (PCLIPHEADER)clipboardAllocPMBuffer(cb);
    if (!pvBuf)
        return FALSE;
    pv = memcpy(pvBuf, pv, cb);

    /*
     * Place it on the clipboard.
     */
    if (OSLibWinSetClipbrdData(GetThreadHAB(),(ULONG)pvBuf, ulPMFormat, OSLIB_CFI_POINTER))
        return TRUE;
    DebugAssertFailed(("WinSetClipbrdData failed!!! lasterr=%#x\n", OSLibWinGetLastError()));
    return FALSE;
}


/**
 * Adds an item to the PM clipboard.
 *
 * @returns success indicator.
 * @param   uOdinFormat     Odin format number.
 * @param   pv              Pointer to a buffer containing the data.
 *                          If fAllocate is clear this is a buffer allocated
 *                          using clipboardAllocPMBuffer()
 * @param   cb              Size of the data in the buffer.
 */
BOOL     clipboardAddPMHeader(UINT uOdinFormat, void *pv, unsigned cb)
{
    DebugAssert(uOdinFormat != CF_TEXT && uOdinFormat != CF_BITMAP && uOdinFormat != CF_PALETTE,
                ("%d should not have a header. fixme!!!\n", uOdinFormat));
    /*
     * Validate and translate the format number.
     */
    ULONG ulPMFormat = clipboardOdinToPMFormat(uOdinFormat);
    DebugAssert(ulPMFormat > 0, ("failed to convert odin format %d to PM!!!\n", uOdinFormat));
    if (ulPMFormat == 0)
        return FALSE;

    /*
     * Allocate PM Clipboard buffer.
     */
    PCLIPHEADER pClip = (PCLIPHEADER)clipboardAllocPMBuffer(cb + sizeof(CLIPHEADER));
    if (!pClip)
        return FALSE;
    memcpy(pClip->achMagic, CLIPHEADER_MAGIC, sizeof(CLIPHEADER_MAGIC));
    pClip->cbData  = cb;
    pClip->uFormat = uOdinFormat;
    pv = memcpy(pClip + 1, pv, cb);

    /*
     * Place it on the clipboard.
     */
    if (OSLibWinSetClipbrdData(GetThreadHAB(),(ULONG)pClip, ulPMFormat, OSLIB_CFI_POINTER))
        return TRUE;
    DebugAssertFailed(("WinSetClipbrdData failed!!! lasterr=%#x\n", OSLibWinGetLastError()));
    return FALSE;
}


/**
 * Adds an item to the PM clipboard.
 *
 * @returns success indicator.
 * @param   uOdinFormat     Odin format number.
 * @param   h               Handle.
 */
BOOL     clipboardAddPMHandle(UINT uOdinFormat, HANDLE h)
{
    DebugAssert(uOdinFormat == CF_TEXT || uOdinFormat == CF_BITMAP || uOdinFormat == CF_PALETTE,
                ("%d should have a header. fixme!!!\n", uOdinFormat));
    /*
     * Validate and translate the format number.
     */
    ULONG ulPMFormat = clipboardOdinToPMFormat(uOdinFormat);
    DebugAssert(ulPMFormat > 0, ("failed to convert odin format %d to PM!!!\n", uOdinFormat));
    if (ulPMFormat == 0)
        return FALSE;

    /*
     * Place it on the clipboard.
     */
    if (OSLibWinSetClipbrdData(GetThreadHAB(),(ULONG)h, ulPMFormat, OSLIB_CFI_HANDLE))
        return TRUE;
    DebugAssertFailed(("WinSetClipbrdData failed!!! lasterr=%#x\n", OSLibWinGetLastError()));
    return FALSE;
}


/**
 * Allocates a buffer for use with the PM clipboard.
 *
 * @returns Pointer to buffer.
 * @returns NULL on failure.
 * @param   cb  Size of buffer.
 */
void *   clipboardAllocPMBuffer(unsigned cb)
{
    /*
     * Dos alloc a shared buffer.
     */
    PVOID   pv = NULL;
    ULONG   rc = OSLibDosAllocSharedMem(&pv, NULL, (cb + 0xfff) & ~0xfff, OSLIB_PAG_READ | OSLIB_PAG_WRITE | OSLIB_PAG_COMMIT | OSLIB_OBJ_GIVEABLE);
    if (rc)
        return NULL;
    return pv;
}



/**
 * Inserts a dummy placeholder in the clipboard.
 *
 * @returns Success indicator.
 * @param   uOdinFormat     Odin clipboard format number.
 * @remark  The few other applications using the Open32 defined
 *          format names may not like this...
 */
BOOL     clipboardAddPMDummy(UINT uOdinFormat)
{
    /*
     * Validate and translate the format number.
     */
    ULONG ulPMFormat = clipboardOdinToPMFormat(uOdinFormat);
    DebugAssert(ulPMFormat > 0, ("failed to convert odin format %d to PM!!!\n", uOdinFormat));
    if (ulPMFormat == 0)
        return FALSE;

    /*
     * Insert a dummy handle.
     */
    BOOL fRc = OSLibWinSetClipbrdData(GetThreadHAB(), DUMMY_HANDLE, ulPMFormat, OSLIB_CFI_HANDLE);
    DebugAssert(fRc, ("WinSetClipbrdData failed. lasterr=%#x\n", OSLibWinGetLastError()));
    return fRc;
}


/**
 * Tries to figure out the size of the data retrieved from the clipboard.
 *
 * @returns 0 on failure.
 * @returns size in bytes of the data.
 * @param   pvData  Data retrieved from the clipboard.
 */
ULONG clipboardGetPMDataSize(void *pvData)
{
    ULONG   fl = ~0;
    ULONG   cb = ~0;
    ULONG ulRc = OSLibDosQueryMem(pvData, &cb, &fl);
    if (ulRc)
    {   /* retry */
        fl = ~0;
        cb = 512*1024*1024;
        ulRc = OSLibDosQueryMem(pvData, &cb, &fl);
    }
    if (ulRc)
    {
        DebugAssertFailed(("DosQueryMem(%#x,,) failed with rc=%d\n", pvData, ulRc));
        return 0;
    }

    dprintf2(("data: %#x size: %d flags: %#x\n", pvData, cb, fl));
    return cb;
}


/**
 * Opens the clipboard.
 *
 * @returns Success indicator.
 * @param   hwnd    Handle to window which is to become owner
 *                  when EmptyClipboard is called.
 *                  If NULL the clipboard is to be associated with
 *                  the current thread.
 * @status  completely implemented
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  This should not block, although there is a possiblity that a competing
 *          thread may cause us to block. This is PM's fault.
 */
BOOL WIN32API OpenClipboard(HWND hwnd)
{
    HWND    hwndOS2 = NULL;

    /*
     * Validate input and translate window handle.
     */
    if (hwnd)
    {
        Win32BaseWindow    *pWnd = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if (!pWnd)
        {
            dprintf(("USER32: OpenClipboard: window %x not found", hwnd));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return 0;
        }
        hwndOS2 = pWnd->getOS2WindowHandle();
        RELEASE_WNDOBJ(pWnd);
    }

    /*
     * Open the clipboard.
     */
    dprintf(("USER32: OpenClipboard(%#x/%#x)\n", hwnd, hwndOS2));
    BOOL fRc = OSLib_OpenClipbrd(GetThreadHAB(), hwndOS2);
    if (fRc)
    {
        ghwndOpenClipboard    = hwnd;
        ghwndOS2OpenClipboard = hwndOS2;
        gtidOpenClipboardThreadId  = GetCurrentThreadId();
        gfClipboardChanged    = FALSE;
        gfClipboardChangedText= FALSE;
    }
    dprintf(("USER32: OpenClipboard: returning %d\n", fRc));
    return fRc;
}


/**
 * Empty the clipboard.
 *
 * This will empty the clipboard freeing all global resources.
 * It will further free all local resources associated with the
 * clipboard.
 *
 * The window specified in the OpenClipboard() call will become
 * the owner of the clipboard upon successful return from this
 * call.
 *
 * @returns Success indicator.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL WIN32API EmptyClipboard(void)
{
    dprintf(("USER32: EmptyClipboard. ghwndOpenClipboard=%#x/%#x\n", ghwndOpenClipboard, ghwndOS2OpenClipboard));
    BOOL    fRc = OSLibWinEmptyClipbrd(GetThreadHAB());
    if (!fRc)
    {
        ULONG ulRc = OSLibWinGetLastError();
        if ((ulRc & 0xffff) == OSLIB_PMERR_CALL_FROM_WRONG_THREAD)
            SetLastError(ERROR_CLIPBOARD_NOT_OPEN);
        else
            SetLastError(ERROR_GEN_FAILURE);
        dprintf(("USER32: EmptyClipboard: WinEmptyClipbrd failed. lasterr=%#x\n", ulRc));
        return FALSE;
    }

    /*
     * Take clipboard ownership.
     */
    if (!OSLibWinSetClipbrdOwner(GetThreadHAB(), ghwndOS2OpenClipboard))
    {
        DebugAssertFailed(("WinSetClipbrdOwner failed!! lasterr=%#x\n", OSLibWinGetLastError()));
        #if 0 /* Not sure what kind of action which makes most sense. */
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
        #else
        OSLibWinSetClipbrdOwner(GetThreadHAB(), NULL);
        #endif
    }

    /*
     * Free per process resources.
     */
    for (PCLIPLOCALDATA p = gpLocalData, pNext = NULL; p; p = pNext)
    {
        pNext = p->pNext;
        clipboardCacheDeleteNode(p);
    }
    gpLocalData = NULL;
    gfClipboardChanged = TRUE;
    gfClipboardChangedText = FALSE;

    dprintf(("USER32: EmptyClipboard: returns successfully\n"));
    return TRUE;
}


/**
 * Puts data onto the clipboard.
 *
 * @returns hCliObj on success.
 * @returns NULL on failure, last error may be set.
 * @param   uFormat     The format of hClipObj.
 * @param   hClipObj    Handle to data to put on the clipboard.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @todo    Checkup last errors!
 */
HANDLE WIN32API SetClipboardData(UINT uFormat, HANDLE hClipObj)
{
    dprintf(("SetClipboardData uFormat=%#x (%s) hClipObj=%#x", uFormat, dbgGetFormatName(uFormat), hClipObj));
    dbgDumpClipboardData("SetClipboardData", uFormat, hClipObj);

    /*
     * Validate that the thread have opened the clipboard.
     */
    if (/* !clipboardIsRWOwner() - must allow rendering. PM catches it. ||*/ !uFormat)
    {
        SetLastError(ERROR_CLIPBOARD_NOT_OPEN);
        dprintf(("USER32: SetClipboardData: returns NULL (no openclipboard)\n"));
        return NULL;
    }

    /*
     * Validate format.
     */
    ULONG ulPMFormat = clipboardOdinToPMFormat(uFormat);
    DebugAssert(!uFormat || ulPMFormat > 0, ("failed to convert odin format %d to PM!!!\n", uFormat));
    if (!ulPMFormat)
    {
        //last error?
        dprintf(("USER32: SetClipboardData: return NULL (invalid format %d)\n", uFormat));
        return NULL;
    }

    /*
     * For some formats we just put out OS/2 handles, for the
     * others we duplicate the memory buffer passed in to us.
     */
    PCLIPLOCALDATA  pLocalClip = NULL;  /* Cache node for per process data. */
    ULONG           flPMData = 0;
    ULONG           ulPMData = 0;
    BOOL            fHeader = TRUE;     /* Whether or not to prepend an odin clip data header. */
    switch (uFormat)
    {
        /*
         * Make a duplication of the PM handle for the PM clipboard and
         * put the Odin32 handle in the cache.
         */
        case CF_BITMAP:
        case CF_DSPBITMAP:
        case CF_PALETTE:
            flPMData = OSLIB_CFI_HANDLE;
            if (hClipObj)
            {
                ulPMData = O32_GetPMHandleFromGDIHandle(hClipObj);
                if (!ulPMData)
                {
                    dprintf(("USER32: SetClipboardData: return NULL. Failed to get PM handle from Odin32 %s handle %#x\n",
                             dbgGetFormatName(uFormat), hClipObj));
                    return NULL;
                }

                /*
                 * Duplicate the PM handle.
                 */
                switch (uFormat)
                {
                    case CF_BITMAP:
                    case CF_DSPBITMAP:
                        ulPMData = (ULONG)OSLibClipboardPMBitmapDuplicate((HBITMAP)ulPMData);
                        break;
                    case CF_PALETTE:
                        ulPMData = (ULONG)OSLibClipboardPMPaletteDuplicate((HPALETTE)ulPMData);
                        break;
                }
                if (!ulPMData)
                {
                    dprintf(("USER32: SetClipboardData: return NULL. Failed to duplicate PM handle!\n"));
                    return NULL;
                }

                /*
                 * Allocate cache node.
                 */
                if (!clipboardCacheAllocGDI(uFormat, hClipObj, &pLocalClip))
                    DebugAssertFailed(("out of memory"));
            }
            break;

        /*
         * Put the metafile bits onto the clipboard.
         * Put hClipObj in cache for delayed freeing.
         */
        case CF_METAFILEPICT:
        case CF_DSPMETAFILEPICT:
        {
#if 0 //todo implement get code.
            flPMData = OSLIB_CFI_POINTER;
            if (hClipObj)
            {
                /* get the data */
                LPMETAFILEPICT pMeta = (LPMETAFILEPICT)GlobalLock(hClipObj);
                if (!pMeta)
                {
                    SetLastError(ERROR_INVALID_PARAMETER);
                    dprintf(("USER32: SetClipboardData: returns NULL (metafilepict lock)\n"));
                    return NULL;
                }
                /* get size */
                DWORD cb = GetMetaFileBitsEx(pMeta->hMF, 0, NULL);
                dprintf2(("GetMetaFileBitsEx -> %d\n", cb));
                if (!cb)
                {   /* ?? */
                    SetLastError(ERROR_INVALID_PARAMETER);
                    dprintf(("USER32: SetClipboardData: returns NULL (metafilepict handle)\n"));
                    return NULL;
                }
                /* allocate shared */
                DWORD cbTotal = sizeof(CLIPHEADER) + sizeof(METAFILEPICT) + cb;
                PCLIPHEADER pClip = (PCLIPHEADER)clipboardAllocPMBuffer(cbTotal);
                if (!pClip)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    dprintf(("USER32: SetClipboardData: returns NULL (metafilepict mem)\n"));
                    return NULL;
                }
                /* make block */
                memcpy(pClip->achMagic, CLIPHEADER_MAGIC, sizeof(CLIPHEADER_MAGIC));
                pClip->cbData  = cbTotal; /* bits + metafilepict */
                pClip->uFormat = uFormat;
                *(LPMETAFILEPICT)(pClip + 1) = *pMeta;
                /* (ASSUMES it's not be touched since last call to this api.) */
                GetMetaFileBitsEx(pMeta->hMF, cb, (char*)(pClip + 1) + sizeof(*pMeta));
            }
            break;
#else
            DebugAssertFailed(("metafile support not enabled"));
            return NULL;
#endif
        }

        /*
         * Put the enh. metafile bits onto the clipboard.
         * Put hClipObj in cache for delayed freeing.
         */
        case CF_ENHMETAFILE:
        case CF_DSPENHMETAFILE:
        {
#if 0 //todo implement get code.
            flPMData = OSLIB_CFI_POINTER;
            if (hClipObj)
            {
                /* get size */
                DWORD cb = GetEnhMetaFileBits(hClipObj, 0, NULL);
                dprintf2(("GetEnhMetaFileBits -> %d\n", cb));
                if (!cb)
                {   /* ?? */
                    SetLastError(ERROR_INVALID_PARAMETER);
                    dprintf(("USER32: SetClipboardData: returns NULL (enhmetafile handle)\n"));
                    return NULL;
                }
                /* allocate shared */
                DWORD cbTotal = sizeof(CLIPHEADER) + cb;
                PCLIPHEADER pClip = (PCLIPHEADER)clipboardAllocPMBuffer(cbTotal);
                if (!pClip)
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    dprintf(("USER32: SetClipboardData: returns NULL (enhmetafile mem)\n"));
                    return NULL;
                }
                /* make block */
                pClip->cbData  = cbTotal; /* bits */
                pClip->uFormat = uFormat;
                /* (ASSUMES it's not be touched since last call to this api.) */
                GetEnhMetaFileBits(hClipObj, cb, (LPBYTE)(pClip + 1));
            }
            break;
#else
            DebugAssertFailed(("metafile support not enabled"));
            return NULL;
#endif
        }

        /*
         * The Owner display, the data must be NULL.
         */
        case CF_OWNERDISPLAY:
        {
            flPMData = OSLIB_CFI_OWNERDISPLAY | OSLIB_CFI_HANDLE;
            if (hClipObj)
            {
                SetLastError(ERROR_INVALID_PARAMETER); /** @todo check this last error */
                dprintf(("USER32: SetClipboardData: returns NULL (owner display data)\n"));
                return NULL;
            }
            break;
        }

        /*
         * Allocate shared data.
         */
        case CF_TEXT:
        case CF_DSPTEXT:
             /* these are shared with OS/2 and shall have no header */
             fHeader = FALSE;
             /* fallthru */
        case CF_UNICODETEXT:
        case CF_OEMTEXT:
        case CF_LOCALE:
        case CF_DIB:
        case CF_DIBV5:
        case CF_HDROP:
        case CF_SYLK:
        case CF_DIF:
        case CF_TIFF:
        case CF_PENDATA:
        case CF_RIFF:
        case CF_WAVE:
            flPMData = OSLIB_CFI_POINTER;
            /* fall thru */
        default:
            if (uFormat >= CF_GDIOBJFIRST && uFormat <= CF_GDIOBJFIRST)
            {
                DebugAssertFailed(("CF_GDIOBJ* is not implemented!!!\n"));
                SetLastError(ERROR_GEN_FAILURE);
                dprintf(("USER32: SetClipboardData: returns NULL (gdiobj)\n"));
                return NULL;
            }

            /*
             * We assume the rest is global handles, if not we'll check treat
             * them as handles we don't care to much about.
             */
            if (hClipObj)
            {
                PVOID pv = GlobalLock(hClipObj);
                if (pv)
                {
                    /*
                     * Global memory object.
                     * Duplicate it and put hClipObj in the cache for delayed deletion.
                     */
                    DWORD   cb = GlobalSize(hClipObj);
                    ulPMData = (ULONG)clipboardAllocPMBuffer(cb + (fHeader ? sizeof(CLIPHEADER) : 0));
                    if (!ulPMData)
                    {
                        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                        GlobalUnlock(hClipObj);
                        dprintf(("USER32: SetClipboardData: returns NULL (shared alloc failed)\n"));
                        return NULL;
                    }
                    if (fHeader)
                    {
                        PCLIPHEADER pClip = (PCLIPHEADER)(void*)ulPMData;
                        memcpy(pClip->achMagic, CLIPHEADER_MAGIC, sizeof(CLIPHEADER_MAGIC));
                        pClip->cbData  = cb;
                        pClip->uFormat = uFormat;
                        memcpy((void*)(ulPMData + sizeof(CLIPHEADER)), pv, cb);
                    }
                    else
                        memcpy((void*)ulPMData, pv, cb);
                    GlobalUnlock(hClipObj);
                    flPMData = OSLIB_CFI_POINTER;

                    /* Allocate a cache node. */
                    if (!clipboardCacheAllocGlobalMem(uFormat, hClipObj, &pLocalClip))
                        DebugAssertFailed(("out of memory"));
                }
                else
                {   /*
                     * Handle of some other kind?
                     *      Check if it's supposed to be a pointer.
                     */
                    if (flPMData)
                    {   /* it's a predefined format which is supposed to be a global memory handle. */
                        SetLastError(ERROR_INVALID_PARAMETER); /** @todo check this last error */
                        DebugAssertFailed(("expected GlobalAlloc handle, got %#x!! format=%d (%s)\n",
                                           hClipObj, uFormat, dbgGetFormatName(uFormat))); /* (for application debugging.) */
                        dprintf(("USER32: SetClipboardData: returns NULL (bad global handle)\n"));
                        return NULL;
                    }

                    /* some kind of handle, this might not work to well... */
                    flPMData = OSLIB_CFI_HANDLE;
                    dprintf(("USER32: SetClipboardData: Warning: format %d data %#x is not a Global memory handle!!!\n",
                             uFormat, hClipObj));
                }
            }
            else
            {
                /*
                 * To be rendered.
                 */
                if (!flPMData)
                    flPMData = OSLIB_CFI_POINTER; /* must be set to something, and pointer is a safe bet. */
            }
            break;
    }

    /*
     * Put it onto the PM clipboard.
     */
    dprintf2(("calling OSLibWinSetClipbrdData(%#x, %#x, %d, %#x)\n", GetThreadHAB(), ulPMData, ulPMFormat, flPMData));
    if (!OSLibWinSetClipbrdData(GetThreadHAB(), ulPMData, ulPMFormat, flPMData))
    {
        /*
         * Failed!
         */
        if ((OSLibWinGetLastError() & 0xffff) == OSLIB_PMERR_CALL_FROM_WRONG_THREAD)
            SetLastError(ERROR_CLIPBOARD_NOT_OPEN);
        else
            DebugAssertFailed(("OSLibWinSetClipbrdData(%#x, %#x, %d, %#x) failed last error=%d\n",
                               GetThreadHAB(), ulPMData, ulPMFormat, flPMData, OSLibWinGetLastError()));

        /* cleanup */
        if ((flPMData & OSLIB_CFI_POINTER) && ulPMData)
            OSLibDosFreeMem((void*)ulPMData);
        else
            switch (uFormat)
            {
                case CF_BITMAP:
                case CF_DSPBITMAP:
                    OSLibClipboardPMBitmapDelete(ulPMData);
                    break;
                case CF_PALETTE:
                    OSLibClipboardPMPaletteDelete(ulPMData);
                    break;
            }

        if (pLocalClip)
            free(pLocalClip);           /* Do not free the data suplied! */

        dprintf(("USER32: SetClipboardData: returns NULL (set clipboard failed)\n"));
        return NULL;
    }

    /*
     * Success!
     */
    /* update globals */
    gfClipboardChanged = TRUE;
    switch (uFormat)
    {
        case CF_TEXT:
        case CF_OEMTEXT:
        case CF_UNICODETEXT:
            gfClipboardChangedText = TRUE;
            break;
    }

    /* insert node into cache if required. */
    if (pLocalClip)
        hClipObj = clipboardCacheInsertNode(pLocalClip);

    dprintf(("USER32: SetClipboardData: returns %#x\n", hClipObj));
    return hClipObj;
}


/**
 * Gets a specific form of the clipboard if it's there.
 *
 * @returns handle to clipboard data on success.
 * @returns NULL on failure. Last error may be set, depends on the failure.
 * @param   uFormat     The clipboard format to get off the clipboard.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  we don't like metafiles at the present.
 * @remark  we never return the same hMem for the same data, this is mentioned as a @todo in the code.
 */
HANDLE WIN32API GetClipboardData(UINT uFormat)
{
    dprintf(("USER32: GetClipboardData: uFormat=%u (%s)\n", uFormat, dbgGetFormatName(uFormat)));

    /*
     * Validate that the thread have opened the clipboard.
     */
    if (!clipboardIsRWOwner())
    {
        SetLastError(ERROR_CLIPBOARD_NOT_OPEN);
        dprintf(("USER32: GetClipboardData: returns NULL (no openclipboard)\n"));
        return NULL;
    }

    /*
     * Validate and translate the format number.
     */
    ULONG ulPMFormat = clipboardOdinToPMFormat(uFormat);
    DebugAssert(!uFormat || ulPMFormat > 0, ("failed to convert odin format %d to PM!!!\n", uFormat));
    if (ulPMFormat == 0)
    {
        //no last error?
        dprintf(("USER32: GetClipboardData: returns NULL (invalid format!!!)\n"));
        return NULL;
    }

    /*
     * We need to synthesize formats in case OS/2 were the one
     * which put stuff on the clipboard.
     */
    if (!gfClipboardChanged)
        clipboardSynthesize(gfClipboardChangedText);

    /*
     * Get the data.
     */
    HANDLE  hab      = GetThreadHAB();
    ULONG   ulPMInfo;
    if (!OSLibWinQueryClipbrdFmtInfo(hab, ulPMFormat, &ulPMInfo))
    {
        //no last error?
        dprintf(("USER32: GetClipboardData: returns NULL (format not present)\n"));
        return NULL;
    }
    ULONG   ulPMData = OSLibWinQueryClipbrdData(hab, ulPMFormat);
    if (!ulPMData)
    {
        //no last error?
        dprintf(("USER32: GetClipboardData: returns NULL (format not present)\n"));
        return NULL;
    }

    /*
     * Allocate local copies of the data like Odin expects.
     *
     * In this process we'll synthesize formats and convert
     * from OS/2 PM formats to Odin formats.
     */
    /* dummy handle? */
    if (       (ulPMInfo & (OSLIB_CFI_HANDLE | OSLIB_CFI_POINTER | OSLIB_CFI_OWNERDISPLAY | OSLIB_CFI_OWNERFREE))
            == OSLIB_CFI_HANDLE
        &&  ulPMData == DUMMY_HANDLE)
    {   /* synthesized */
        /** @todo check if we've done this operation on this data before. */
        HANDLE hRet;
        switch (uFormat)
        {
            case CF_TEXT:
            case CF_UNICODETEXT:
            case CF_OEMTEXT:
                hRet = clipboardSynthesizeText(uFormat);
                break;

            case CF_LOCALE:
                hRet = clipboardSynthesizeLocale(uFormat);
                break;

            case CF_BITMAP:
                hRet = clipboardSynthesizeBitmap(uFormat);
                break;

            case CF_DIB:
            case CF_DIBV5:
                hRet = clipboardSynthesizeDIB(uFormat);
                break;

            #if 0 // not implemented yet.
            case CF_PALETTE:
                hRet = clipboardSynthesizePalette(uFormat);
                break;

            case CF_METAFILEPICT:
                hRet = clipboardSynthesizeMetaFile(uFormat);
                break;
            case CF_ENHMETAFILE:
                hRet = clipboardSynthesizeEnhMetaFile(uFormat);
                break;
            #endif

            default:
                DebugAssertFailed(("Format %d doesn't do dummy handles!\n", uFormat));
                dprintf(("USER32: GetClipboardData: returns NULL (internal error!!!)\n"));
                return NULL;
        }
        dprintf(("USER32: GetClipboardData: returns %#x (synthesized)\n", hRet));
        return hRet;
    }
    else if (ulPMInfo & OSLIB_CFI_POINTER)
    {   /* pointers: text */
        /* Sanity check. */
        switch (uFormat)
        {
            case CF_BITMAP:
            case CF_DSPBITMAP:
            case CF_PALETTE:
                dprintf(("USER32: GetClipboardData: return NULL (format/data mismatch)\n"));
                return NULL;

            case CF_ENHMETAFILE:
            case CF_DSPENHMETAFILE:
            case CF_METAFILEPICT:
            case CF_DSPMETAFILEPICT:
                DebugAssertFailed(("Metafile support isn't enabled\n"));
                return NULL;
        }

        /** @todo check if we've done this operation on this data before. */

        /*
         * Duplicate the data raw in a GlobalAlloc'ed block local to the calling process.
         */
        /* Figure out the size of the data */
        ULONG   cbOdin;
        void   *pvOdin;
        if (uFormat != CF_TEXT && uFormat != CF_BITMAP && uFormat != CF_PALETTE
            && !memcmp(((PCLIPHEADER)ulPMData)->achMagic, CLIPHEADER_MAGIC, sizeof(CLIPHEADER_MAGIC)))
        {   /* Have Odin header */
            PCLIPHEADER pClip = (PCLIPHEADER)ulPMData;
            DebugAssert(pClip->uFormat == uFormat, ("Format mismatch %d != %d\n", pClip->uFormat, uFormat));
            cbOdin = pClip->cbData;
            pvOdin = (void*)(pClip + 1);
        }
        else
        {
            DebugAssert(uFormat == CF_TEXT || uFormat == CF_BITMAP || uFormat == CF_PALETTE,
                        ("%d should have a header!!!\n", uFormat));
            pvOdin = (void*)ulPMData;
            ULONG cb = clipboardGetPMDataSize(pvOdin);
            if (!cb)
            {
                dprintf(("USER32: GetClipboardData: return NULL (cannot determin size of data)\n"));
                return NULL;
            }

            /* find the size of the odin data. */
            cbOdin = cb;
            switch (uFormat)
            {
                case CF_TEXT:
                    cbOdin = clipboardTextCalcSize(pvOdin, cb);
                    break;
            }
        }

        /* allocate object. */
        PCLIPLOCALDATA pLocalClip;
        HANDLE hMem = clipboardCacheAllocGlobalDup(uFormat, pvOdin, cbOdin, &pLocalClip);
        if (!hMem)
        {
            dprintf(("USER32: GetClipboardData: returns NULL (GlobalAlloc(,%d) failed)\n", cbOdin));
            return NULL;
        }

        /* insert cache object. */
        hMem = clipboardCacheInsertNode(pLocalClip);

        dprintf(("USER32: GetClipboardData: return %#x (%d bytes)\n", hMem, cbOdin));
        return hMem;
    }
    else if (ulPMInfo & OSLIB_CFI_HANDLE)
    {   /* handles: palette or bitmap */
        HANDLE hGDI;
        switch (uFormat)
        {
            case CF_BITMAP:
            case CF_DSPBITMAP:
                hGDI = O32_CreateBitmapFromPMHandle((HBITMAP)ulPMData);
                break;

            case CF_PALETTE:
                hGDI = O32_CreateBitmapFromPMHandle((HPALETTE)ulPMData);
                break;

            /** @todo make cases for the other which are defined as handles. */
            /* (for now we assume that everything else is Odin handles.) */
            default:
                dprintf(("USER32: GetClipboardData: returns NULL (funny stuff on clipboard)\n"));
                return (HANDLE)ulPMData;
        }

        /* allocate and insert cache node. */
        PCLIPLOCALDATA pLocalClip;
        if (clipboardCacheAllocGDI(uFormat, hGDI, &pLocalClip))
            hGDI = clipboardCacheInsertNode(pLocalClip);

        dprintf(("USER32: GetClipboardData: returns %#x (translated OS/2 handle)\n", hGDI));
        return hGDI;
    }
    else
    {
        DebugAssertFailed(("Something is on the clipboard without any understandable attributes!! ulInfo=%#x ulPMData=%#x\n",
                            ulPMInfo, ulPMData));
        dprintf(("USER32: GetClipboardData: returns NULL (funny stuff on clipboard)\n"));
        return NULL;
    }
}


/**
 * Enumerate clipboard formats.
 *
 * Must be RW owner of clipboard (i.e. OpenClipboard()).
 *
 * @returns next clipboard format.
 * @returns 0 if no more formats in the clipboard.
 * @param   uFormat     Current format. Use 0 for the starting
 *                      the enumeration.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
UINT WIN32API EnumClipboardFormats(UINT uFormat)
{
    dprintf(("USER32: EnumClipboardFormats: uFormat=%d (%s)\n", uFormat,
             uFormat ? dbgGetFormatName(uFormat) : "start enum"));

    /*
     * Validate that the thread have opened the clipboard.
     */
    if (!clipboardIsRWOwner())
    {
        SetLastError(ERROR_CLIPBOARD_NOT_OPEN);
        dprintf(("USER32: EnumClipboardFormats: returns 0 (no openclipboard)\n"));
        return 0;
    }

    UINT rc = clipboardEnumClipboardFormats(uFormat);
    dprintf(("USER32: EnumClipboardFormats: returns %d\n"));
    return rc;
}


/**
 * Internal worker for EnumClipboardFormats.
 * Also used by CountClipboardFormats();
 *
 * @returns next clipboard format.
 * @returns 0 if no more formats in the clipboard.
 * @param   uFormat     Current format. Use 0 for the starting
 *                      the enumeration.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
UINT clipboardEnumClipboardFormats(UINT uFormat)
{
    dprintf(("USER32: clipboardEnumClipboardFormats: uFormat=%d (%s)\n", uFormat,
             uFormat ? dbgGetFormatName(uFormat) : "start enum"));

    /*
     * Validate/translate input.
     */
    ULONG   ulPMFormat = 0;
    if (uFormat)
    {
        ulPMFormat = clipboardOdinToPMFormat(uFormat);
        if (!ulPMFormat)
        {
            dprintf(("USER32: clipboardEnumClipboardFormats: invalid uFormat(=%u)!!\n", uFormat));
            return 0;
        }
    }

    /*
     * Enumerate next OS/2 format.
     */
    HANDLE  hab = GetThreadHAB();
    while ((ulPMFormat = OSLibWinEnumClipbrdFmts(GetThreadHAB(), ulPMFormat)) != 0)
    {
        UINT uNextFormat = clipboardPMToOdinFormat(ulPMFormat);
        if (uNextFormat)
        {
            dprintf(("USER32: clipboardEnumClipboardFormats: returning %u (%s)\n", uNextFormat, dbgGetFormatName(uNextFormat)));
            return uNextFormat;
        }
        dprintf2(("USER32: clipboardEnumClipboardFormats: skipping OS/2 only format %lu\n", ulPMFormat));
    }

    /*
     * Since we cannot know what non Odin applications are doing,
     * we will have to check for synthesized formats here.
     *
     * We're not doing this if we're the clipboard owner and have changed
     * the clipboard in any way. This is to not synthesize stuff to early
     * compared to W2K. See GetClipboardFormat.
     */
    if (uFormat && (!clipboardIsRWOwner() || !gfClipboardChanged))
    {
        static UINT auSeq[] = { CF_TEXT, CF_UNICODETEXT, CF_OEMTEXT, CF_DIB, CF_DIBV5, CF_BITMAP, 0 }; /** @todo complete and check this. */
        ULONG       flInfo;

        /* find starting index */
        int         i = 0;
        if (!clipboardIsAvailable(uFormat))
        {
            for (i = 0; auSeq[i]; i++)
                if (auSeq[i] == uFormat)
                {
                    i++;
                    break;
                }
        }

        /*
         * Find the next synthesized format.
         */
        while ((uFormat = auSeq[i]) != 0)
        {
            if (clipboardShouldBeSynthesized(uFormat))
            {   /* not there, potentially synthesized format. */
                dprintf(("USER32: clipboardEnumClipboardFormats: returning %u (%s)\n", uFormat, dbgGetFormatName(uFormat)));
                return uFormat;
            }
            /* next */
            i++;
        }

    }

    dprintf(("USER32: clipboardEnumClipboardFormats: returning 0\n"));
    return 0;
}



/**
 * Closes the clipboard.
 *
 * @returns Success indicator.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL WIN32API CloseClipboard(void)
{
    dprintf(("USER32: CloseClipboard"));

    /*
     * Check that we're owning the clipboard.
     */
    if (!clipboardIsRWOwner())
    {
        SetLastError(ERROR_CLIPBOARD_NOT_OPEN);
        dprintf(("USER32: CloseClipboard returns FALSE (clipboard not open!!)\n"));
        return FALSE;
    }

    HAB hab = GetThreadHAB();

    /*
     * Synthesize formats.
     */
    if (gfClipboardChanged)
        clipboardSynthesize(gfClipboardChangedText);

    /*
     * Actually close it.
     */
    if (gtidOpenClipboardThreadId == GetCurrentThreadId()) /* paranoia */
        gtidOpenClipboardThreadId = 0;
    BOOL fRc = OSLibWinCloseClipbrd(hab);
    DebugAssert(fRc, ("WinCloseClipbrd failed!!! lasterr=%#x\n", OSLibWinGetLastError()));
    dprintf(("USER32: CloseClipboard: returning %s\n", fRc ? "TRUE" : "FALSE"));
    return fRc;
}


/**
 * Makes dummies for synthesized formats.
 *
 * This is called by CloseClipboard() and GetClipboardData(). The latter
 * because Odin32 might not have been the one to put data on the clipboard.
 */
void clipboardSynthesize(BOOL fHaveAddedText)
{
    dprintf2(("USER32: clipboardSynthesize: fHaveAddedText=%d\n", fHaveAddedText));
    /* text */
    BOOL    fCF_OEMTEXT     = clipboardIsAvailable(CF_OEMTEXT);
    BOOL    fCF_UNICODETEXT = clipboardIsAvailable(CF_UNICODETEXT);
    BOOL    fCF_TEXT        = clipboardIsAvailable(CF_TEXT);
    if (fCF_TEXT || fCF_UNICODETEXT || fCF_OEMTEXT)
    {
        /*
         * Add the CF_LOCALE if we have modified the clipboard.
         */
        if (!clipboardIsAvailable(CF_LOCALE))
        {
            if (!fHaveAddedText)
                clipboardAddPMDummy(CF_LOCALE);
            else
            {
                LCID    lcid = GetThreadLocale();
                clipboardAddPMHeader(CF_LOCALE, &lcid, sizeof(lcid));
            }
        }

        /*
         * Add dummies.
         */
        if (!fCF_TEXT)
        {
            /* CT_TEXT must be synthesized so PM can see the text. */
            HANDLE hMem = clipboardSynthesizeText(CF_TEXT);
            if (hMem)
            {
                void *pv = GlobalLock(hMem);
                clipboardAddPM(CF_TEXT, pv, GlobalSize(hMem));
                GlobalUnlock(hMem);
            }
        }
        if (!fCF_UNICODETEXT)
            clipboardAddPMDummy(CF_UNICODETEXT);
        if (!fCF_OEMTEXT)
            clipboardAddPMDummy(CF_OEMTEXT);
    }
    else
    {
        /* DIBs */
        BOOL    fCF_DIBV5       = clipboardIsAvailable(CF_DIBV5);
        BOOL    fCF_DIB         = clipboardIsAvailable(CF_DIB);
        BOOL    fCF_BITMAP      = clipboardIsAvailable(CF_BITMAP);
        if (fCF_BITMAP || fCF_DIB || fCF_DIBV5)
        {
            /*
             * Add dummies.
             */
            if (!fCF_BITMAP)
            {   /* CT_BITMAP must be synthesized so PM can understand it. */
                HANDLE hbm = clipboardSynthesizeBitmap(CF_BITMAP);
                if (hbm)
                {
                    if (!SetClipboardData(CF_BITMAP, hbm))
                        DebugAssertFailed(("SetClipboardData for synthesized bitmap %#x failed!\n", hbm));
                }
            }
            if (!fCF_DIB)
                clipboardAddPMDummy(CF_DIB);
            if (!fCF_DIBV5)
                clipboardAddPMDummy(CF_DIBV5);
        }
        else
        {
            /** @todo metafiles and palettes */
        }
    }

}



/**
 * Checks whether or not a given format will be synthesized by GetClipboardData
 * in the current clipboard state.
 *
 * @returns TRUE if it will be synthesized, FALSE if not.
 * @param   uFormat     Odin format number
 */
BOOL clipboardShouldBeSynthesized(UINT uFormat)
{
    /* Should be, that means to be done in GetClipboardData. */
    if (clipboardIsAvailable(uFormat))
        return FALSE;

    switch (uFormat)
    {
        case CF_TEXT:
            return clipboardIsAvailable(CF_OEMTEXT)
                || clipboardIsAvailable(CF_UNICODETEXT);
        case CF_OEMTEXT:
            return clipboardIsAvailable(CF_TEXT)
                || clipboardIsAvailable(CF_UNICODETEXT);
        case CF_UNICODETEXT:
            return clipboardIsAvailable(CF_TEXT)
                || clipboardIsAvailable(CF_OEMTEXT);

        case CF_LOCALE:
            return clipboardIsAvailable(CF_TEXT)
                || clipboardIsAvailable(CF_OEMTEXT)
                || clipboardIsAvailable(CF_UNICODETEXT);

        case CF_BITMAP:
            return clipboardIsAvailable(CF_DIB)
                || clipboardIsAvailable(CF_DIBV5);
        case CF_DIB:
            return clipboardIsAvailable(CF_BITMAP)
                || clipboardIsAvailable(CF_DIBV5);
        case CF_DIBV5:
            return clipboardIsAvailable(CF_BITMAP)
                || clipboardIsAvailable(CF_DIB);

        #if 0
        case CF_PALETTE:
            return clipboardIsAvailable(CF_BITMAP)
                || clipboardIsAvailable(CF_DIB)
                || clipboardIsAvailable(CF_DIBV5);
        /* metafiles */
        #endif

        default:
            return FALSE;
    }
}


/**
 * Synthesizes the given text format from the available text format
 * on the clipboard.
 *
 * @returns Handle to global memory object containing the synthesized text.
 * @returns NULL on failure.
 * @param   uFormat     Format to synthesize.
 */
HANDLE   clipboardSynthesizeText(UINT uFormat)
{
    dprintf2(("USER32: clipboardSynthesizeText: uFormat=%d (%s)\n", uFormat, dbgGetFormatName(uFormat)));
    DebugAssert(uFormat == CF_TEXT || uFormat == CF_OEMTEXT || uFormat == CF_UNICODETEXT,
                ("invalid format %d\n", uFormat));

    /*
     * Check what's available.
     */
    BOOL fCF_TEXT       = clipboardIsAvailableReal(CF_TEXT);
    BOOL fCF_UNICODETEXT= clipboardIsAvailableReal(CF_UNICODETEXT);
    BOOL fCF_OEMTEXT    = clipboardIsAvailableReal(CF_OEMTEXT);
    DebugAssert(fCF_TEXT || fCF_UNICODETEXT || fCF_OEMTEXT, ("no text is available!!!"));

    /*
     * Figure out format to convert from.
     */
    UINT uSrcFormat;
    switch (uFormat)
    {
        case CF_TEXT:
            uSrcFormat = fCF_UNICODETEXT ? CF_UNICODETEXT : fCF_OEMTEXT ? CF_OEMTEXT : 0;
            break;
        case CF_OEMTEXT:
            uSrcFormat = fCF_UNICODETEXT ? CF_UNICODETEXT : fCF_TEXT    ? CF_TEXT    : 0;
            break;
        case CF_UNICODETEXT:
            uSrcFormat = fCF_TEXT        ? CF_TEXT        : fCF_OEMTEXT ? CF_OEMTEXT : 0;
            break;
    }
    dprintf2(("USER32: clipboardSynthesizeText: uSrcFormat=%d (%s)\n", uSrcFormat, dbgGetFormatName(uSrcFormat)));

    /*
     * Get the data.
     */
    HANDLE hRet = NULL;
    HANDLE hMem = GetClipboardData(uSrcFormat);
    if (hMem)
    {
        const void *pv = GlobalLock(hMem);
        if (pv)
        {
            DWORD   cb = GlobalSize(hMem);

            /*
             * determin size of converted text
             */
            unsigned cbRet;
            unsigned uCp;
            switch (uSrcFormat)
            {
                case CF_TEXT:
                case CF_OEMTEXT:
                    cbRet = strlen((char*)pv) + 1; /* OEM better not bet multibyte/dbcs. */
                    if (uFormat == CF_UNICODETEXT)
                        cbRet *= sizeof(WCHAR);
                    break;
                case CF_UNICODETEXT:
                    uCp = clipboardGetCodepage(uFormat == CF_TEXT ? LOCALE_IDEFAULTANSICODEPAGE : LOCALE_IDEFAULTCODEPAGE);
                    cbRet = WideCharToMultiByte(uCp, 0, (LPCWSTR)pv, cb / sizeof(WCHAR), NULL, 0, NULL, NULL);
                    break;
            }

            /*
             * Allocate memory.
             */
            PCLIPLOCALDATA pLocalClip;
            hRet = clipboardCacheAllocGlobalAlloc(uFormat, cbRet, &pLocalClip);
            if (hRet)
            {
                void * pvRet = GlobalLock(hRet);

                /*
                 * Do the convertion.
                 */
                switch (uFormat)
                {
                    case CF_TEXT:
                        switch (uSrcFormat)
                        {
                            case CF_OEMTEXT:
                                OemToCharBuffA((LPCSTR)pv, (LPSTR)pvRet, cbRet);
                                break;
                            case CF_UNICODETEXT:
                                WideCharToMultiByte(uCp, 0, (LPCWSTR)pv, cb / sizeof(WCHAR), (LPSTR)pvRet, cb, NULL, NULL);
                                break;
                        }
                        break;

                    case CF_OEMTEXT:
                        switch (uSrcFormat)
                        {
                            case CF_TEXT:
                                CharToOemBuffA((LPCSTR)pv, (LPSTR)pvRet, cbRet);
                                break;
                            case CF_UNICODETEXT:
                                WideCharToMultiByte(uCp, 0, (LPCWSTR)pv, cb / sizeof(WCHAR), (LPSTR)pvRet, cb, NULL, NULL);
                                break;
                        }
                        break;

                    case CF_UNICODETEXT:
                        uCp = clipboardGetCodepage(uFormat == CF_TEXT ? LOCALE_IDEFAULTANSICODEPAGE : LOCALE_IDEFAULTCODEPAGE);
                        MultiByteToWideChar(uCp, MB_PRECOMPOSED, (LPCSTR)pv, cb, (LPWSTR)pvRet, cbRet / sizeof(WCHAR));
                        break;
                }
                GlobalUnlock(hRet);

                hRet = clipboardCacheInsertNode(pLocalClip);
            }
            GlobalUnlock(hMem);
        }
        else
            DebugAssertFailed(("Failed to lock object %#x format %d!!!\n", hMem, uSrcFormat));
    }
    else
        DebugAssertFailed(("now why isn't %d on the clipboard????\n", uSrcFormat));

    return hRet;
}


/**
 * Synthesizes a locale structure.
 *
 * @returns Handle to global memory object containing the synthesized locale.
 * @returns NULL on failure.
 * @param   uFormat     Format to synthesize.
 */
HANDLE   clipboardSynthesizeLocale(UINT uFormat)
{
    dprintf2(("USER32: clipboardSynthesizeLocale: uFormat=%d (%s)\n", uFormat, dbgGetFormatName(uFormat)));
    DebugAssert(uFormat == CF_LOCALE, ("invalid format %d\n", uFormat));

    /*
     * Guess an appropriate Locale.
     */
    LCID    lcid = GetSystemDefaultLCID();

    /*
     * Allocate a block in the 'cache'.
     */
    PCLIPLOCALDATA pLocalClip;
    HANDLE hMem = clipboardCacheAllocGlobalDup(uFormat, &lcid, sizeof(LCID), &pLocalClip);
    if (!hMem)
        return NULL;

    /* insert */
    hMem = clipboardCacheInsertNode(pLocalClip);

    dprintf2(("USER32: clipboardSynthesizeLocale: returning %#x (lcid=%#x)\n", hMem, lcid));
    return hMem;
}


/**
 * Synthesizes a bitmap from the avilable DIB format on the clipboard
 *
 * @returns Handle to GDI bitmap.
 * @returns NULL on failure.
 * @param   uFormat     Format to synthesize. (CF_BITMAP)
 */
HANDLE   clipboardSynthesizeBitmap(UINT uFormat)
{
    dprintf2(("USER32: clipboardSynthesizeBitmap: uFormat=%d (%s)\n", uFormat, dbgGetFormatName(uFormat)));
    DebugAssert(uFormat == CF_BITMAP, ("invalid format %d\n", uFormat));

    /*
     * Find DIB format to convert from.
     */
    UINT    uSrcFormat = CF_DIB;
    if (!clipboardIsAvailableReal(uSrcFormat))
    {
        uSrcFormat = CF_DIBV5;
        if (!clipboardIsAvailableReal(uSrcFormat))
        {
            DebugAssertFailed(("no DIB data avilable!\n"));
            return NULL;
        }
    }
    dprintf2(("USER32: clipboardSynthesizeBitmap: uSrcFormat=%d (%s)\n", uSrcFormat, dbgGetFormatName(uSrcFormat)));

    /*
     * Get the data.
     */
    PCLIPHEADER pClip = (PCLIPHEADER)OSLibWinQueryClipbrdData(GetThreadHAB(), clipboardOdinToPMFormat(uSrcFormat));
    if (    !pClip
        ||  pClip == (PCLIPHEADER)DUMMY_HANDLE
        ||  memcmp(pClip->achMagic, CLIPHEADER_MAGIC, sizeof(CLIPHEADER_MAGIC)))
    {
        DebugAssertFailed(("invalid clipboard data for format %d\n", uSrcFormat));
        return NULL;
    }

    /*
     * Check the content a little bit.
     */
    union u
    {
        PVOID               pv;
        char *              pch;
        LPBITMAPINFOHEADER  pHdr;
        LPBITMAPV5HEADER    pHdrV5;
    }       u = { (void*)(pClip + 1) };
    if (u.pHdr->biSize < sizeof(BITMAPCOREHEADER) || u.pHdr->biSize > sizeof(BITMAPV5HEADER))
    {
        DebugAssertFailed(("invalid clipboard DIB data: biSize=%d\n", u.pHdr->biSize));
        return NULL;
    }
    switch (u.pHdr->biBitCount)
    {
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 24:
        case 32:
            break;
        default:
            DebugAssertFailed(("invalid clipboard DIB data: biBitCount=%d\n", u.pHdr->biBitCount));
            return NULL;
    }
    if (    u.pHdrV5->bV5Size == sizeof(BITMAPV5HEADER)
        &&  u.pHdrV5->bV5CSType != LCS_sRGB)
    {
        DebugAssertFailed(("unexpected clipboard DIBV5 data: bV5CSType=%#x\n", u.pHdrV5->bV5CSType));
        return NULL;
    }
    /* todo more checking. */

    /*
     * Calc start of bits.
     */
    void *pvBits = (char*)u.pv + u.pHdr->biSize;
    if (u.pHdr->biBitCount <= 8)
        pvBits = (char*)pvBits + sizeof(RGBQUAD) * (1 << u.pHdr->biBitCount);

    /*
     * Create the bitmap.
     */
    HBITMAP hbm;
    HDC hdc = CreateDCA("DISPLAY", NULL, NULL, NULL);
    if (hdc)
    {
        hbm = CreateDIBitmap(hdc, u.pHdr, CBM_INIT, pvBits, (PBITMAPINFO)u.pHdr, DIB_RGB_COLORS);
        if (hbm)
        {
            /* allocate and insert it into the cache. */
            PCLIPLOCALDATA pLocalClip;
            if (clipboardCacheAllocGDI(uFormat, hbm, &pLocalClip))
                hbm = clipboardCacheInsertNode(pLocalClip);
            dprintf(("synthesized bitmap %#x\n", hbm));
        }
        else
            DebugAssertFailed(("CreateBitmapIndirect failed\n"));
        DeleteDC(hdc);
    }

    return hbm;
}

/**
 * Synthesizes the given DIB format from the available DIB or bitmap
 * on the clipboard.
 *
 * @returns Handle to global memory object containing the synthesized DIB.
 * @returns NULL on failure.
 * @param   uFormat     Format to synthesize. (CF_BITMAP)
 */
HANDLE   clipboardSynthesizeDIB(UINT uFormat)
{
    dprintf2(("USER32: clipboardSynthesizeDIB: uFormat=%d (%s)\n", uFormat, dbgGetFormatName(uFormat)));
    OSLIB_BITMAPINFOHEADER2 bih2;
    DebugAssert(uFormat == CF_DIB || uFormat == CF_DIBV5, ("invalid format %d\n", uFormat));

    /*
     * Find convert format and calculate the size of the converted bitmap.
     */
    UINT    uSrcFormat = CF_DIB;
    if (!clipboardIsAvailableReal(uSrcFormat))
    {
        uSrcFormat = CF_DIBV5;
        if (!clipboardIsAvailableReal(uSrcFormat))
        {
            uSrcFormat = CF_BITMAP;
            if (!clipboardIsAvailableReal(uSrcFormat))
            {
                DebugAssertFailed(("no bitmap or DIB available on the clipboard\n"));
                return NULL;
            }
        }
    }
    dprintf2(("USER32: clipboardSynthesizeDIB: uSrcFormat=%d (%s)\n", uSrcFormat, dbgGetFormatName(uSrcFormat)));

    /*
     * Get the data.
     */
    void       *pv = (void*)OSLibWinQueryClipbrdData(GetThreadHAB(), clipboardOdinToPMFormat(uSrcFormat));
    if (    !pv
        ||  pv == (void*)DUMMY_HANDLE
        ||  (   uSrcFormat != CF_BITMAP
             && memcmp(pv, CLIPHEADER_MAGIC, sizeof(CLIPHEADER_MAGIC))
                )
            )
    {
        DebugAssertFailed(("invalid clipboard data for format %d\n", uSrcFormat));
        return NULL;
    }


    /*
     * Do the conversion.
     */
    switch (uSrcFormat)
    {
        /*
         * Convert an OS/2 bitmap to a DIB or DIBV5.
         */
        case CF_BITMAP:
        {
            /* We're letting GPI do the major bit of the work actually. */
            HBITMAP hbm = O32_CreateBitmapFromPMHandle((HBITMAP)pv);
            if (!hbm)
            {
                DebugAssertFailed(("invalid OS/2 hbitmap %#x on clipboard\n", (unsigned)pv));
                return NULL;
            }

            HANDLE hMem = NULL;
            HDC hdc = CreateDCA("DISPLAY", NULL, NULL, NULL);
            if (hdc)
            {
                SelectObject(hdc, hbm);

                /*
                 * Figure out the sizes.
                 */
                unsigned cbTrgHeader = uFormat == CF_DIB ? sizeof(BITMAPINFOHEADER) : sizeof(BITMAPV5HEADER);
                BITMAPINFOHEADER        hdr = {sizeof(BITMAPINFOHEADER), 0};
                if (GetDIBits(hdc, hbm, 0, 0, NULL, (PBITMAPINFO)&hdr, DIB_RGB_COLORS))
                {   /* we've got the header now */
                    unsigned    cbTrgColorTable = 0;
                    switch (hdr.biBitCount)
                    {
                        case 1:
                        case 2:
                        case 4:
                        case 8:
                            cbTrgColorTable = (1 << hdr.biBitCount) * sizeof(RGBQUAD);
                            break;
                    }

                    unsigned cbTrgBits = hdr.biSizeImage;
                    if (!cbTrgBits)
                    {
                        cbTrgBits = ((hdr.biWidth * hdr.biBitCount + 31) >> 5) << 2;
                        cbTrgBits *= abs(hdr.biHeight);
                    }

                    unsigned cbTotal = cbTrgHeader + cbTrgColorTable + cbTrgBits;

                    /*
                     * Allocate a cache object.
                     */
                    PCLIPLOCALDATA pLocalClip;
                    hMem = clipboardCacheAllocGlobalAlloc(uFormat, cbTotal, &pLocalClip);
                    if (hMem)
                    {
                        union u
                        {
                            PVOID               pv;
                            char *              pch;
                            LPBITMAPINFOHEADER  pHdr;
                            LPBITMAPV5HEADER    pHdrV5;
                        }           uTrg;
                        uTrg.pv = GlobalLock(hMem);

                        /*
                         * Get the bitmap bits and pieces.
                         */
                        memcpy(uTrg.pv, &hdr, sizeof(hdr));
                        if (GetDIBits(hdc, hbm, 0, hdr.biHeight,
                                      uTrg.pch + cbTrgHeader + cbTrgColorTable,
                                      (PBITMAPINFO)uTrg.pv,
                                      DIB_RGB_COLORS))
                        {
                            /* adjust the header+color table for CF_DIBV5 */
                            #ifdef USE_DIBV5
                            if (uFormat == CF_DIBV5)
                            {
                                if (cbTrgColorTable)
                                    memmove(uTrg.pch + sizeof(BITMAPV5HEADER), uTrg.pch + sizeof(hdr), cbTrgColorTable);
                                memset(uTrg.pch + sizeof(hdr), 0, sizeof(BITMAPV5HEADER) - sizeof(hdr));
                                uTrg.pHdrV5->bV5Size = sizeof(BITMAPV5HEADER);
                                uTrg.pHdrV5->bV5CSType = LCS_sRGB;
                            }
                            #endif
                            if (!uTrg.pHdr->biClrImportant)
                                uTrg.pHdr->biClrImportant = uTrg.pHdr->biClrUsed;

                            DeleteDC(hdc);
                            HANDLE hRet = clipboardCacheInsertNode(pLocalClip);
                            dprintf2(("USER32: clipboardSynthesizeDIB: returning %#x\n", hRet));
                            return hRet;
                        }
                        else
                            DebugAssertFailed(("GetDIBits failed lasterr=%d\n", GetLastError()));

                        /* failure */
                        clipboardCacheDeleteNode(pLocalClip);
                    }
                    else
                        DebugAssertFailed(("GlobalAlloc failed lasterr=%d\n", GetLastError()));
                }
                else
                    DebugAssertFailed(("GetDIBits (header) failed lasterr=%d\n", GetLastError()));
                DeleteDC(hdc);
            }
            else
                DebugAssertFailed(("CreateDCA failed lasterr=%d\n", GetLastError()));
            return NULL;
        }

        /*
         * DIB <-> DIBV5: basically only header adjustments
         * (We do not yet support advanced DIBV5 formats.)
         */
        case CF_DIB:
        case CF_DIBV5:
        {
            union u
            {
                PVOID               pv;
                char *              pch;
                LPBITMAPINFOHEADER  pHdr;
                LPBITMAPV5HEADER    pHdrV5;
            }           uSrc, uTrg;
            uSrc.pv = (char*)pv + sizeof(CLIPHEADER);
            unsigned    cbSrc = ((PCLIPHEADER)pv)->cbData;

            /*
             * Validate the bitmap data a little bit.
             */
            if (uSrc.pHdr->biSize < sizeof(BITMAPCOREHEADER) || uSrc.pHdr->biSize > sizeof(BITMAPV5HEADER))
            {
                DebugAssertFailed(("Unknown header size %d\n", uSrc.pHdr->biSize));
                return NULL;
            }
            switch (uSrc.pHdr->biBitCount)
            {
                case 1:
                case 2:
                case 4:
                case 8:
                case 16:
                case 24:
                case 32:
                    break;
                default:
                    DebugAssertFailed(("invalid clipboard DIB data: biBitCount=%d\n", uSrc.pHdr->biBitCount));
                    return NULL;
            }
            if (    uSrc.pHdrV5->bV5Size == sizeof(BITMAPV5HEADER)
                &&  uSrc.pHdrV5->bV5CSType != LCS_sRGB)
            {
                DebugAssertFailed(("unexpected clipboard DIBV5 data: bV5CSType=%#x\n", uSrc.pHdrV5->bV5CSType));
                return NULL;
            }

            /*
             * Figure out the size of the target bitmap and allocate memory for this.
             */
            unsigned        cbTrgHeader = uFormat == CF_DIB ? sizeof(BITMAPINFOHEADER) : sizeof(BITMAPV5HEADER);
            unsigned        cbTotal = cbSrc + (cbTrgHeader - uSrc.pHdr->biSize);
            PCLIPLOCALDATA  pLocalClip;
            HANDLE          hMem = clipboardCacheAllocGlobalAlloc(uFormat, cbTotal, &pLocalClip);
            if (!hMem)
                return NULL;
            uTrg.pv = GlobalLock(hMem);

            /*
             * Convert the bitmap data.
             * Note that the source and target DIB data might be in the same format.
             */
            if (cbTrgHeader != uTrg.pHdr->biSize)
            {
                /** @todo Only a subset of DIB V5 bitmaps are supported, implement it fully all around Odin32. */
                switch (uFormat)
                {
                    case CF_DIBV5:
                    #ifdef USE_DIBV5
                        memcpy(uTrg.pv, uSrc.pv, uSrc.pHdr->biSize);
                        memset(uTrg.pch + uSrc.pHdr->biSize, 0, sizeof(BITMAPV5HEADER) - uSrc.pHdr->biSize);
                        uTrg.pHdrV5->bV5Size = sizeof(BITMAPV5HEADER);
                        uTrg.pHdrV5->bV5CSType = LCS_sRGB;
                        memcpy(uTrg.pch + sizeof(BITMAPV5HEADER), uSrc.pch + uSrc.pHdr->biSize, cbSrc - uSrc.pHdr->biSize);
                        break;
                    #else
                        //fallthru
                    #endif
                    case CF_DIB:
                        memcpy(uTrg.pv, uSrc.pv, std::min<unsigned int>(uSrc.pHdr->biSize, sizeof(BITMAPINFOHEADER)));
                        if (uSrc.pHdr->biSize < sizeof(BITMAPINFOHEADER))
                            memset(uTrg.pch + uSrc.pHdr->biSize, 0, sizeof(BITMAPINFOHEADER) - uSrc.pHdr->biSize);
                        memcpy(uTrg.pch + sizeof(BITMAPINFOHEADER), uSrc.pch + uSrc.pHdr->biSize, cbSrc - uSrc.pHdr->biSize);
                        break;
                }
            }
            GlobalUnlock(hMem);
            /* done */
            return hMem;
        }
    }

    return NULL;
}

#if 0 // not implemented yet
HANDLE   clipboardSynthesizePalette(UINT uFormat)
{
    return NULL;
}

HANDLE   clipboardSynthesizeMetaFile(UINT uFormat)
{
    return NULL;
}

HANDLE   clipboardSynthesizeEnhMetaFile(UINT uFormat)
{
    return NULL;
}
#endif


/**
 * Get the length of a text on the clipboard.
 *
 * @returns up to cb.
 * @param   pvData  Data pointer.
 * @param   cb      Data size.
 */
unsigned clipboardTextCalcSize(void *pvData, unsigned cb)
{
    const char *psz = (const char*)pvData;
    const char *pszEnd = psz + cb;
    while (psz + 1 < pszEnd)
    {
        if (!*psz && !psz[1])     /* Check for two '\0' to keep out of dbcs trouble. */
            return psz - (char*)pvData + 1;
        psz++;
    }
    return cb;
}


/**
 * Gets the clipboad code page using the CF_LOCALE entry if present, if not
 * present the system default is used.
 *
 * @returns Codepage corresponding to uType.
 * @param   uType   Code page type.
 */
unsigned clipboardGetCodepage(UINT uType)
{
    /*
     * Find the LCID to use.
     */
    HANDLE hMem = GetClipboardData(CF_LOCALE);
    LCID    lcid;
    LCID   *plcid;
    if (hMem && (plcid = (LCID *)GlobalLock(hMem)) != NULL)
    {
        lcid = *plcid;
        GlobalUnlock(hMem);
    }
    else
        lcid = GetSystemDefaultLCID();

    char szCp[6];
    if (GetLocaleInfoA(lcid, uType, szCp, sizeof(szCp)))
        return atoi(szCp);

    switch (uType)
    {
        case LOCALE_IDEFAULTCODEPAGE    : return CP_OEMCP;
        case LOCALE_IDEFAULTANSICODEPAGE: return CP_ACP;
        default                         : return CP_MACCP;
    }
}


/**
 * GlobalAllocs a block of memory for the cache initializing it from the
 * the memory block specified pv.
 *
 * @returns Handle to 'global' memory object.
 * @returns NULL on failure.
 * @param   uFormat     Odin format number.
 * @param   pv          Pointer to memory block to duplicate.
 * @param   cb          Number of bytes to allocate and duplicate.
 * @param   ppLocalClip Where to put the cache structure.
 */
HANDLE   clipboardCacheAllocGlobalDup(UINT uFormat, const void *pv, unsigned cb, PCLIPLOCALDATA *ppClip)
{
    dprintf2(("USER32: clipboardCacheAllocGlobalDup: uFormat=%d (%s) pv=%p cb=%d\n",
              uFormat, dbgGetFormatName(uFormat), pv, cb));

    /*
     * Allocate cache block and copy data.
     */
    HANDLE hMem = clipboardCacheAllocGlobalAlloc(uFormat, cb, ppClip);
    if (hMem)
    {
        PVOID pvMem = GlobalLock(hMem);
        memcpy(pvMem, pv, cb);
        GlobalUnlock(hMem);
    }

    return hMem;
}



/**
 * GlobalAllocs a block of memory for the cache.
 *
 * @returns Handle to 'global' memory object.
 * @returns NULL on failure.
 * @param   uFormat     Odin format number.
 * @param   cb          Number of bytes to allocate.
 * @param   ppLocalClip Where to put the cache structure.
 */
HANDLE clipboardCacheAllocGlobalAlloc(UINT uFormat, unsigned cb, PCLIPLOCALDATA *ppLocalClip)
{
    dprintf2(("USER32: clipboardCacheAllocGlobalAlloc: uFormat=%d (%s) cb=%d\n",
              uFormat, dbgGetFormatName(uFormat), cb));
    /*
     * Allocate object.
     */
    HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, cb);
    if (!hMem)
    {
        dprintf(("USER32: GetClipboardData: returns NULL (GlobalAlloc(,%d) failed)\n", cb));
        return NULL;
    }

    /*
     * Allocate and initialize cache node.
     */
    if (clipboardCacheAllocGlobalMem(uFormat, hMem, ppLocalClip))
        return hMem;

    /* failed */
    GlobalFree(hMem);
    return NULL;
}


/**
 * Allocates a cache node for a global memory handle.
 *
 * @returns Handle to 'global' memory object.
 * @returns NULL on failure.
 * @param   uFormat     Odin format number.
 * @param   cb          Number of bytes to allocate.
 * @param   ppLocalClip Where to put the cache structure.
 */
BOOL clipboardCacheAllocGlobalMem(UINT uFormat, HANDLE hMem, PCLIPLOCALDATA *ppLocalClip)
{
    dprintf2(("USER32: clipboardCacheAllocGlobalMem: uFormat=%d (%s) hMem=%#x\n",
              uFormat, dbgGetFormatName(uFormat), hMem));

    /*
     * Allocate and initialize cache node.
     */
    PCLIPLOCALDATA  pLocalClip = (PCLIPLOCALDATA)malloc(sizeof(*pLocalClip));
    *ppLocalClip = pLocalClip;
    if (!pLocalClip)
        return FALSE;

    pLocalClip->enmType   = CLIPLOCALDATA::enmGlobalMem;
    pLocalClip->h         = hMem;
    pLocalClip->uFormat   = uFormat;
    pLocalClip->pNext     = NULL;

    return TRUE;
}


/**
 * Allocates a cache node for a GDI handle.
 *
 * @returns hGDI on success.
 * @returns NULL on failure.
 * @param   uFormat     Clipboard format number.
 * @param   hGDI        GDI handle.
 * @param   ppLocalClip Where to store the pointer to the allocated cache node.
 */
BOOL    clipboardCacheAllocGDI(UINT uFormat, HANDLE hGDI, PCLIPLOCALDATA *ppLocalClip)
{
    dprintf2(("USER32: clipboardCacheAllocGDI: uFormat=%d (%s) hGdi=%#x\n",
              uFormat, dbgGetFormatName(uFormat), hGDI));
    /*
     * Allocate and initialize cache node.
     */
    PCLIPLOCALDATA  pLocalClip = (PCLIPLOCALDATA)malloc(sizeof(*pLocalClip));
    *ppLocalClip = pLocalClip;
    if (!pLocalClip)
        return FALSE;

    pLocalClip->enmType   = CLIPLOCALDATA::enmGDI;
    pLocalClip->h         = hGDI;
    pLocalClip->uFormat   = uFormat;
    pLocalClip->pNext     = NULL;

    return TRUE;
}


/**
 * Inserts a previously allocated local cache node.
 *
 * Since this is a cache, pNew might be rejected because of the data
 * already existing in the cache. Therefor always use the returned handle.
 *
 * @returns The handle in the cached node.
 * @param   pNew    Node to insert.
 */
HANDLE clipboardCacheInsertNode(PCLIPLOCALDATA pNew)
{
    dprintf2(("USER32: clipboardCacheInsertNode: pNew=%p format=%d (%s) h=%#x\n",
              pNew, pNew->uFormat, dbgGetFormatName(pNew->uFormat), pNew->h));
    /*
     * Find the previous node with the same format.
     */
    PCLIPLOCALDATA p = gpLocalData;
    PCLIPLOCALDATA pPrev = NULL;
    while (p && p->uFormat != pNew->uFormat)
        p = p->pNext;
    if (p)
    {
        /*
         * If the old and new cache nodes are idendical we'll keep the old one.
         */
        if (p->enmType == pNew->enmType)
        {
            switch (p->enmType)
            {
                case CLIPLOCALDATA::enmGlobalMem:
                {
                    DWORD cb = GlobalSize(p->h);
                    if (cb == GlobalSize(pNew->h))
                    {
                        PVOID pv = GlobalLock(p->h);
                        PVOID pvNew = GlobalLock(pNew->h);
                        int diff = -1;
                        if (pv && pvNew)
                            diff = memcmp(pv, pvNew, cb);
                        GlobalUnlock(pNew->h);
                        GlobalUnlock(p->h);
                        if (!diff)
                        {
                            clipboardCacheDeleteNode(pNew);
                            dprintf2(("USER32: clipboardCacheInsertNode: returning existing node %#x (cache hit)\n", p->h));
                            return p->h;
                        }
                    }
                    break;
                }

                case CLIPLOCALDATA::enmGDI:
                    /** @todo */
                    break;

                case CLIPLOCALDATA::enmPrivate:
                default:
                    DebugAssertFailed(("Don't know how to handle this type\n"));
                    break;
            }

        }

        /*
         * Remove and delete the old node.
         */
        dprintf2(("USER32: clipboardCacheInsertNode: removing old node %p h=%#x\n", p, p->h));
        if (pPrev)
            pPrev->pNext = p->pNext;
        else
            gpLocalData = p->pNext;
        clipboardCacheDeleteNode(p);
    }

    /*
     * Insert the new node.
     */
    pNew->pNext = gpLocalData;
    gpLocalData = pNew;

    return pNew->h;
}


/**
 * Delete Odin resources associated with a cache node.
 *
 * The node must be unlinked from the cache before this function is called.
 *
 * @param   p   Node to delete.
 */
void clipboardCacheDeleteNode(PCLIPLOCALDATA p)
{
    dprintf2(("USER32: clipboardCacheDeleteNode: p=%p format=%d (%s) h=%#x\n",
              p, p->uFormat, dbgGetFormatName(p->uFormat), p->h));
    switch (p->enmType)
    {
        case CLIPLOCALDATA::enmGlobalMem:
            if (GlobalFree(p->h))
                DebugAssertFailed(("GlobalFree failed freeing %#x (format %d). lasterr=%d\n",
                                   p->h, p->uFormat, GetLastError()));
            p->h = NULL;
            break;

        case CLIPLOCALDATA::enmGDI:
            if (!DeleteObject(p->h))
                DebugAssertFailed(("DeleteObject failed freeing %#x (format %d). lasterr=%d\n",
                                   p->h, p->uFormat, GetLastError()));
            p->h = NULL;
            break;

        case CLIPLOCALDATA::enmPrivate:
            DebugAssertFailed(("shit! don't know how to handle this cache type!"));
            break;
        default:
            DebugAssertFailed(("!cache corrupted! invalid type %d", p->enmType));
            break;
    }
    free(p);
}

