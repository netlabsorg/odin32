#define INCL_GPI
#define INCL_DOSMODULEMGR
#define INCL_ORDINALS
#define INCL_WINHOOKS
#include <os2wrap.h>
#include <os2im.h>
#include <os2ime.h>
#include <win32type.h>
#include <winuser32.h>
#include <win32api.h>
#include <stdlib.h>
#include <heapstring.h>

#include "im32.h"

#define CP_ACP  0

extern BOOL WINAPI IsDBCSLeadByte(BYTE);

typedef APIRET ( APIENTRY *PFN_IMASSOCIATEINSTANCE )( HWND hwnd, HIMI himi, PHIMI phimiPrev );

typedef APIRET ( APIENTRY *PFN_IMCREATEINSTANCE )( PHIMI phimi );

typedef APIRET ( APIENTRY *PFN_IMDESTROYINSTANCE )( HIMI himi );

typedef APIRET ( APIENTRY *PFN_IMGETINSTANCE )( HWND hwnd, PHIMI phimi );

typedef APIRET ( APIENTRY *PFN_IMQUERYDEFAULTIMWINDOW )( HWND hwnd, PHWND phwnd );

typedef APIRET ( APIENTRY *PFN_IMRELEASEINSTANCE )( HWND hwnd, HIMI himi );

typedef APIRET ( APIENTRY *PFN_IMENUMREGISTERWORD )( HIMI himi,
                                    REGISTERWORDENUMPROC pfnEnumProc,
                                    PSZ pReading,
                                    ULONG ulType,
                                    PSZ pRegister,
                                    PVOID pData );

typedef APIRET ( APIENTRY *PFN_IMQUERYREGISTERWORDTYPE )( HIMI himi, PULONG pulCount,
                                         PWORDTYPE pWordType );


typedef APIRET ( APIENTRY *PFN_IMREGISTERWORD )( HIMI himi,
                                PSZ pszReading,
                                ULONG ulType,
                                PSZ pszRegister );

typedef APIRET ( APIENTRY *PFN_IMDEREGISTERWORD )( HIMI himi,
                                  PSZ pszReading,
                                  ULONG ulType,
                                  PSZ pszDeregister );

typedef APIRET ( APIENTRY *PFN_IMSHOWIMEDLG )( HIMI himi,
                             ULONG ulDlgType,
                             PREGISTERWORDHEADER pRegWord );

typedef APIRET ( APIENTRY *PFN_IMESCAPE )( HIMI himi, ULONG ulEscaoe, PVOID pData );

typedef APIRET ( APIENTRY *PFN_IMQUERYCANDIDATELIST )( HIMI himi,
                                      ULONG ulIndex,
                                      PCANDIDATELISTHEADER pCandidateList,
                                      PULONG pulBuffer );

typedef APIRET ( APIENTRY *PFN_IMQUERYCANDIDATEWINDOWPOS )( HIMI himi,
                                           PCANDIDATEPOS pCandidatePos );

typedef APIRET ( APIENTRY *PFN_IMSETCANDIDATEWINDOWPOS )( HIMI himi,
                                         PCANDIDATEPOS pCandidatePos );

typedef APIRET ( APIENTRY *PFN_IMQUERYCONVERSIONFONT )( HIMI himi,
                                       PFATTRS pFontAttrs );

typedef APIRET ( APIENTRY *PFN_IMSETCONVERSIONFONT )( HIMI himi,
                                     PFATTRS pFontAttrs );

typedef APIRET ( APIENTRY *PFN_IMQUERYCONVERSIONFONTSIZE )( HIMI himi,
                                           PSIZEF psizfxBox );

typedef APIRET ( APIENTRY *PFN_IMSETCONVERSOINFONTSIZE )( HIMI himi,
                                         PSIZEF psizfxBox );

typedef APIRET ( APIENTRY *PFN_IMGETCONVERSIONSTRING )( HIMI himi, ULONG ulIndex,
                                       PVOID pBuf, PULONG pulBufLen );

typedef APIRET ( APIENTRY *PFN_IMSETCONVERSIONSTRING )( HIMI himi, ULONG ulIndex,
                                         PVOID pConv, ULONG ulConvLen,
                                         PVOID pReading, ULONG ulReadingLen );

typedef APIRET ( APIENTRY *PFN_IMQUERYCONVERSIONANGLE )( HIMI himi, PGRADIENTL pgradlAngle );

typedef APIRET ( APIENTRY *PFN_IMSETCONVERSIONANGLE )( HIMI himi, PGRADIENTL pgradlAngle );

typedef APIRET ( APIENTRY *PFN_IMQUERYCONVERSIONWINDOWPOS )( HIMI himi,
                                            PCONVERSIONPOS pConvPos );

typedef APIRET ( APIENTRY *PFN_IMSETCONVERSIONWINDOWPOS )( HIMI himi,
                                          PCONVERSIONPOS pConvPos );

typedef APIRET ( APIENTRY *PFN_IMGETRESULTSTRING )( HIMI himi, ULONG ulIndex,
                                   PVOID pBuf, PULONG pulBufLen );

typedef APIRET ( APIENTRY *PFN_IMQUERYINFOMSG )( HIMI himi, ULONG ulIndex,
                                     PVOID pBuf, PULONG pulBufLen );

typedef APIRET ( APIENTRY *PFN_IMQUERYSTATUSWINDOWPOS )( HIMI himi, PPOINTL pptPos, PSIZEL pszlSize );

typedef APIRET ( APIENTRY *PFN_IMSETSTATUSWINDOWPOS )( HIMI himi, PPOINTL pptPos );

typedef APIRET ( APIENTRY *PFN_IMSHOWSTATUSWINDOW )( HIMI himi, ULONG fShow );

typedef APIRET ( APIENTRY *PFN_IMGETSTATUSSTRING )( HIMI himi, ULONG ulIndex,
                                   PVOID pBuf, PULONG pulBufLen );

typedef APIRET ( APIENTRY *PFN_IMCONVERTSTRING )( HIMI himi,
                                 PSZ pSrc,
                                 PCANDIDATELISTHEADER pDst,
                                 PULONG pulBufLen,
                                 ULONG ulFlag );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMMODE )( HIMI himi,
                               PULONG pulInputMode,
                               PULONG pulConversionMode );

typedef APIRET ( APIENTRY *PFN_IMSETIMMODE )( HIMI himi,
                             ULONG ulInputMode,
                             ULONG ulConversionMode );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMEINFO )( ULONG ImeId, PIMEINFOHEADER pImeInfoHeader,
                                PULONG pulBufLen );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMEPROPERTY )( HIMI himi, ULONG ulIndex, PULONG pulProp );

typedef APIRET ( APIENTRY *PFN_IMREQUESTIME )( HIMI himi, ULONG ulAction,
                              ULONG ulIndex, ULONG ulValue );

typedef APIRET ( APIENTRY *PFN_IMISIMEMESSAGE )( HWND hwndIm,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2,
                                PBOOL pfResult );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMELIST )( ULONG ulCodepage,
                                PULONG aImeId,
                                PULONG pulCount );

typedef APIRET ( APIENTRY *PFN_IMQUERYCURRENTIME )( HIMI himi, PULONG pImeId );

typedef APIRET ( APIENTRY *PFN_IMSETCURRENTIME )( HIMI himi,
                                 ULONG ulAction,
                                 ULONG ImeId );

typedef APIRET ( APIENTRY *PFN_IMQUERYDEFAULTIME )( ULONG ulCodepage, PULONG pImeId );

typedef APIRET ( APIENTRY *PFN_IMSETDEFAULTIME )( ULONG ulCodepage, ULONG ImeId );


typedef APIRET ( APIENTRY *PFN_IMSETIMELISTORDER )( ULONG ulCodepage,
                                   ULONG ulAction,
                                   ULONG ImeIdTarget,
                                   ULONG ImeIdIndex );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMEID )( PSZ pszIMEName, PULONG pImeId );

typedef APIRET ( APIENTRY *PFN_IMQUERYMSGQUEUEPROPERTY )( HMQ hmq, PULONG pulFlag );

typedef APIRET ( APIENTRY *PFN_IMSETMSGQUEUEPROPERTY )( HMQ hmq, ULONG ulFlag );


typedef APIRET ( APIENTRY *PFN_IMREQUESTEVENT)( HIMI hImi,
                                ULONG msg,
                                ULONG mp1,
                                ULONG mp2 );

typedef APIRET ( APIENTRY *PFN_IMREQUESTIMINSTANCE)( HIMI hImi,
                                     PIMINSTANCE * ppIMInstance );

typedef APIRET ( APIENTRY *PFN_IMRELEASEIMINSTANCE)( HIMI hImi );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMINSTANCEREQCOUNT)( HIMI hImi,
                                           PULONG pulCount );

typedef APIRET ( APIENTRY *PFN_IMCREATEIMIPART)( ULONG ulPartSize,
                                 PHIMIP phimiPart );

typedef APIRET ( APIENTRY *PFN_IMDESTROYIMIPART)( HIMIP himiPart );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMIPARTSIZE)( HIMIP himiPart,
                                    PULONG pulPartSize );

typedef APIRET ( APIENTRY *PFN_IMREQUESTIMIPART)( HIMIP himiPart,
                                  PVOID * ppPartData );

typedef APIRET ( APIENTRY *PFN_IMRELEASEIMIPART)( HIMIP himiPart );

typedef APIRET ( APIENTRY *PFN_IMRESIZEIMIPART)( HIMIP himiPart,
                                 ULONG ulNewSize );

typedef APIRET ( APIENTRY *PFN_IMQUERYIMIPARTREQCOUNT)( HIMIP himiPart,
                                        PULONG pulCount );


typedef APIRET ( APIENTRY *PFN_IMBROADCASTDATA)( PSZ pszIMEName,
                                 ULONG ulIndex,
                                 PBROADCASTDATAHEADER pData );

typedef APIRET ( APIENTRY *PFN_IMGETINSTANCEMQ )( HMQ hmq, PHIMI phimi );

typedef APIRET ( APIENTRY *PFN_IMRELEASEINSTANCEMQ )( HMQ hmq, HIMI himi );

static BOOL fIM32Inited = FALSE;

static HMODULE hIM32Mod = NULLHANDLE;

static PFN_IMASSOCIATEINSTANCE pfnImAssociateInstance = NULL;

static PFN_IMCREATEINSTANCE pfnImCreateInstance = NULL;

static PFN_IMDESTROYINSTANCE pfnImDestroyInstance = NULL;

