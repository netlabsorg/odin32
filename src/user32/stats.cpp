#include <os2win.h>
#include <stats.h>

#ifdef RAS

RAS_TRACK_HANDLE rthDCCalls = 0;

//******************************************************************************
//******************************************************************************
void STATS_GetDCEx(HWND hwnd, HDC hdc, HRGN hrgn, ULONG flags)
{
    if (!rthDCCalls) STATS_InitializeUSER32 ();
    RasAddObject (rthDCCalls, hdc, NULL, 0);
}
//******************************************************************************
//******************************************************************************
void STATS_ReleaseDC(HWND hwnd, HDC hdc)
{
    if (!rthDCCalls) STATS_InitializeUSER32 ();
    RasRemoveObject (rthDCCalls, hdc);
}
//******************************************************************************
//******************************************************************************
void STATS_DumpStatsUSER32()
{
    RasLogObjects (rthDCCalls, RAS_FLAG_LOG_OBJECTS);
}
void STATS_InitializeUSER32 (void)
{
    RasEnterSerialize ();
    if (!rthDCCalls)
    {
        RasRegisterObjectTracking(&rthDCCalls, "DCCalls", 0, 0, NULL, NULL);
    }
    RasExitSerialize ();
}

void STATS_UninitializeUSER32(void)
{
}


#else

#ifdef DEBUG

static DWORD nrwindowdcsallocated = 0;
static DWORD nrcalls_GetDCEx = 0;
static DWORD nrcalls_ReleaseDC = 0;

//******************************************************************************
//******************************************************************************
void STATS_GetDCEx(HWND hwnd, HDC hdc, HRGN hrgn, ULONG flags)
{
    nrwindowdcsallocated++;
    nrcalls_GetDCEx++;
}
//******************************************************************************
//******************************************************************************
void STATS_ReleaseDC(HWND hwnd, HDC hdc)
{
    nrwindowdcsallocated--;
    nrcalls_ReleaseDC++;
}
//******************************************************************************
//******************************************************************************
void STATS_DumpStatsUSER32()
{
    dprintf(("*************  USER32 STATISTICS BEGIN *****************"));
    dprintf(("Total nr of GetDCEx   calls %d", nrcalls_GetDCEx));
    dprintf(("Total nr of ReleaseDC calls %d", nrcalls_ReleaseDC));
    dprintf(("Leaked DCs: %d", nrwindowdcsallocated));
    dprintf(("*************  USER STATISTICS END   *****************"));
}
//******************************************************************************
//******************************************************************************


#endif //DEBUG
#endif //RAS
