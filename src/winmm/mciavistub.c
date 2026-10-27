/*
 * Built-in MCI "avivideo" stub driver.
 *
 * Opens an AVI file, reads its header (frame rate, frame count, size) and
 * answers the MCI command set so that applications which play intro videos
 * through MCI keep running.  No picture is decoded or displayed and no sound
 * is produced: "playing" completes immediately (the MCI_NOTIFY message is
 * delivered right away).
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <string.h>
#include "winbase.h"
#include "winerror.h"
#include "windef.h"
#include "wingdi.h"
#include "winuser.h"
#include <mmddk.h>
#include <mmsystem.h>

#define AVISTUB_TYPE_DIGITAL_VIDEO  0x0204   /* MCI_DEVTYPE_DIGITAL_VIDEO */

typedef struct {
    DWORD   dwUsPerFrame;
    DWORD   dwFrames;
    DWORD   dwWidth;
    DWORD   dwHeight;
    DWORD   dwMode;
    DWORD   dwTimeFormat;
    DWORD   dwPosition;     /* in frames */
    char    szFile[260];
} AVISTUB;

extern BOOL WINAPI mciDriverNotify(HWND hwndCallback, UINT uDeviceID, UINT uStatus);

/* read the main AVI header ('avih' chunk, 56 bytes) of a RIFF AVI file */
static void avistub_readheader(AVISTUB *a, const char *name)
{
    HANDLE h;
    BYTE buf[128];
    DWORD got = 0;

    h = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return;
    if (ReadFile(h, buf, sizeof(buf), &got, NULL) && got >= 72 &&
        !memcmp(buf, "RIFF", 4) && !memcmp(buf + 8, "AVI ", 4) &&
        !memcmp(buf + 12, "LIST", 4) && !memcmp(buf + 20, "hdrl", 4) &&
        !memcmp(buf + 24, "avih", 4)) {
        const DWORD *p = (const DWORD *)(buf + 32);
        a->dwUsPerFrame = p[0];
        a->dwFrames     = p[4];
        a->dwWidth      = p[8];
        a->dwHeight     = p[9];
    }
    CloseHandle(h);
}

static DWORD avistub_ms(const AVISTUB *a, DWORD frames)
{
    return (DWORD)(((unsigned __int64)frames * a->dwUsPerFrame) / 1000);
}

static DWORD avistub_notify(DWORD devID, DWORD flags, DWORD dwCallback, UINT status)
{
    if (flags & MCI_NOTIFY)
        mciDriverNotify((HWND)dwCallback, devID, status);
    return 0;
}

static DWORD avistub_open(DWORD devID, DWORD flags, LPMCI_OPEN_PARMSA parms)
{
    AVISTUB *a = (AVISTUB *)mciGetDriverData(devID);

    if (!a) return MCIERR_INVALID_DEVICE_ID;
    if (parms && (flags & MCI_OPEN_ELEMENT) && !(flags & MCI_OPEN_ELEMENT_ID) &&
        parms->lpstrElementName) {
        lstrcpynA(a->szFile, parms->lpstrElementName, sizeof(a->szFile));
        avistub_readheader(a, a->szFile);
    }
    a->dwMode = MCI_MODE_STOP;
    if (parms)
        parms->wDeviceID = (WORD)devID;
    return avistub_notify(devID, flags, parms ? parms->dwCallback : 0, MCI_NOTIFY_SUCCESSFUL);
}