static PFN_IMGETINSTANCE pfnImGetInstance = NULL;

static PFN_IMQUERYDEFAULTIMWINDOW pfnImQueryDefaultIMWindow = NULL;

static PFN_IMRELEASEINSTANCE pfnImReleaseInstance = NULL;

static PFN_IMENUMREGISTERWORD pfnImEnumRegisterWord = NULL;

static PFN_IMQUERYREGISTERWORDTYPE pfnImQueryRegisterWordType = NULL;

static PFN_IMREGISTERWORD pfnImRegisterWord = NULL;

static PFN_IMDEREGISTERWORD pfnImDeregisterWord = NULL;

static PFN_IMSHOWIMEDLG pfnImShowIMEDlg = NULL;

static PFN_IMESCAPE pfnImEscape = NULL;

static PFN_IMQUERYCANDIDATELIST pfnImQueryCandidateList = NULL;

static PFN_IMQUERYCANDIDATEWINDOWPOS pfnImQueryCandidateWindowPos = NULL;

static PFN_IMSETCANDIDATEWINDOWPOS pfnImSetCandidateWindowPos = NULL;

static PFN_IMQUERYCONVERSIONFONT pfnImQueryConversionFont = NULL;

static PFN_IMSETCONVERSIONFONT pfnImSetConversionFont = NULL;

static PFN_IMQUERYCONVERSIONFONTSIZE pfnImQueryConversionFontSize = NULL;

static PFN_IMSETCONVERSOINFONTSIZE pfnImSetConversionFontSize = NULL;

static PFN_IMGETCONVERSIONSTRING pfnImGetConversionString = NULL;

static PFN_IMSETCONVERSIONSTRING pfnImSetConversionString = NULL;

static PFN_IMQUERYCONVERSIONANGLE pfnImQueryConversionAngle = NULL;

static PFN_IMSETCONVERSIONANGLE pfnImSetConversionAngle = NULL;

static PFN_IMQUERYCONVERSIONWINDOWPOS pfnImQueryConversionWindowPos = NULL;

static PFN_IMSETCONVERSIONWINDOWPOS pfnImSetConversionWindowPos = NULL;

static PFN_IMGETRESULTSTRING pfnImGetResultString = NULL;

static PFN_IMQUERYINFOMSG pfnImQueryInfoMsg = NULL;

static PFN_IMQUERYSTATUSWINDOWPOS pfnImQueryStatusWindowPos = NULL;

static PFN_IMSETSTATUSWINDOWPOS pfnImSetStatusWindowPos = NULL;

static PFN_IMSHOWSTATUSWINDOW pfnImShowStatusWindow = NULL;

static PFN_IMGETSTATUSSTRING pfnImGetStatusString = NULL;

static PFN_IMCONVERTSTRING pfnImConvertString = NULL;

static PFN_IMQUERYIMMODE pfnImQueryIMMode = NULL;

static PFN_IMSETIMMODE pfnImSetIMMode = NULL;

static PFN_IMQUERYIMEINFO pfnImQueryIMEInfo = NULL;

static PFN_IMQUERYIMEPROPERTY pfnImQueryIMEProperty = NULL;

static PFN_IMREQUESTIME pfnImRequestIME = NULL;

static PFN_IMISIMEMESSAGE pfnImIsIMEMessage = NULL;

static PFN_IMQUERYIMELIST pfnImQueryIMEList = NULL;

static PFN_IMQUERYCURRENTIME pfnImQueryCurrentIME = NULL;

static PFN_IMSETCURRENTIME pfnImSetCurrentIME = NULL;

static PFN_IMQUERYDEFAULTIME pfnImQueryDefaultIME = NULL;

static PFN_IMSETDEFAULTIME pfnImSetDefaultIME = NULL;

static PFN_IMSETIMELISTORDER pfnImSetIMEListOrder = NULL;

static PFN_IMQUERYIMEID pfnImQueryIMEID = NULL;

static PFN_IMQUERYMSGQUEUEPROPERTY pfnImQueryMsgQueueProperty = NULL;

static PFN_IMSETMSGQUEUEPROPERTY pfnImSetMsgQueueProperty = NULL;

// Input Method Interfaces for IME

static PFN_IMREQUESTEVENT pfnImRequestEvent = NULL;

static PFN_IMREQUESTIMINSTANCE pfnImRequestIMInstance = NULL;

static PFN_IMRELEASEIMINSTANCE pfnImReleaseIMInstance = NULL;

static PFN_IMQUERYIMINSTANCEREQCOUNT pfnImQueryIMInstanceReqCount = NULL;

static PFN_IMCREATEIMIPART pfnImCreateIMIPart = NULL;

static PFN_IMDESTROYIMIPART pfnImDestroyIMIPart = NULL;

static PFN_IMQUERYIMIPARTSIZE pfnImQueryIMIPartSize = NULL;

static PFN_IMREQUESTIMIPART pfnImRequestIMIPart = NULL;

static PFN_IMRELEASEIMIPART pfnImReleaseIMIPart = NULL;

static PFN_IMRESIZEIMIPART pfnImResizeIMIPart = NULL;

static PFN_IMQUERYIMIPARTREQCOUNT pfnImQueryIMIPartReqCount = NULL;

static PFN_IMBROADCASTDATA pfnImBroadcastData = NULL;

static PFN_IMGETINSTANCEMQ pfnImGetInstanceMQ = NULL;

static PFN_IMRELEASEINSTANCEMQ pfnImReleaseInstanceMQ = NULL;

#define DOSQUERYPROCADDR( ordinal, ppfn ) \
    DosQueryProcAddr( hIM32Mod, ordinal, NULL, ( PFN * )( ppfn ))

BOOL IM32Init( VOID )
{
    UCHAR szErrorName[ 256 ];

    if( fIM32Inited )
        return TRUE;

    if( !IsDBCSEnv() )
        return FALSE;

    hIM32Mod = NULLHANDLE;

    if( DosLoadModule(( PSZ )szErrorName, sizeof( szErrorName ), "OS2IM.DLL", &hIM32Mod ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMASSOCIATEINSTANCE, &pfnImAssociateInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMCREATEINSTANCE, &pfnImCreateInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMDESTROYINSTANCE, &pfnImDestroyInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMGETINSTANCE, &pfnImGetInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYDEFAULTIMWINDOW, &pfnImQueryDefaultIMWindow ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMRELEASEINSTANCE, &pfnImReleaseInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMENUMREGISTERWORD, &pfnImEnumRegisterWord ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYREGISTERWORDTYPE, &pfnImQueryRegisterWordType ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMREGISTERWORD, &pfnImRegisterWord ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMDEREGISTERWORD, &pfnImDeregisterWord ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSHOWIMEDLG, &pfnImShowIMEDlg ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMESCAPE, &pfnImEscape ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCANDIDATELIST, &pfnImQueryCandidateList ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCANDIDATEWINDOWPOS, &pfnImQueryCandidateWindowPos ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCANDIDATEWINDOWPOS, &pfnImSetCandidateWindowPos ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCONVERSIONFONT, &pfnImQueryConversionFont ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCONVERSIONFONT, &pfnImSetConversionFont ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMGETCONVERSIONSTRING, &pfnImGetConversionString ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCONVERSIONSTRING, &pfnImSetConversionString ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCONVERSIONWINDOWPOS, &pfnImQueryConversionWindowPos ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCONVERSIONWINDOWPOS, &pfnImSetConversionWindowPos ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMGETRESULTSTRING, &pfnImGetResultString ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCONVERSIONFONTSIZE, &pfnImQueryConversionFontSize ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCONVERSIONFONTSIZE, &pfnImSetConversionFontSize ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCONVERSIONANGLE, &pfnImQueryConversionAngle ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCONVERSIONANGLE, &pfnImSetConversionAngle ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYINFOMSG, &pfnImQueryInfoMsg ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYSTATUSWINDOWPOS, &pfnImQueryStatusWindowPos ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETSTATUSWINDOWPOS, &pfnImSetStatusWindowPos ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSHOWSTATUSWINDOW, &pfnImShowStatusWindow ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMGETSTATUSSTRING, &pfnImGetStatusString ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMCONVERTSTRING, &pfnImConvertString ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMMODE, &pfnImQueryIMMode ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETIMMODE, &pfnImSetIMMode ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMEINFO, &pfnImQueryIMEInfo ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMEPROPERTY, &pfnImQueryIMEProperty ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMREQUESTIME, &pfnImRequestIME ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMISIMEMESSAGE, &pfnImIsIMEMessage ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMELIST, &pfnImQueryIMEList ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYCURRENTIME, &pfnImQueryCurrentIME ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETCURRENTIME, &pfnImSetCurrentIME ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYDEFAULTIME, &pfnImQueryDefaultIME ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETDEFAULTIME, &pfnImSetDefaultIME ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETIMELISTORDER, &pfnImSetIMEListOrder ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMEID, &pfnImQueryIMEID ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYMSGQUEUEPROPERTY, &pfnImQueryMsgQueueProperty ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMSETMSGQUEUEPROPERTY, &pfnImSetMsgQueueProperty ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMREQUESTEVENT, &pfnImRequestEvent ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMREQUESTIMINSTANCE, &pfnImRequestIMInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMRELEASEIMINSTANCE, &pfnImReleaseIMInstance ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMINSTANCEREQCOUNT, &pfnImQueryIMInstanceReqCount ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMCREATEIMIPART, &pfnImCreateIMIPart ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMDESTROYIMIPART, &pfnImDestroyIMIPart ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMIPARTSIZE, &pfnImQueryIMIPartSize ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMREQUESTIMIPART, &pfnImRequestIMIPart ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMRELEASEIMIPART, &pfnImReleaseIMIPart ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMRESIZEIMIPART, &pfnImResizeIMIPart ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMQUERYIMIPARTREQCOUNT, &pfnImQueryIMIPartReqCount ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMBROADCASTDATA, &pfnImBroadcastData ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMGETINSTANCEMQ, &pfnImGetInstanceMQ ))
        goto error_exit;

    if( DOSQUERYPROCADDR( ORD_IMRELEASEINSTANCEMQ, &pfnImReleaseInstanceMQ ))
        goto error_exit;

    fIM32Inited = TRUE;

error_exit:

    if( hIM32Mod && !fIM32Inited )
        DosFreeModule( hIM32Mod );

    return fIM32Inited;
}

VOID IM32Term( VOID )
{
    if( !fIM32Inited )
        return;

    DosFreeModule( hIM32Mod );

    fIM32Inited = FALSE;
}

