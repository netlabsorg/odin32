#ifndef __DCIMAN_H__
#define __DCIMAN_H__

#include "dciddi.h"


typedef DWORD HWINWATCH;

#define WINWATCHNOTIFY_START        0
#define WINWATCHNOTIFY_STOP         1
#define WINWATCHNOTIFY_DESTROY      2
#define WINWATCHNOTIFY_CHANGING     3
#define WINWATCHNOTIFY_CHANGED      4
typedef void (* CALLBACK WINWATCHNOTIFYPROC)(HWINWATCH hww, HWND hwnd, DWORD code, LPARAM lParam);


HDC WINAPI DCIOpenProvider(void);
void WINAPI DCICloseProvider(HDC hdc);

int WINAPI DCICreatePrimary(HDC hdc, LPDCISURFACEINFO  *lplpSurface);
int WINAPI DCICreateOffscreen(HDC hdc, DWORD dwCompression, DWORD dwRedMask,
                              DWORD dwGreenMask, DWORD dwBlueMask, DWORD dwWidth, 
                              DWORD dwHeight, DWORD dwDCICaps, DWORD dwBitCount, 
                              LPDCIOFFSCREEN  *lplpSurface);

int WINAPI DCICreateOverlay(HDC hdc, LPVOID lpOffscreenSurf, 
                            LPDCIOVERLAY *lplpSurface);
int WINAPI DCIEnum(HDC hdc, LPRECT lprDst, LPRECT lprSrc, LPVOID lpFnCallback,
                   LPVOID lpContext);
DCIRVAL WINAPI DCISetSrcDestClip(LPDCIOFFSCREEN pdci, LPRECT srcrc,
                                 LPRECT destrc, LPRGNDATA prd );

void WINAPI DCIEndAccess(LPDCISURFACEINFO pdci);
DCIRVAL WINAPI DCIBeginAccess(LPDCISURFACEINFO pdci, int x, int y, int dx, int dy);
void WINAPI DCIDestroy(LPDCISURFACEINFO pdci);
DCIRVAL WINAPI DCIDraw(LPDCIOFFSCREEN pdci);
DCIRVAL WINAPI DCISetClipList(LPDCIOFFSCREEN pdci, LPRGNDATA prd);
DCIRVAL WINAPI DCISetDestination(LPDCIOFFSCREEN pdci, LPRECT dst, LPRECT src);

HWINWATCH WINAPI WinWatchOpen(HWND hwnd);
void WINAPI WinWatchClose(HWINWATCH hWW);
UINT WINAPI WinWatchGetClipList(HWINWATCH hWW, LPRECT prc, UINT size,  LPRGNDATA prd);
BOOL WINAPI WinWatchDidStatusChange(HWINWATCH hWW);
BOOL WINAPI WinWatchNotify(HWINWATCH hWW, WINWATCHNOTIFYPROC NotifyCallback,
                           LPARAM NotifyParam );


DWORD WINAPI GetWindowRegionData(HWND hwnd, DWORD size, LPRGNDATA prd);
DWORD WINAPI GetDCRegionData(HDC hdc, DWORD size, LPRGNDATA prd);





#endif // __DCIMAN_H__