static DWORD avistub_status(DWORD devID, DWORD flags, LPMCI_STATUS_PARMS parms)
{
    AVISTUB *a = (AVISTUB *)mciGetDriverData(devID);

    if (!a) return MCIERR_INVALID_DEVICE_ID;
    if (!parms) return MCIERR_NULL_PARAMETER_BLOCK;

    parms->dwReturn = 0;
    if (flags & MCI_STATUS_ITEM) {
        switch (parms->dwItem) {
        case MCI_STATUS_LENGTH:
            parms->dwReturn = a->dwTimeFormat == MCI_FORMAT_FRAMES ? a->dwFrames
                                                                   : avistub_ms(a, a->dwFrames);
            break;
        case MCI_STATUS_POSITION:
            parms->dwReturn = a->dwTimeFormat == MCI_FORMAT_FRAMES ? a->dwPosition
                                                                   : avistub_ms(a, a->dwPosition);
            break;
        case MCI_STATUS_MODE:
            parms->dwReturn = a->dwMode;
            break;
        case MCI_STATUS_READY:
        case MCI_STATUS_MEDIA_PRESENT:
            parms->dwReturn = TRUE;
            break;
        case MCI_STATUS_TIME_FORMAT:
            parms->dwReturn = a->dwTimeFormat;
            break;
        case MCI_STATUS_NUMBER_OF_TRACKS:
        case MCI_STATUS_CURRENT_TRACK:
            parms->dwReturn = 1;
            break;
        default:
            parms->dwReturn = 0;
            break;
        }
    }
    return avistub_notify(devID, flags, parms->dwCallback, MCI_NOTIFY_SUCCESSFUL);
}

static DWORD avistub_devcaps(DWORD devID, DWORD flags, LPMCI_GETDEVCAPS_PARMS parms)
{
    if (!parms) return MCIERR_NULL_PARAMETER_BLOCK;
    if (flags & MCI_GETDEVCAPS_ITEM) {
        switch (parms->dwItem) {
        case MCI_GETDEVCAPS_DEVICE_TYPE:     parms->dwReturn = AVISTUB_TYPE_DIGITAL_VIDEO; break;
        case MCI_GETDEVCAPS_HAS_VIDEO:
        case MCI_GETDEVCAPS_USES_FILES:
        case MCI_GETDEVCAPS_COMPOUND_DEVICE:
        case MCI_GETDEVCAPS_CAN_PLAY:        parms->dwReturn = TRUE;  break;
        default:                             parms->dwReturn = FALSE; break;
        }
    }
    return 0;
}

static DWORD avistub_play(DWORD devID, DWORD flags, LPMCI_PLAY_PARMS parms)
{
    AVISTUB *a = (AVISTUB *)mciGetDriverData(devID);

    if (!a) return MCIERR_INVALID_DEVICE_ID;
    /* nothing to show: the clip is considered played to its end */
    a->dwPosition = a->dwFrames;
    a->dwMode = MCI_MODE_STOP;
    return avistub_notify(devID, flags, parms ? parms->dwCallback : 0, MCI_NOTIFY_SUCCESSFUL);
}

static DWORD avistub_set(DWORD devID, DWORD flags, LPMCI_SET_PARMS parms)
{
    AVISTUB *a = (AVISTUB *)mciGetDriverData(devID);

    if (!a) return MCIERR_INVALID_DEVICE_ID;
    if (parms && (flags & MCI_SET_TIME_FORMAT))
        a->dwTimeFormat = parms->dwTimeFormat;
    return avistub_notify(devID, flags, parms ? parms->dwCallback : 0, MCI_NOTIFY_SUCCESSFUL);
}

static DWORD avistub_seek(DWORD devID, DWORD flags, LPMCI_SEEK_PARMS parms)
{
    AVISTUB *a = (AVISTUB *)mciGetDriverData(devID);

    if (!a) return MCIERR_INVALID_DEVICE_ID;
    if (flags & MCI_SEEK_TO_START)    a->dwPosition = 0;
    else if (flags & MCI_SEEK_TO_END) a->dwPosition = a->dwFrames;
    a->dwMode = MCI_MODE_STOP;
    return avistub_notify(devID, flags, parms ? parms->dwCallback : 0, MCI_NOTIFY_SUCCESSFUL);
}

/* MCI_WHERE / MCI_PUT share this layout: {DWORD dwCallback; RECT rc;} */
static DWORD avistub_where(DWORD devID, DWORD flags, DWORD *parms)
{
    AVISTUB *a = (AVISTUB *)mciGetDriverData(devID);

    if (!a) return MCIERR_INVALID_DEVICE_ID;
    if (parms) {
        RECT *rc = (RECT *)(parms + 1);
        rc->left = rc->top = 0;
        rc->right  = a->dwWidth;
        rc->bottom = a->dwHeight;
    }
    return 0;
}