APIRET APIENTRY ImAssociateInstance( HWND hwnd, HIMI himi, PHIMI phimiPrev )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImAssociateInstance( hwnd, himi, phimiPrev );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImCreateInstance( PHIMI phimi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImCreateInstance( phimi );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImDestroyInstance( HIMI himi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImDestroyInstance( himi );
    SetFS( sel );

    return rc;
}


APIRET APIENTRY ImGetInstance( HWND hwnd, PHIMI phimi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImGetInstance( hwnd, phimi );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryDefaultIMWindow( HWND hwnd, PHWND phwnd )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryDefaultIMWindow( hwnd, phwnd );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImReleaseInstance( HWND hwnd, HIMI himi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImReleaseInstance( hwnd, himi );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImEnumRegisterWord( HIMI himi,
                                    REGISTERWORDENUMPROC pfnEnumProc,
                                    PSZ pReading,
                                    ULONG ulType,
                                    PSZ pRegister,
                                    PVOID pData )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImEnumRegisterWord( himi, pfnEnumProc, pReading, ulType, pRegister, pData );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryRegisterWordType( HIMI himi, PULONG pulCount,
                                         PWORDTYPE pWordType )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryRegisterWordType( himi, pulCount, pWordType );
    SetFS( sel );

    return rc;
}


APIRET APIENTRY ImRegisterWord( HIMI himi,
                                PSZ pszReading,
                                ULONG ulType,
                                PSZ pszRegister )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImRegisterWord( himi, pszReading, ulType, pszRegister );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImDeregisterWord( HIMI himi,
                                  PSZ pszReading,
                                  ULONG ulType,
                                  PSZ pszDeregister )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImDeregisterWord( himi, pszReading, ulType, pszDeregister );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImShowIMEDlg( HIMI himi,
                             ULONG ulDlgType,
                             PREGISTERWORDHEADER pRegWord )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImShowIMEDlg( himi, ulDlgType, pRegWord );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImEscape( HIMI himi, ULONG ulEscape, PVOID pData )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImEscape( himi, ulEscape, pData );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryCandidateList( HIMI himi,
                                      ULONG ulIndex,
                                      PCANDIDATELISTHEADER pCandidateList,
                                      PULONG pulBuffer )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryCandidateList( himi, ulIndex, pCandidateList, pulBuffer );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryCandidateWindowPos( HIMI himi,
                                           PCANDIDATEPOS pCandidatePos )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryCandidateWindowPos( himi, pCandidatePos );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetCandidateWindowPos( HIMI himi,
                                         PCANDIDATEPOS pCandidatePos )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetCandidateWindowPos( himi, pCandidatePos );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryConversionFont( HIMI himi,
                                       PFATTRS pFontAttrs )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryConversionFont( himi, pFontAttrs );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetConversionFont( HIMI himi,
                                     PFATTRS pFontAttrs )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetConversionFont( himi, pFontAttrs );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryConversionFontSize( HIMI himi,
                                           PSIZEF psizfxBox )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryConversionFontSize( himi, psizfxBox );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetConversionFontSize( HIMI himi,
                                         PSIZEF psizfxBox )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetConversionFontSize( himi, psizfxBox );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImGetConversionString( HIMI himi, ULONG ulIndex,
                                       PVOID pBuf, PULONG pulBufLen )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImGetConversionString( himi, ulIndex, pBuf, pulBufLen );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetConversionString( HIMI himi, ULONG ulIndex,
                                         PVOID pConv, ULONG ulConvLen,
                                         PVOID pReading, ULONG ulReadingLen )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetConversionString( himi, ulIndex, pConv, ulConvLen, pReading, ulReadingLen );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryConversionAngle( HIMI himi, PGRADIENTL pgradlAngle )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryConversionAngle( himi, pgradlAngle );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetConversionAngle( HIMI himi, PGRADIENTL pgradlAngle )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetConversionAngle( himi, pgradlAngle );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryConversionWindowPos( HIMI himi,
                                            PCONVERSIONPOS pConvPos )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryConversionWindowPos( himi, pConvPos );
    SetFS( sel );

    return rc;
}


APIRET APIENTRY ImSetConversionWindowPos( HIMI himi,
                                          PCONVERSIONPOS pConvPos )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetConversionWindowPos( himi, pConvPos );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImGetResultString( HIMI himi, ULONG ulIndex,
                                   PVOID pBuf, PULONG pulBufLen )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImGetResultString( himi, ulIndex, pBuf, pulBufLen );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryInfoMsg( HIMI himi, ULONG ulIndex,
                                     PVOID pBuf, PULONG pulBufLen )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryInfoMsg( himi, ulIndex, pBuf, pulBufLen );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryStatusWindowPos( HIMI himi, PPOINTL pptPos, PSIZEL pszlSize )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryStatusWindowPos( himi, pptPos, pszlSize );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetStatusWindowPos( HIMI himi, PPOINTL pptPos )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetStatusWindowPos( himi, pptPos );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImShowStatusWindow( HIMI himi, ULONG fShow )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImShowStatusWindow( himi, fShow );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImGetStatusString( HIMI himi, ULONG ulIndex,
                                   PVOID pBuf, PULONG pulBufLen )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImGetStatusString( himi, ulIndex, pBuf, pulBufLen );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImConvertString( HIMI himi,
                                 PSZ pSrc,
                                 PCANDIDATELISTHEADER pDst,
                                 PULONG pulBufLen,
                                 ULONG ulFlag )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImConvertString( himi, pSrc, pDst, pulBufLen, ulFlag );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMMode( HIMI himi,
                               PULONG pulInputMode,
                               PULONG pulConversionMode )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMMode( himi, pulInputMode, pulConversionMode );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetIMMode( HIMI himi,
                             ULONG ulInputMode,
                             ULONG ulConversionMode )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetIMMode( himi, ulInputMode, ulConversionMode );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMEInfo( ULONG ImeId, PIMEINFOHEADER pImeInfoHeader,
                                PULONG pulBufLen )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMEInfo( ImeId, pImeInfoHeader, pulBufLen );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMEProperty( HIMI himi, ULONG ulIndex, PULONG pulProp )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMEProperty( himi, ulIndex, pulProp );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImRequestIME( HIMI himi, ULONG ulAction,
                              ULONG ulIndex, ULONG ulValue )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImRequestIME( himi, ulAction, ulIndex, ulValue );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImIsIMEMessage( HWND hwndIm,
                                ULONG msg,
                                MPARAM mp1,
                                MPARAM mp2,
                                PBOOL pfResult )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImIsIMEMessage( hwndIm, msg, mp1, mp2, pfResult );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMEList( ULONG ulCodepage,
                                PULONG aImeId,
                                PULONG pulCount )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMEList( ulCodepage, aImeId, pulCount );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryCurrentIME( HIMI himi, PULONG pImeId )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryCurrentIME( himi, pImeId );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetCurrentIME( HIMI himi,
                                 ULONG ulAction,
                                 ULONG ImeId )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetCurrentIME( himi, ulAction, ImeId );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryDefaultIME( ULONG ulCodepage, PULONG pImeId )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryDefaultIME( ulCodepage, pImeId );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetDefaultIME( ULONG ulCodepage, ULONG ImeId )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetDefaultIME( ulCodepage, ImeId );
    SetFS( sel );

    return rc;
}


APIRET APIENTRY ImSetIMEListOrder( ULONG ulCodepage,
                                   ULONG ulAction,
                                   ULONG ImeIdTarget,
                                   ULONG ImeIdIndex )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetIMEListOrder( ulCodepage, ulAction, ImeIdTarget, ImeIdIndex );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMEID( PSZ pszIMEName, PULONG pImeId )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMEID( pszIMEName, pImeId );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryMsgQueueProperty( HMQ hmq, PULONG pulFlag )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryMsgQueueProperty( hmq, pulFlag );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImSetMsgQueueProperty( HMQ hmq, ULONG ulFlag )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImSetMsgQueueProperty( hmq, ulFlag );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImRequestEvent( HIMI hImi,
                                ULONG msg,
                                ULONG mp1,
                                ULONG mp2 )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImRequestEvent( hImi, msg, mp1, mp2 );
    SetFS( sel );

    return rc;
}


APIRET APIENTRY ImRequestIMInstance( HIMI hImi,
                                     PIMINSTANCE * ppIMInstance )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImRequestIMInstance( hImi, ppIMInstance );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImReleaseIMInstance( HIMI hImi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImReleaseIMInstance( hImi );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMInstanceReqCount( HIMI hImi,
                                           PULONG pulCount )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMInstanceReqCount( hImi, pulCount );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImCreateIMIPart( ULONG ulPartSize,
                                 PHIMIP phimiPart )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImCreateIMIPart( ulPartSize, phimiPart );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImDestroyIMIPart( HIMIP himiPart )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImDestroyIMIPart( himiPart );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMIPartSize( HIMIP himiPart,
                                    PULONG pulPartSize )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMIPartSize( himiPart, pulPartSize );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImRequestIMIPart( HIMIP himiPart,
                                  PVOID * ppPartData )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImRequestIMIPart( himiPart, ppPartData );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImReleaseIMIPart( HIMIP himiPart )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImReleaseIMIPart( himiPart );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImResizeIMIPart( HIMIP himiPart,
                                 ULONG ulNewSize )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImResizeIMIPart( himiPart, ulNewSize );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImQueryIMIPartReqCount( HIMIP himiPart,
                                        PULONG pulCount )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImQueryIMIPartReqCount( himiPart, pulCount );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImBroadcastData( PSZ pszIMEName,
                                 ULONG ulIndex,
                                 PBROADCASTDATAHEADER pData )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImBroadcastData( pszIMEName, ulIndex, pData );
    SetFS( sel );

    return rc;
}


APIRET APIENTRY ImGetInstanceMQ( HMQ hmq, PHIMI phimi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImGetInstanceMQ( hmq, phimi );
    SetFS( sel );

    return rc;
}

APIRET APIENTRY ImReleaseInstanceMQ( HMQ hmq, HIMI himi )
{
    USHORT sel;
    APIRET rc;

    if( !fIM32Inited )
        return 1;

    sel = RestoreOS2FS();
    rc = pfnImReleaseInstanceMQ( hmq, himi );
    SetFS( sel );

    return rc;
}

