#include <os2win.h>
#include <stats.h>

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