static DWORD avistub_info(DWORD devID, DWORD flags, LPMCI_INFO_PARMSA parms)
{
    if (!parms || !parms->lpstrReturn || !parms->dwRetSize) return MCIERR_NULL_PARAMETER_BLOCK;
    if (flags & MCI_INFO_PRODUCT)
        lstrcpynA(parms->lpstrReturn, "Odin AVI video stub", parms->dwRetSize);
    else
        parms->lpstrReturn[0] = 0;
    return 0;
}

LONG WINAPI MCIAVISTUB_DriverProc(DWORD dwDevID, HDRVR hDriv, DWORD wMsg,
                                  DWORD dwParam1, DWORD dwParam2)
{
    switch (wMsg) {
    case DRV_LOAD:
    case DRV_FREE:
    case DRV_ENABLE:
    case DRV_DISABLE:
    case DRV_QUERYCONFIGURE:
        return 1;

    case DRV_OPEN:
    {
        LPMCI_OPEN_DRIVER_PARMSA modp = (LPMCI_OPEN_DRIVER_PARMSA)dwParam2;
        AVISTUB *a;

        if (!modp) return 0;
        a = (AVISTUB *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*a));
        if (!a) return 0;
        a->dwUsPerFrame = 66667;                 /* default 15 fps until a header is read */
        a->dwMode       = MCI_MODE_STOP;
        a->dwTimeFormat = MCI_FORMAT_MILLISECONDS;
        mciSetDriverData(modp->wDeviceID, (DWORD)a);
        modp->wCustomCommandTable = (WORD)-1;
        modp->wType = AVISTUB_TYPE_DIGITAL_VIDEO;
        return modp->wDeviceID;
    }

    case DRV_CLOSE:
    {
        AVISTUB *a = (AVISTUB *)mciGetDriverData(dwDevID);
        if (a) {
            mciSetDriverData(dwDevID, 0);
            HeapFree(GetProcessHeap(), 0, a);
        }
        return 1;
    }

    case DRV_INSTALL:
    case DRV_REMOVE:
        return DRVCNF_RESTART;
    }

    if (dwDevID == 0xFFFFFFFF) return MCIERR_UNSUPPORTED_FUNCTION;

    switch (wMsg) {
    case MCI_OPEN_DRIVER:   return avistub_open(dwDevID, dwParam1, (LPMCI_OPEN_PARMSA)dwParam2);
    case MCI_CLOSE_DRIVER:  return 0;
    case MCI_PLAY:          return avistub_play(dwDevID, dwParam1, (LPMCI_PLAY_PARMS)dwParam2);
    case MCI_SEEK:          return avistub_seek(dwDevID, dwParam1, (LPMCI_SEEK_PARMS)dwParam2);
    case MCI_SET:           return avistub_set(dwDevID, dwParam1, (LPMCI_SET_PARMS)dwParam2);
    case MCI_STATUS:        return avistub_status(dwDevID, dwParam1, (LPMCI_STATUS_PARMS)dwParam2);
    case MCI_GETDEVCAPS:    return avistub_devcaps(dwDevID, dwParam1, (LPMCI_GETDEVCAPS_PARMS)dwParam2);
    case MCI_INFO:          return avistub_info(dwDevID, dwParam1, (LPMCI_INFO_PARMSA)dwParam2);
    case MCI_WHERE:
    case MCI_PUT:           return avistub_where(dwDevID, dwParam1, (DWORD *)dwParam2);
    case MCI_STOP:
    case MCI_PAUSE:
    case MCI_RESUME:
    case MCI_CUE:
    case MCI_WINDOW:
    case MCI_UPDATE:
    case MCI_REALIZE:
    case MCI_FREEZE:
    case MCI_UNFREEZE:
    case MCI_STEP:
    {
        DWORD cb = dwParam2 ? *(DWORD *)dwParam2 : 0;
        return avistub_notify(dwDevID, dwParam1, cb, MCI_NOTIFY_SUCCESSFUL);
    }
    case MCI_OPEN:
    case MCI_CLOSE:
        return MCIERR_UNRECOGNIZED_COMMAND;     /* handled by the MCI layer itself */
    }
    return MCIERR_UNRECOGNIZED_COMMAND;
}