HIMC    IM32AssociateContext(HWND hWnd, HIMC hIMC)
{
    HWND hwndOS2 = Win32ToOS2Handle( hWnd );
    HIMI himiPrev;

    if( ImAssociateInstance( hwndOS2, ( HIMI )hIMC, &himiPrev ))
        return NULL;

    return ( HIMC )himiPrev;
}

BOOL    IM32AssociateContextEx(HWND hWnd, HIMC hIMC, DWORD dword)
{
    return FALSE;
}

BOOL    IM32ConfigureIME(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData, BOOL fUnicode )
{
// TODO

    return FALSE;
}

HIMC    IM32CreateContext(VOID)
{
    HIMI    himi;

    if( ImCreateInstance( &himi ) != 0 )
        return NULL;

    return ( HIMC )himi;
}

HIMCC   IM32CreateIMCC(DWORD dword)
{
    return NULL;
}

HWND    IM32CreateSoftKeyboard(UINT uint, HWND hwnd, int in1, int in2)
{
    return NULL;
}

BOOL    IM32DestroyContext(HIMC hIMC)
{
    if( ImDestroyInstance(( HIMI )hIMC ) != 0 )
        return FALSE;

    return TRUE;
}

HIMCC   IM32DestroyIMCC(HIMCC himcc)
{
    return NULL;
}

BOOL    IM32DestroySoftKeyboard(HWND hwnd)
{
    return FALSE;
}

BOOL    IM32DisableIME(DWORD dword)
{
    return FALSE;
}

UINT    IM32EnumRegisterWord( HKL hKL, LPVOID lpfnEnumProc, LPVOID lpszReading, DWORD dwStyle,  LPVOID lpszRegister, LPVOID lpData, BOOL fUnicode )
{
// TODO
    return 0;
}

LRESULT IM32Escape(HKL hKL, HIMC hIMC,   UINT uEscape, LPVOID lpData, BOOL fUnicode)
{
// TODO
    return NULL;
}

BOOL    IM32GenerateMessage(HIMC himc)
{
    return FALSE;
}

DWORD   IM32GetCandidateList(HIMC hIMC, DWORD dwIndex, LPCANDIDATELIST lpCandList, DWORD dwBufLen, BOOL fUnicode)
{
// TODO

    return 0;
}

DWORD   IM32GetCandidateListCount( HIMC hIMC, LPDWORD lpdwListCount, BOOL fUnicode)
{
// TODO

    return 0;
}

BOOL    IM32GetCandidateWindow(HIMC hIMC, DWORD dwBufLen, LPCANDIDATEFORM lpCandidate)
{
// TODO

    return FALSE;
}

BOOL    IM32GetCompositionFont(HIMC hIMC, LPVOID lplf, BOOL fUnicode)
{
// TODO

    return FALSE;
}

typedef APIRET ( APIENTRY *PFN_GETCOMPOSITIONSTRING )( HIMI, ULONG, PVOID, PULONG );

LONG    IM32GetCompositionString(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen, BOOL fUnicode)
{
    PFN_GETCOMPOSITIONSTRING pfnGetCompositionString = ImGetConversionString;
    ULONG   ulIndex;
    PCHAR   pBuffer = NULL;
    ULONG   ulBufLen;
    PSZ     pszStr = NULL;
    ULONG   ulStrLen;
    APIRET  rc;

    switch( dwIndex )
    {
        case GCS_COMPSTR_W :
            ulIndex = IMR_CONV_CONVERSIONSTRING;
            break;

        case GCS_COMPATTR_W :
            ulIndex = IMR_CONV_CONVERSIONATTR;
            break;

        case GCS_COMPCLAUSE_W :
            ulIndex = IMR_CONV_CONVERSIONCLAUSE;
            break;

        case GCS_COMPREADSTR_W :
            ulIndex = IMR_CONV_READINGSTRING;
            break;

        case GCS_COMPREADATTR_W :
            ulIndex = IMR_CONV_READINGATTR;
            break;

        case GCS_COMPREADCLAUSE_W :
            ulIndex = IMR_CONV_READINGCLAUSE;
            break;

        case GCS_CURSORPOS_W :
            ulIndex = IMR_CONV_CURSORPOS;
            break;

        case GCS_DELTASTART_W :
            ulIndex = IMR_CONV_CHANGESTART;
            break;

        case GCS_RESULTSTR_W :
            ulIndex = IMR_RESULT_RESULTSTRING;
            pfnGetCompositionString = ImGetResultString;
            break;

        case GCS_RESULTCLAUSE_W :
            ulIndex = IMR_RESULT_RESULTCLAUSE;
            pfnGetCompositionString = ImGetResultString;
            break;

        case GCS_RESULTREADSTR_W :
            ulIndex = IMR_RESULT_READINGSTRING;
            pfnGetCompositionString = ImGetResultString;
            break;

        case GCS_RESULTREADCLAUSE_W :
            ulIndex = IMR_RESULT_READINGCLAUSE;
            pfnGetCompositionString = ImGetResultString;
            break;

        default :
            goto error_exit;
    }

    if( fUnicode )
    {
        ulBufLen = 0;
        rc = pfnGetCompositionString( hIMC, ulIndex, pBuffer, &ulBufLen );
        if( rc != 0 )
            goto error_exit;

        pBuffer = ( PCHAR )malloc( ulBufLen + 1 );
        if( !pBuffer )
            goto error_exit;

        rc = pfnGetCompositionString( hIMC, ulIndex, pBuffer, &ulBufLen );
        if( rc != 0 )
            goto error_exit;

        switch( ulIndex )
        {
            case IMR_CONV_CONVERSIONSTRING :
            case IMR_CONV_READINGSTRING :
            case IMR_RESULT_RESULTSTRING :
            case IMR_RESULT_READINGSTRING :
                /* dwBufLen is always in bytes */
                if( ulBufLen > 0 )
                {
                    ulBufLen = MultiByteToWideChar( CP_ACP, 0, pBuffer, ulBufLen, ( LPWSTR )lpBuf, dwBufLen / sizeof( WCHAR ));
                    if( ulBufLen == 0 )
                        goto error_exit;
                }

                ulBufLen *= sizeof( WCHAR );
                break;

            default :
/*
            case IMR_CONV_CONVERSIONATTR :
            case IMR_CONV_READINGATTR :
            case IMR_CONV_CONVERSIONCLAUSE :
            case IMR_CONV_READINGCLAUSE :
            case IMR_RESULT_RESULTCLAUSE :
            case IMR_RESULT_READINGCLAUSE :
            case IMR_CONV_CURSORPOS :
            case IMR_CONV_CHANGESTART :
*/
            {
                ULONG ulStrIndex;
                INT   i, j;

                switch( ulIndex )
                {
                    case IMR_CONV_CONVERSIONATTR :
                    case IMR_CONV_CONVERSIONCLAUSE :
                    case IMR_CONV_CURSORPOS :
                    case IMR_CONV_CHANGESTART :
                        ulStrIndex = IMR_CONV_CONVERSIONSTRING;
                        break;

                    case IMR_CONV_READINGATTR :
                    case IMR_CONV_READINGCLAUSE :
                        ulStrIndex = IMR_CONV_READINGSTRING;
                        break;

                    case IMR_RESULT_RESULTCLAUSE :
                        ulStrIndex = IMR_RESULT_RESULTSTRING;
                        break;

                    case IMR_RESULT_READINGCLAUSE :
                        ulStrIndex = IMR_RESULT_READINGSTRING;
                }

                ulStrLen = 0;
                rc = pfnGetCompositionString( hIMC, ulStrIndex, pszStr, &ulStrLen );
                if( rc != 0 )
                    goto error_exit;

                pszStr = ( PSZ )malloc( ulStrLen + 1 );
                if( !pszStr )
                    goto error_exit;

                rc = pfnGetCompositionString( hIMC, ulStrIndex, pszStr, &ulStrLen );
                if( rc != 0 )
                    goto error_exit;

                switch( ulIndex )
                {
                    case IMR_CONV_CONVERSIONATTR :
                    case IMR_CONV_READINGATTR :
                    {
                        PCHAR pch = ( PCHAR )lpBuf;

                        for( i = j = 0; ( i < ulBufLen ) && (( dwBufLen == 0 ) || ( j < dwBufLen )); i++, j++ )
                        {
                            if( dwBufLen )
                                pch[ j ] = pBuffer[ i ];

                            if( IsDBCSLeadByte( pszStr[ i ] ))
                                i++;
                        }

                        ulBufLen = j;
                        break;
                    }

                    case IMR_CONV_CONVERSIONCLAUSE :
                    case IMR_CONV_READINGCLAUSE :
                    case IMR_RESULT_RESULTCLAUSE :
                    case IMR_RESULT_READINGCLAUSE :
                    {
                        PULONG pulBufSrc = ( PULONG )pBuffer;
                        PULONG pulBufDest = ( PULONG )lpBuf;

                        /* if dwBufLen == 0, then current ulBufLen has what we want */
                        if( dwBufLen != 0 )
                        {
                            if( dwBufLen >= sizeof( ULONG ))
                            {
                                pulBufDest[ 0 ] = 0;

                                for( i = 1; ( i * sizeof( ULONG ) < ulBufLen ) &&
                                            ( i * sizeof( ULONG ) < dwBufLen ); i++ )
                                {
                                    pulBufDest[ i ] = MultiByteToWideChar( CP_ACP, 0, pszStr, pulBufSrc[ i ], 0, 0 );
                                    if( !pulBufDest[ i ])
                                        goto error_exit;
                                }

                                ulBufLen = i * sizeof( ULONG );
                            }
                            else
                                ulBufLen = 0;
                        }
                        break;
                    }

                    case IMR_CONV_CURSORPOS :
                    case IMR_CONV_CHANGESTART :
                    {
                        ULONG ul = *( PULONG )pBuffer;

                        ulBufLen = MultiByteToWideChar( CP_ACP, 0, pszStr, ul, 0, 0 );
                        break;
                    }
                }

                free( pszStr );
                break;
            }
        }

        free( pBuffer );
    }
    else
    {
        if(( ulIndex == IMR_CONV_CURSORPOS ) || ( ulIndex == IMR_CONV_CHANGESTART ))
        {
            dwBufLen = sizeof( ULONG );
            rc = pfnGetCompositionString( hIMC, ulIndex, &ulBufLen, &dwBufLen );
        }
        else
        {
            ulBufLen = dwBufLen;
            rc = pfnGetCompositionString( hIMC, ulIndex, lpBuf, &ulBufLen );
        }

        if( rc != 0 )
            return IMM_ERROR_GENERAL_W;
    }

    return ulBufLen;

error_exit :
    if( pBuffer )
        free( pBuffer );

    if( pszStr )
        free( pszStr );

    return IMM_ERROR_GENERAL_W;
}

