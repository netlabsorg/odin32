#ifndef _STATISTICS_H__
#define _STATISTICS_H__

//Get memory statistics from odincrt
void _LNK_CONV getcrtstat(unsigned long *pnrcalls_malloc,
                          unsigned long *pnrcalls_free,
                          unsigned long *ptotalmemalloc);

#ifdef DEBUG
void STATS_GetDCEx(HWND hwnd, HDC hdc, HRGN hrgn, ULONG flags);
void STATS_ReleaseDC(HWND hwnd, HDC hdc);
void STATS_DumpStats();
#else
#define STATS_GetDCEx(a, b, c, d)
#define STATS_ReleaseDC(a,b)
#define STATS_DumpStats()
#endif

#endif