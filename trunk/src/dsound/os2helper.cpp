#define INCL_DOSMISC
#define INCL_DOSPROFILE
#include <os2wrap.h>
#include <misc.h>
#include "asmutil.h"

typedef ULONG DWORD ;

#if 1
DWORD GetTickCountOS2()
{
  ULONG ulTickCount;
  APIRET rc;
  rc = DosQuerySysInfo( QSV_MS_COUNT,
                        QSV_MS_COUNT,
                        &ulTickCount,
                        sizeof(ULONG));
  return 0==rc?ulTickCount:0x00FFDEAD;
}
#else
DWORD GetTickCountOS2()
{
  QWORD qwTickCount;
  ULONG ulFreq;
  ULONG ulTicks;
  APIRET rc;

  rc = DosTmrQueryFreq( &ulFreq);
  dprintf(("DSOUND: GetTickCountOS2 Freq: %d(%08X)\n",
           ulFreq, ulFreq));

  if(0==rc)
  {
    rc = DosTmrQueryTime(&qwTickCount);
    dprintf(("DSOUND: GetTickCountOS2 Time %d(%08X) : %d(%08X)\n",
             qwTickCount.ulHi, qwTickCount.ulHi,
             qwTickCount.ulLo,qwTickCount.ulLo, ulTicks));
    ulTicks = GetMsCount(&qwTickCount,ulFreq);
    dprintf(("DSOUND: GetTickCountOS2 ms Ticks %d\n"));
  }
  return 0==rc?ulTicks:0x00FFDEAD;
}
#endif