BOOL    IM32GetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
    PIMINSTANCE     pimi;
    RECTL           rcl;

    if( ImRequestIMInstance( hIMC, &pimi ) != 0 )
        return FALSE;

    if(!( pimi->ulClientInfo & IMI_CI_PMCALLABLE ))
    {
        ImReleaseIMInstance( hIMC );
        return FALSE;
    }

    lpCompForm->dwStyle = CFS_DEFAULT_W;

    if( pimi->cpConversionPos.ulStyle & CPS_FORCE )
        lpCompForm->dwStyle |= CFS_FORCE_POSITION_W;

    if( pimi->cpConversionPos.ulStyle & CPS_POINT )
        lpCompForm->dwStyle |= CFS_POINT_W;

    if( pimi->cpConversionPos.ulStyle & CPS_RECT )
        lpCompForm->dwStyle |= CFS_RECT_W;

    WinQueryWindowRect( pimi->hwnd, &rcl );

    lpCompForm->ptCurrentPos.x = pimi->cpConversionPos.ptCurrentPos.x;
    lpCompForm->ptCurrentPos.y = rcl.yTop -
        ( pimi->cpConversionPos.ptCurrentPos.y + pimi->faConversionWindow.lMaxBaselineExt );

    lpCompForm->rcArea.left = pimi->cpConversionPos.rcArea.xLeft;
    lpCompForm->rcArea.right = pimi->cpConversionPos.rcArea.xRight;
    lpCompForm->rcArea.top = rcl.yTop - pimi->cpConversionPos.rcArea.yTop;
    lpCompForm->rcArea.bottom = rcl.yTop - pimi->cpConversionPos.rcArea.yBottom;

    ImReleaseIMInstance( hIMC );

    return TRUE;
}

HIMC    IM32GetContext(HWND hWnd)
{
    HWND hwndOS2 = Win32ToOS2Handle( hWnd );
    HIMI himi;

    if( ImGetInstance( hwndOS2, &himi ) != 0 )
        return NULL;

    return himi;
}

DWORD   IM32GetConversionList( HKL hKL, HIMC hIMC, LPVOID pSrc, LPCANDIDATELIST lpDst,  DWORD dwBufLen, UINT uFlag, BOOL fUnicode)
{
// TODO

    return 0;
}

BOOL    IM32GetConversionStatus(HIMC hIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence)
{
    ULONG ulInputMode;
    ULONG ulConversionMode;

    if( ImQueryIMMode( hIMC, &ulInputMode, &ulConversionMode ) != 0 )
        return FALSE;

    *lpfdwConversion = IME_CMODE_ALPHANUMERIC_W;

    if( ulInputMode & IMI_IM_NLS_HANGEUL )
        *lpfdwConversion |= IME_CMODE_NATIVE_W;

    if(( ulInputMode & IMI_IM_NLS_KATAKANA ) == IMI_IM_NLS_KATAKANA )
        *lpfdwConversion |= IME_CMODE_KATAKANA_W;

    if( ulInputMode & IMI_IM_WIDTH_FULL )
        *lpfdwConversion |= IME_CMODE_FULLSHAPE_W;

    if( ulInputMode & IMI_IM_ROMAJI_ON )
        *lpfdwConversion |= IME_CMODE_ROMAN_W;

    if( ulInputMode & IMI_IM_SYSTEMROMAJI_DISABLE )
        *lpfdwConversion |= 0;

    if( ulInputMode & IMI_IM_IME_ON )
        *lpfdwConversion |= IME_CMODE_HANJACONVERT_W; // right ?

    if( ulInputMode & IMI_IM_IME_DISABLE )
        *lpfdwConversion |= IME_CMODE_NOCONVERSION_W; // right ?

    if( ulInputMode & IMI_IM_IME_SOFTKBD_ON )
        *lpfdwConversion |= IME_CMODE_SOFTKBD_W;

    if( ulInputMode & IMI_IM_IME_CSYMBOL_ON )
        *lpfdwConversion |= IME_CMODE_SYMBOL_W; // right ?

    if( ulInputMode & IMI_IM_2NDCONV )
        *lpfdwConversion |= 0;

    *lpfdwSentence = IME_SMODE_NONE_W;

    if( ulConversionMode & IMI_CM_PLURALCLAUSE )
        *lpfdwSentence |= IME_SMODE_PLAURALCLAUSE_W;

    if( ulConversionMode & IMI_CM_SINGLE )
        *lpfdwSentence |= IME_SMODE_SINGLECONVERT_W;

    if( ulConversionMode & IMI_CM_AUTOMATIC )
        *lpfdwSentence |= IME_SMODE_AUTOMATIC_W;

    if( ulConversionMode & IMI_CM_PREDICT )
        *lpfdwSentence |= IME_SMODE_PHRASEPREDICT_W;

    return TRUE;
}

HWND    IM32GetDefaultIMEWnd(HWND hWnd)
{
    HWND hwndOS2 = Win32ToOS2Handle( hWnd );
    HWND hwndIm;
    HWND hwndWin32;

    if( ImQueryDefaultIMWindow( hwndOS2, &hwndIm ) != 0 )
        return NULL;

    hwndWin32 = OS2ToWin32Handle( hwndIm );

    return hwndWin32 ? hwndWin32 : hwndIm;
}

UINT    IM32GetDescription(HKL hKL, LPVOID lpszDescription, UINT uBufLen, BOOL fUnicode)
{
    return 0;
}

DWORD   IM32GetGuideLine(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen, BOOL fUnicode)
{
// TODO

    return 0;
}

BOOL    IM32GetHotKey(DWORD dword, LPUINT lpuModifiers, LPUINT lpuVKey, LPHKL lphkl)
{
    return FALSE;
}

DWORD   IM32GetIMCCLockCount(HIMCC himcc)
{
    return 0;
}

DWORD   IM32GetIMCCSize(HIMCC himcc)
{
    return 0;
}

DWORD   IM32GetIMCLockCount(HIMC himc)
{
    return 0;
}

UINT    IM32GetIMEFileName( HKL hKL, LPVOID lpszFileName, UINT uBufLen, BOOL fUnicode)
{
// TODO

    return 0;
}

DWORD   IM32GetImeMenuItems(HIMC himc, DWORD dword, DWORD dword2, LPVOID pimen, LPVOID lpimen2, DWORD dword3, BOOL fUnicode)
{
    return 0;
}

BOOL    IM32GetOpenStatus(HIMC hIMC)
{
    ULONG ulInputMode;
    ULONG ulConversionMode;

    if( ImQueryIMMode(( HIMI )hIMC, &ulInputMode, &ulConversionMode ) != 0 )
        return FALSE;

    return !( ulInputMode & IMI_IM_IME_DISABLE );
}

// FIXME : hKL is ignored, instead, return the property of IM Instance attached to
//         current thread, maybe default locale.
DWORD   IM32GetProperty(HKL hKL, DWORD fdwIndex)
{
    HIMI    himi;
    ULONG   ulIndex;
    ULONG   ulProp;
    DWORD   result;


    switch( fdwIndex )
    {
        case IGP_GETIMEVERSION_W :
            return IMEVER_0400_W;

        case IGP_PROPERTY_W :
            ulIndex = QIP_PROPERTY;
            break;

        case IGP_CONVERSION_W :
            ulIndex = QIP_INPUTMODE;
            break;

        case IGP_SENTENCE_W :
            ulIndex = QIP_CONVERSIONMODE;
            break;

        case IGP_UI_W :
            ulIndex = QIP_UI;
            break;

        case IGP_SETCOMPSTR_W :
            ulIndex = QIP_SETCONVSTR;
            break;

        case IGP_SELECT_W :
            ulIndex = QIP_SELECT;
            break;
    }

    if( ImGetInstanceMQ( HMQ_CURRENT, &himi ) != 0 )
        return 0;

    if( ImQueryIMEProperty( himi, ulIndex, &ulProp ) != 0 )
        ulProp = 0;

    ImReleaseInstanceMQ( HMQ_CURRENT, himi );

    result = 0;
    switch( ulIndex )
    {
        case QIP_PROPERTY :
            if( ulProp & PRP_SPECIALUI )
                result |= IME_PROP_SPECIAL_UI_W;

            if( ulProp & PRP_UNICODE   )
                result |= IME_PROP_UNICODE_W;

            if( ulProp & PRP_FORCEPOSITION )
                result |= IME_PROP_AT_CARET_W;
            break;

        case QIP_INPUTMODE :
            if( ulProp & IMI_IM_NLS_HANGEUL )
                result |= IME_CMODE_NATIVE_W;

            if(( ulProp & IMI_IM_NLS_KATAKANA ) == IMI_IM_NLS_KATAKANA )
                result |= IME_CMODE_KATAKANA_W;

            if( ulProp & IMI_IM_WIDTH_FULL )
                result |= IME_CMODE_FULLSHAPE_W;

            if( ulProp & IMI_IM_ROMAJI_ON )
                result |= IME_CMODE_ROMAN_W;

            if( ulProp & IMI_IM_SYSTEMROMAJI_DISABLE )
                result |= 0;

            if( ulProp & IMI_IM_IME_ON )
                result |= IME_CMODE_HANJACONVERT_W; // right ?

            if( ulProp & IMI_IM_IME_DISABLE )
                result |= IME_CMODE_NOCONVERSION_W; // right ?

            if( ulProp & IMI_IM_IME_SOFTKBD_ON )
                result |= IME_CMODE_SOFTKBD_W;

            if( ulProp & IMI_IM_IME_CSYMBOL_ON )
                result |= IME_CMODE_SYMBOL_W; // right ?

            if( ulProp & IMI_IM_2NDCONV )
                result |= 0;
            break;

        case QIP_CONVERSIONMODE :
            if( ulProp & IMI_CM_PLURALCLAUSE )
                result |= IME_SMODE_PLAURALCLAUSE_W;

            if( ulProp & IMI_CM_SINGLE )
                result |= IME_SMODE_SINGLECONVERT_W;

            if( ulProp & IMI_CM_AUTOMATIC )
                result |= IME_SMODE_AUTOMATIC_W;

            if( ulProp & IMI_CM_PREDICT )
                result |= IME_SMODE_PHRASEPREDICT_W;
            break;

        case QIP_UI :
            if( ulProp & UIC_270 )
                result |= UI_CAP_2700_W;

            if( ulProp & UIC_ANGLE90 )
                result |= UI_CAP_ROT90_W;

            if( ulProp & UIC_ANGLEANY )
                result |= UI_CAP_ROTANY_W;
            break;

        case QIP_SETCONVSTR :
            if( ulProp & SCSC_CONVSTR )
                result |= SCS_CAP_COMPSTR_W;

            if( ulProp & SCSC_MAKEREAD )
                result |= SCS_CAP_MAKEREAD_W;
            break;

        case QIP_SELECT :
            if( ulProp & SLC_INPUTMODE )
                result |= SELECT_CAP_CONVERSION_W;

            if( ulProp & SLC_CONVERSIONMODE )
                result |= SELECT_CAP_SENTENCE_W;
            break;
    }

    return result;
}

