#define INCL_DOSMISC
#define INCL_DOSPROFILE
#include <os2wrap.h>
#include <misc.h>

typedef ULONG DWORD ;

#if 0
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
  DWORD dwTick;
  APIRET rc;

  rc = DosTmrQueryFreq( &ulFreq);
  if(0==rc)
  {
    rc = DosTmrQueryTime(&qwTickCount);
    if(0==rc)
    {
      ulFreq /=1000;
      dwTick = qwTickCount.ulLo / ulFreq;
      //dwTick = ( (qwTickCount.ulHi <<24)+ (qwTickCount.ulLo>>8)) / (ulFreq>>8);
    }
  }
  //dprintf(("DSOUND: GetTickCountOS2 Freq: %d(%08X) Time %d(%08X) : %d(%08X)\n",
  //         ulFreq, ulFreq,
  //         qwTickCount.ulHi, qwTickCount.ulHi,
  //         qwTickCount.ulLo,qwTickCount.ulLo));
  return 0==rc?dwTick:0x00FFDEAD;
}
#endif