UINT    IM32GetRegisterWordStyle(HKL hKL, UINT nItem, LPVOID lpStyleBuf, BOOL fUnicode)
{
// TODO

    return 0;
}

BOOL    IM32GetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
// TODO

    return FALSE;
}

UINT    IM32GetVirtualKey(HWND hWnd)
{
// TODO

    return 0;
}

HKL     IM32InstallIME(LPVOID lpszIMEFileName, LPVOID lpszLayoutText, BOOL fUnicode)
{
    return NULL;
}

BOOL    IM32IsIME(HKL hKL)
{
// TODO

    return FALSE;
}

BOOL    IM32IsUIMessage(HWND hWndIME, UINT msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode)
{
    HWND    hwndIm;
    ULONG   ulMsg;
    ULONG   mp1;
    ULONG   mp2 = 0;
    BOOL    rc;

    switch( msg )
    {
        case WM_IME_CHAR_W :
            // todo
            return FALSE;

        case WM_IME_COMPOSITION_W :
#if 0
            if( lParam & GCS_TYPINGINFO_W )
                // todo
                return FALSE;
#endif

            ulMsg = WM_IMEREQUEST;
            mp1 = IMR_CONVRESULT;

            if( lParam & GCS_COMPSTR_W )
                mp2 |= IMR_CONV_CONVERSIONSTRING;

            if( lParam & GCS_COMPATTR_W )
                mp2 |= IMR_CONV_CONVERSIONATTR;

            if( lParam & GCS_COMPCLAUSE_W )
                mp2 |= IMR_CONV_CONVERSIONCLAUSE;

            if( lParam & GCS_COMPREADSTR_W )
                mp2 |= IMR_CONV_READINGSTRING;

            if( lParam & GCS_COMPREADATTR_W )
                mp2 |= IMR_CONV_READINGATTR;

            if( lParam & GCS_COMPREADCLAUSE_W )
                mp2 |= IMR_CONV_READINGCLAUSE;

            if( lParam & GCS_CURSORPOS_W )
                mp2 |= IMR_CONV_CURSORPOS;

            if( lParam & GCS_DELTASTART_W )
                mp2 |= IMR_CONV_CHANGESTART;

            if( lParam & GCS_RESULTSTR_W )
                mp2 |= IMR_RESULT_RESULTSTRING | IMR_CONV_CURSORPOS;

            if( lParam & GCS_RESULTCLAUSE_W )
                mp2 |= IMR_RESULT_RESULTCLAUSE;

            if( lParam & GCS_RESULTREADSTR_W )
                mp2 |= IMR_RESULT_READINGSTRING;

            if( lParam & GCS_RESULTREADCLAUSE_W )
                mp2 |= IMR_RESULT_READINGCLAUSE;

            if( lParam & CS_INSERTCHAR_W )
                mp2 |= IMR_CONV_INSERTCHAR;

            if( lParam & CS_NOMOVECARET_W )
                mp2 |= IMR_CONV_NOMOVECARET;
            break;

        case WM_IME_COMPOSITIONFULL_W :
            ulMsg = WM_IMENOTIFY;
            mp1 = IMN_CONVERSIONFULL;
            break;

        case WM_IME_CONTROL_W :
            ulMsg = WM_IMECONTROL;

            switch( wParam )
            {
                case IMC_CLOSESTATUSWINDOW_W :
                    mp1 = IMC_HIDESTATUSWINDOW;
                    break;

                case IMC_GETCANDIDATEPOS_W :
                    mp1 = IMC_QUERYCANDIDATEPOS;
                    // todo
                    return FALSE;

                case IMC_GETCOMPOSITIONFONT_W :
                    mp1 = IMC_QUERYCONVERSIONFONT;
                    // TODO
                    return FALSE;

                case IMC_GETCOMPOSITIONWINDOW_W :
                    mp1 = IMC_QUERYCONVERSIONWINDOWPOS;
                    // todo
                    return FALSE;

                case IMC_GETSTATUSWINDOWPOS_W :
                    mp1 = IMC_QUERYSTATUSWINDOWPOS;
                    // todo
                    return FALSE;

                case IMC_OPENSTATUSWINDOW_W :
                    mp1 = IMC_SHOWSTATUSWINDOW;
                    break;

                case IMC_SETCANDIDATEPOS_W :
                    mp1 = IMC_SETCANDIDATEPOS;
                    // todo
                    return FALSE;

                case IMC_SETCOMPOSITIONFONT_W :
                    mp1 = IMC_SETCONVERSIONFONT;
                    // todo
                    return FALSE;

                case IMC_SETCOMPOSITIONWINDOW_W :
                    mp1 = IMC_SETCONVERSIONWINDOWPOS;
                    // todo
                    return FALSE;

                case IMC_SETSTATUSWINDOWPOS_W :
                    mp1 = IMC_SETSTATUSWINDOWPOS;
                    // todo
                    return FALSE;

                default :
                    return FALSE;
            }
            break;

        case WM_IME_ENDCOMPOSITION_W :
            ulMsg = WM_IMENOTIFY;
            mp1 = IMN_ENDCONVERSION;
            break;

        case WM_IME_KEYDOWN_W :
            // todo
            return FALSE;

        case WM_IME_KEYUP_W :
            // todo
            return FALSE;

        case WM_IME_NOTIFY_W :
            ulMsg = WM_IMEREQUEST;

            switch( wParam )
            {
                case IMN_CHANGECANDIDATE_W :
                    mp1 = IMR_CANDIDATE;
                    mp2 = IMR_CANDIDATE_CHANGE;
                    // todo
                    return FALSE;

                case IMN_CLOSECANDIDATE_W :
                    mp1 = IMR_CANDIDATE;
                    mp2 = IMR_CANDIDATE_HIDE;
                    // todo
                    return FALSE;

                case IMN_CLOSESTATUSWINDOW_W :
                    mp1 = IMR_STATUS;
                    mp2 = IMR_STATUS_HIDE;
                    break;

                case IMN_GUIDELINE_W :
                    mp1 = IMR_INFOMSG;
                    break;

                case IMN_OPENCANDIDATE_W :
                    mp1 = IMR_CANDIDATE;
                    mp2 = IMR_CANDIDATE_SHOW;
                    // todo
                    return FALSE;

                case IMN_OPENSTATUSWINDOW_W :
                    mp1 = IMR_STATUS;
                    mp2 = IMR_STATUS_SHOW;
                    break;

                case IMN_SETCANDIDATEPOS_W :
                    mp1 = IMR_CANDIDATE;
                    mp2 = IMR_CANDIDATE_CANDIDATEPOS;
                    // todo
                    return FALSE;

                case IMN_SETCOMPOSITIONFONT_W :
                    mp1 = IMR_CONVRESULT;
                    mp2 = IMR_CONV_CONVERSIONFONT;
                    break;

                case IMN_SETCOMPOSITIONWINDOW_W :
                    mp1 = IMR_CONVRESULT;
                    mp2 = IMR_CONV_CONVERSIONPOS;
                    break;

                case IMN_SETCONVERSIONMODE_W :
                    mp1 = IMR_STATUS;
                    mp2 = IMR_STATUS_INPUTMODE;
                    break;

                case IMN_SETOPENSTATUS_W :
                    mp1 = IMR_STATUS;
                    mp2 = IMR_STATUS_INPUTMODE;
                    break;

                case IMN_SETSENTENCEMODE_W :
                    mp1 = IMR_STATUS;
                    mp2 = IMR_STATUS_CONVERSIONMODE;
                    break;

                case IMN_SETSTATUSWINDOWPOS_W :
                    mp1 = IMR_STATUS;
                    mp2 = IMR_STATUS_STATUSPOS;
                    break;

                default :
                    return FALSE;
            }
            break;

#if 0   // todo
        case WM_IME_REQUEST_W :
            switch( wParam )
            {
                case IMR_CANDIDATEWINDOW_W :
                    return FALSE;

                case IMR_COMPOSITIONFONT_W :
                    return FALSE;

                case IMR_COMPOSITIONWINDOW_W :
                    return FALSE;

                case IMR_CONFIRMRECONVERTSTRING_W :
                    return FALSE;

                case IMR_DOCUMENTFEED_W :
                    return FALSE;

                case IMR_QUERYCHARPOSITION_W :
                    return FALSE;

                case IMR_RECONVERTSTRING_W :
                    return FALSE;

                default :
                    return FALSE;
            }
            break;
#endif

        case WM_IME_SELECT_W :
            ulMsg = WM_IMENOTIFY;
            mp1 = IMN_IMECHANGED;
            // todo
            return FALSE;

        case WM_IME_SETCONTEXT_W :
            ulMsg = WM_IMEREQUEST;
            mp1 = IMR_INSTANCEACTIVATE;
            mp2 = MAKEULONG( 0, wParam );

            if( lParam & ISC_SHOWUIALLCANDIDATEWINDOW_W )
                mp2 |= IMR_IA_CANDIDATE;

            if( lParam & ISC_SHOWUICOMPOSITIONWINDOW_W )
                mp2 |= IMR_IA_CONVERSION;

            if( lParam & ISC_SHOWUIGUIDELINE_W )
                mp2 |= IMR_IA_STATUS;

            // following bits are always cleared, so should be set
            lParam |= IMR_IA_INFOMSG;
            lParam |= IMR_IA_REGWORD;
            break;

        case WM_IME_STARTCOMPOSITION_W :
            ulMsg = WM_IMENOTIFY;
            mp1 = IMN_STARTCONVERSION;
            break;

        default :
            return FALSE;
    }

    if( hWndIME == NULL )
        return TRUE;

    hwndIm = Win32ToOS2Handle( hWndIME );
    if( ImIsIMEMessage( hwndIm, ulMsg, ( MPARAM )mp1, ( MPARAM )mp2, &rc ) != 0 )
        return FALSE;

    switch( msg )
    {
        case WM_IME_CONTROL_W :
            ulMsg = WM_IMECONTROL;

            switch( wParam )
            {
                case IMC_GETCANDIDATEPOS_W :
                    // todo
                    break;

                case IMC_GETCOMPOSITIONFONT_W :
                    // TODO
                    break;

                case IMC_GETCOMPOSITIONWINDOW_W :
                    // todo
                    break;

                case IMC_GETSTATUSWINDOWPOS_W :
                    // todo
                    break;

                case IMC_SETCANDIDATEPOS_W :
                    // todo
                    break;

                case IMC_SETCOMPOSITIONFONT_W :
                    // todo
                    break;

                case IMC_SETCOMPOSITIONWINDOW_W :
                    // todo
                    break;

                case IMC_SETSTATUSWINDOWPOS_W :
                    // todo
                    break;
            }
            break;
    }

    return rc;
}

LPINPUTCONTEXT IM32LockIMC(HIMC himc)
{
    return NULL;
}

LPVOID  IM32LockIMCC(HIMCC himcc)
{
    return NULL;
}

BOOL    IM32NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
// TODO

    return FALSE;
}

HIMCC   IM32ReSizeIMCC(HIMCC himcc, DWORD dword)
{
    return NULL;
}

BOOL    IM32RegisterWord( HKL hKL, LPVOID lpszReading, DWORD dwStyle, LPVOID lpszRegister, BOOL fUnicode)
{
// TODO

    return FALSE;
}

BOOL    IM32ReleaseContext(HWND hWnd, HIMC hIMC)
{
    HWND hwndOS2 = Win32ToOS2Handle( hWnd );

    if( ImReleaseInstance( hwndOS2, ( HIMI )hIMC ) != 0 )
        return FALSE;

    return TRUE;
}

LRESULT IM32RequestMessage(HIMC himc, WPARAM wparam, LPARAM lparam, BOOL fUnicode)
{
    return NULL;
}

BOOL    IM32SetCandidateWindow(HIMC hIMC, LPCANDIDATEFORM lpCandidate)
{
    CANDIDATEPOS    cp;
    PIMINSTANCE     pimi;
    RECTL           rcl;

    if( ImRequestIMInstance( hIMC, &pimi ) != 0 )
        return FALSE;

    if(!( pimi->ulClientInfo & IMI_CI_PMCALLABLE ))
    {
        ImReleaseIMInstance( hIMC );
        return FALSE;
    }

    WinQueryWindowRect( pimi->hwnd, &rcl );

    cp.ulIndex = lpCandidate->dwIndex;

    cp.ulStyle = 0;
    if( lpCandidate->dwStyle & CFS_CANDIDATEPOS_W )
        cp.ulStyle |= CPS_CANDIDATEPOS;

    if( lpCandidate->dwStyle & CFS_EXCLUDE_W )
        cp.ulStyle |= CPS_EXCLUDE;

    cp.ptCurrentPos.x = lpCandidate->ptCurrentPos.x;
    cp.ptCurrentPos.y = rcl.yTop - lpCandidate->ptCurrentPos.y;

    cp.rcArea.xLeft = lpCandidate->rcArea.left;
    cp.rcArea.xRight = lpCandidate->rcArea.right;
    cp.rcArea.yTop = rcl.yTop - lpCandidate->rcArea.top;
    cp.rcArea.yBottom = rcl.yTop - lpCandidate->rcArea.bottom;

    ImReleaseIMInstance( hIMC );

    if( ImSetCandidateWindowPos( hIMC, &cp ) != 0 )
        return FALSE;

    return TRUE;
}

BOOL    IM32SetCompositionFont(HIMC hIMC, LPVOID lplf, BOOL fUnicode)
{
// TODO

    return FALSE;
}

BOOL    IM32SetCompositionString(  HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen, BOOL fUnicode)
{
    ULONG ulIndex;

    switch( dwIndex )
    {
        case SCS_SETSTR_W :
            ulIndex = IME_SCS_STRING;
            break;

        case SCS_CHANGEATTR_W :
            ulIndex = IME_SCS_ATTRIBUTE;
            break;

        case SCS_CHANGECLAUSE_W :
            ulIndex = IME_SCS_CLAUSEINFO;
            break;

        default :
            return FALSE;

    }

    if( fUnicode )
    {
        LPSTR   compStrA = NULL;
        ULONG   compStrLenA = 0;
        LPSTR   readStrA = NULL;
        ULONG   readStrLenA = 0;
        BOOL    rc = FALSE;

        switch( ulIndex )
        {
            case IME_SCS_STRING :
            {
                if( lpComp )
                {
                    compStrLenA = WideCharToMultiByte( CP_ACP, 0, ( LPWSTR )lpComp, dwCompLen / sizeof( WCHAR ), 0, 0, 0, 0 );
                    if( compStrLenA != 0 )
                    {
                        compStrA = ( LPSTR )malloc( compStrLenA );
                        WideCharToMultiByte( CP_ACP, 0, ( LPWSTR )lpComp, dwCompLen / sizeof( WCHAR ), compStrA, compStrLenA, 0, 0 );
                    }
                }

                if( lpRead )
                {
                    readStrLenA = WideCharToMultiByte( CP_ACP, 0, ( LPWSTR )lpRead, dwReadLen / sizeof( WCHAR ), 0, 0, 0, 0 );
                    if( readStrLenA != 0 )
                    {
                        readStrA = ( LPSTR )malloc( readStrLenA );
                        WideCharToMultiByte( CP_ACP, 0, ( LPWSTR )lpRead, dwReadLen / sizeof( WCHAR ), readStrA, readStrLenA, 0, 0 );
                    }
                }

                rc = ( ImSetConversionString( hIMC, ulIndex, compStrA, compStrLenA, readStrA, readStrLenA ) == 0 );

                if( compStrA )
                    free( compStrA );

                if( readStrA )
                    free( readStrA );

                return rc;
            }

            case IME_SCS_ATTRIBUTE :
            {
                PCHAR compAttrA = NULL;
                PCHAR readAttrA = NULL;
                PCHAR pchAttr;
                INT   i, j;

                if( lpComp )
                {
                    if( ImGetConversionString( hIMC, IMR_CONV_CONVERSIONSTRING, compStrA, &compStrLenA ) != 0 )
                        goto attr_exit;

                    compStrA = ( LPSTR )malloc( compStrLenA + 1 );
                    if( !compStrA )
                        goto attr_exit;

                    compStrA[ compStrLenA ] = 0;

                    if( ImGetConversionString( hIMC, IMR_CONV_CONVERSIONSTRING, compStrA, &compStrLenA ) != 0 )
                        goto attr_exit;

                    compAttrA = ( PCHAR )malloc( compStrLenA + 1 );
                    if( !compAttrA )
                        goto attr_exit;

                    compAttrA[ compStrLenA ] = 0;

                    pchAttr = ( PCHAR )lpComp;

                    for( i = j = 0; ( i < dwCompLen ) && ( j < compStrLenA ); i++, j++ )
                    {
                        compAttrA[ j ] = pchAttr[ i ];
                        if( IsDBCSLeadByte( compStrA[ j ] ))
                            compAttrA[ ++j ] = pchAttr[ i ];
                    }

                    if( compStrLenA > j )
                        compStrLenA = j;
                }

                if( lpRead )
                {
                    if( ImGetConversionString( hIMC, IMR_CONV_READINGSTRING, readStrA, &readStrLenA ) != 0 )
                        goto attr_exit;

                    readStrA = ( LPSTR )malloc( readStrLenA + 1 );
                    if( !readStrA )
                        goto attr_exit;

                    readStrA[ readStrLenA ] = 0;

                    if( ImGetConversionString( hIMC, IMR_CONV_READINGSTRING, readStrA, &readStrLenA ) != 0 )
                        goto attr_exit;

                    readAttrA = ( PCHAR )malloc( readStrLenA + 1 );
                    if( !readAttrA )
                        goto attr_exit;

                    readAttrA[ readStrLenA ] = 0;

                    pchAttr = ( PCHAR )lpRead;

                    for( i = j = 0; ( i < dwReadLen ) && ( j < readStrLenA ); i++, j++ )
                    {
                        readAttrA[ j ] = pchAttr[ i ];
                        if( IsDBCSLeadByte( readStrA[ j ] ))
                            readAttrA[ ++j ] = pchAttr[ i ];
                    }

                    if( readStrLenA > j )
                        readStrLenA = j;
                }

                if( ImSetConversionString( hIMC, ulIndex, compAttrA, compStrLenA, readAttrA, readStrLenA ) != 0 )
                    goto attr_exit;

                rc = TRUE;

attr_exit :
                if( compStrA )
                    free( compStrA );

                if( compAttrA )
                    free( compAttrA );

                if( readStrA )
                    free( readStrA );

                if( readAttrA )
                    free( readAttrA );

                return rc;
            }

            case IME_SCS_CLAUSEINFO :
            {
                LPWSTR compStrW = NULL;
                INT    compStrLenW = 0;
                PULONG compClauseA = NULL;
                LPWSTR readStrW = NULL;
                INT    readStrLenW = 0;
                PULONG readClauseA = NULL;
                PULONG pulClause;
                INT    i, j;

                if( lpComp )
                {
                    if( ImGetConversionString( hIMC, IMR_CONV_CONVERSIONSTRING, compStrA, &compStrLenA ) != 0 )
                        goto clause_exit;

                    compStrA = ( LPSTR )malloc( compStrLenA + 1 );
                    if( !compStrA )
                        goto clause_exit;

                    compStrA[ compStrLenA ] = 0;

                    if( ImGetConversionString( hIMC, IMR_CONV_CONVERSIONSTRING, compStrA, &compStrLenA ) != 0 )
                        goto clause_exit;

                    compStrLenW = MultiByteToWideChar( CP_ACP, 0, compStrA, compStrLenA, 0, 0 );
                    compStrW = ( LPWSTR )malloc(( compStrLenW + 1 ) * sizeof( WCHAR ));
                    MultiByteToWideChar( CP_ACP, 0, compStrA, compStrLenA, compStrW, compStrLenW );

                    compClauseA = ( PULONG )malloc( dwCompLen );
                    if( !compClauseA )
                        goto clause_exit;

                    pulClause = ( PULONG )lpComp;

                    for( i = 0; ( i * sizeof( ULONG )) < dwCompLen; i++ )
                        compClauseA[ i ] = WideCharToMultiByte( CP_ACP, 0, compStrW, pulClause[ i ], 0, 0, 0, 0 );
                }

                if( lpRead )
                {
                    if( ImGetConversionString( hIMC, IMR_CONV_READINGSTRING, readStrA, &readStrLenA ) != 0 )
                        goto clause_exit;

                    readStrA = ( LPSTR )malloc( readStrLenA + 1 );
                    if( !readStrA )
                        goto clause_exit;

                    readStrA[ readStrLenA ] = 0;

                    if( ImGetConversionString( hIMC, IMR_CONV_READINGSTRING, readStrA, &readStrLenA ) != 0 )
                        goto clause_exit;

                    readStrLenW = MultiByteToWideChar( CP_ACP, 0, readStrA, readStrLenA, 0, 0 );
                    readStrW = ( LPWSTR )malloc(( readStrLenW + 1 ) * sizeof( WCHAR ));
                    MultiByteToWideChar( CP_ACP, 0, readStrA, readStrLenA, readStrW, readStrLenW );

                    readClauseA = ( PULONG )malloc( dwReadLen );
                    if( !readClauseA )
                        goto clause_exit;

                    pulClause = ( PULONG )lpRead;

                    for( i = 0; ( i * sizeof( ULONG )) < dwReadLen; i++ )
                        readClauseA[ i ] = WideCharToMultiByte( CP_ACP, 0, readStrW, pulClause[ i ], 0, 0, 0, 0 );
                }

                if( ImSetConversionString( hIMC, ulIndex, compClauseA, dwCompLen, readClauseA, dwReadLen ) != 0 )
                    goto clause_exit;

                rc = TRUE;

clause_exit :
                if( compStrA )
                    free( compStrA );

                if( compStrW )
                    free( compStrW );

                if( compClauseA )
                    free( compClauseA );

                if( readStrA )
                    free( readStrA );

                if( readStrW )
                    free( readStrW );

                if( readClauseA )
                    free( readClauseA );

                return rc;
            }
        }
    }
    else
    {
        if( ImSetConversionString( hIMC, ulIndex, ( PVOID )lpComp, dwCompLen, ( PVOID )lpRead, dwReadLen ) != 0 )
            return FALSE;
    }

    return TRUE;
}

BOOL    IM32SetCompositionWindow(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm)
{
    CONVERSIONPOS   cp;
    PIMINSTANCE     pimi;
    RECTL           rcl;
    SIZEF           sizef;

    if( ImRequestIMInstance( hIMC, &pimi ) != 0 )
        return FALSE;

    if(!( pimi->ulClientInfo & IMI_CI_PMCALLABLE ))
    {
        ImReleaseIMInstance( hIMC );
        return FALSE;
    }

    cp.ulStyle = CPS_DEFAULT;

    if( lpCompForm->dwStyle & CFS_FORCE_POSITION_W )
        cp.ulStyle |= CPS_FORCE;

    if( lpCompForm->dwStyle & CFS_POINT_W )
        cp.ulStyle |= CPS_POINT;

    if( lpCompForm->dwStyle & CFS_RECT_W )
        cp.ulStyle |= CPS_RECT;

    WinQueryWindowRect( pimi->hwnd, &rcl );

    cp.ptCurrentPos.x = lpCompForm->ptCurrentPos.x;
    cp.ptCurrentPos.y = rcl.yTop -
        ( lpCompForm->ptCurrentPos.y + pimi->faConversionWindow.lMaxBaselineExt );

    cp.rcArea.xLeft = lpCompForm->rcArea.left;
    cp.rcArea.xRight = lpCompForm->rcArea.right;
    cp.rcArea.yTop = rcl.yTop - rcl.yBottom - lpCompForm->rcArea.top;
    cp.rcArea.yBottom = rcl.yTop - rcl.yBottom - lpCompForm->rcArea.bottom;

    ImReleaseIMInstance( hIMC );

    if( ImSetConversionWindowPos( hIMC, &cp ) != 0 )
        return FALSE;

    return TRUE;
}

BOOL    IM32SetConversionStatus(HIMC hIMC, DWORD fdwConversion, DWORD fdwSentence)
{
    ULONG ulCurIM = 0;
    ULONG ulInputMode;
    ULONG ulConversionMode;

    if( ImQueryIMMode( hIMC, &ulCurIM, &ulConversionMode ) != 0 )
        return FALSE;

    ulInputMode = IMI_IM_NLS_ALPHANUMERIC;

    if( fdwConversion & IME_CMODE_NATIVE_W       )
        ulInputMode |= IMI_IM_NLS_HANGEUL;

    if( fdwConversion & IME_CMODE_KATAKANA_W     )
        ulInputMode |= IMI_IM_NLS_KATAKANA;

    if( fdwConversion & IME_CMODE_FULLSHAPE_W    )
        ulInputMode |= IMI_IM_WIDTH_FULL;

    if( fdwConversion & IME_CMODE_ROMAN_W        )
        ulInputMode |= IMI_IM_ROMAJI_ON;

    if( ulCurIM & IMI_IM_SYSTEMROMAJI_DISABLE )
        ulInputMode |= IMI_IM_SYSTEMROMAJI_DISABLE;

    if( ulCurIM & IMI_IM_2NDCONV )
        ulInputMode |= IMI_IM_2NDCONV;

    if( fdwConversion & IME_CMODE_CHARCODE_W     )
        ulInputMode |= 0;

    if( fdwConversion & IME_CMODE_HANJACONVERT_W )
        ulInputMode |= IMI_IM_IME_ON;           // right ?

    if( fdwConversion & IME_CMODE_SOFTKBD_W      )
        ulInputMode |= IMI_IM_IME_SOFTKBD_ON;

    if( fdwConversion & IME_CMODE_NOCONVERSION_W )
        ulInputMode |= IMI_IM_IME_DISABLE;      // right ?

    if( fdwConversion & IME_CMODE_EUDC_W         )
        ulInputMode |= 0;

    if( fdwConversion & IME_CMODE_SYMBOL_W       )
        ulInputMode |= IMI_IM_IME_CSYMBOL_ON;   // right ?

    ulConversionMode = IMI_CM_NONE;

    if( fdwSentence & IME_SMODE_PLAURALCLAUSE_W )
        ulConversionMode |= IMI_CM_PLURALCLAUSE;

    if( fdwSentence & IME_SMODE_SINGLECONVERT_W )
        ulConversionMode |= IMI_CM_SINGLE;

    if( fdwSentence & IME_SMODE_AUTOMATIC_W )
        ulConversionMode |= IMI_CM_AUTOMATIC;

    if( fdwSentence & IME_SMODE_PHRASEPREDICT_W )
        ulConversionMode |= IMI_CM_PREDICT;

    if( ImSetIMMode( hIMC, ulInputMode, ulConversionMode ) != 0 )
        return FALSE;

    return TRUE;
}

BOOL    IM32SetHotKey(DWORD dword, UINT uint, UINT uint2, HKL hkl)
{
    return FALSE;
}

BOOL    IM32SetOpenStatus(HIMC hIMC, BOOL fOpen)
{
    ULONG ulInputMode;
    ULONG ulConversionMode;

    if( ImQueryIMMode(( HIMI )hIMC, &ulInputMode, &ulConversionMode ) != 0 )
        return FALSE;

    if( fOpen )
        ulInputMode &= ~IMI_IM_IME_DISABLE;
    else
        ulInputMode |= IMI_IM_IME_DISABLE;

    if( ImSetIMMode(( HIMI )hIMC, ulInputMode, ulConversionMode ) != 0  )
        return FALSE;

    return TRUE;
}

BOOL    IM32SetStatusWindowPos(HIMC hIMC, LPPOINT lpptPos)
{
// TODO

    return FALSE;
}

BOOL    IM32ShowSoftKeyboard(HWND hwnd, int in1)
{
    return FALSE;
}

BOOL    IM32SimulateHotKey(HWND hWnd, DWORD dwHotKeyID)
{
    HWND  hwndOS2 = Win32ToOS2Handle( hWnd );
    HIMI  himi;
    ULONG ulInputMode;
    ULONG ulConversionMode;
    BOOL  rc = FALSE;

    if( ImGetInstance( hwndOS2, &himi ))
        return FALSE;

    if( ImQueryIMMode( himi, &ulInputMode, &ulConversionMode ))
        goto IM32SimulateHotKey_exit;

    switch( dwHotKeyID )
    {
        case IME_CHOTKEY_IME_NONIME_TOGGLE_W :
            ulInputMode ^= IMI_IM_IME_DISABLE;
            break;

        case IME_CHOTKEY_SHAPE_TOGGLE_W :
        case IME_CHOTKEY_SYMBOL_TOGGLE_W :
            // not supported
            goto IM32SimulateHotKey_exit;

        case IME_JHOTKEY_CLOSE_OPEN_W :
            ulInputMode ^= IMI_IM_IME_ON;
            break;

        case IME_KHOTKEY_SHAPE_TOGGLE_W :
            // not supported
            goto IM32SimulateHotKey_exit;

        case IME_KHOTKEY_HANJACONVERT_W :
            ulInputMode ^= IMI_IM_IME_ON; // right ?

        case IME_KHOTKEY_ENGLISH_W :
            ulInputMode ^= IMI_IM_NLS_HANGEUL;
            break;

        case IME_THOTKEY_IME_NONIME_TOGGLE_W :
            ulInputMode ^= IMI_IM_IME_DISABLE;
            break;

        case IME_THOTKEY_SHAPE_TOGGLE_W :
        case IME_THOTKEY_SYMBOL_TOGGLE_W :
            // not supported
            goto IM32SimulateHotKey_exit;
            break;
    }

    rc = !ImSetIMMode( himi, ulInputMode, ulConversionMode );

IM32SimulateHotKey_exit:

    ImReleaseInstance( hwndOS2, himi );

    return rc;
}

BOOL    IM32UnlockIMC(HIMC himc)
{
    return FALSE;
}

BOOL    IM32UnlockIMCC(HIMCC himcc)
{
    return FALSE;
}

BOOL    IM32UnregisterWord(HKL hKL, LPVOID lpszReading, DWORD dwStyle, LPVOID lpszUnregister, BOOL fUnicode)
{
// TODO

    return FALSE;
}


