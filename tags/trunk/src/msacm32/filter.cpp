/*
 *      MSACM32 library
 *
 *      Copyright 1998  Patrik Stridvall
 */

#include <os2win.h>
#include "winbase.h"
#include "winerror.h"
#include "mmsystem.h"
#include "msacm.h"
#include "msacmdrv.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(msacm)

/***********************************************************************
 *           acmFilterChooseA (MSACM32.13)
 */
MMRESULT WINAPI acmFilterChooseA(
  PACMFILTERCHOOSEA pafltrc)
{
  dprintf(("(%p): stub\n", pafltrc));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterChooseW (MSACM32.14)
 */
MMRESULT WINAPI acmFilterChooseW(
  PACMFILTERCHOOSEW pafltrc)
{
  dprintf(("(%p): stub\n", pafltrc));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterDetailsA (MSACM32.15)
 */
MMRESULT WINAPI acmFilterDetailsA(
  HACMDRIVER had, PACMFILTERDETAILSA pafd, DWORD fdwDetails)
{
  if(fdwDetails & ~(ACM_FILTERDETAILSF_FILTER))
    return MMSYSERR_INVALFLAG;

  /* FIXME
   *   How does the driver know if the ANSI or
   *   the UNICODE variant of PACMFILTERDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  return (MMRESULT) acmDriverMessage(
    had, ACMDM_FILTER_DETAILS,
    (LPARAM) pafd,  (LPARAM) fdwDetails
  );
}

/***********************************************************************
 *           acmFilterDetailsW (MSACM32.16)
 */
MMRESULT WINAPI acmFilterDetailsW(
  HACMDRIVER had, PACMFILTERDETAILSW pafd, DWORD fdwDetails)
{
  dprintf(("(0x%08x, %p, %ld): stub\n", had, pafd, fdwDetails));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterEnumA (MSACM32.17)
 */
MMRESULT WINAPI acmFilterEnumA(
  HACMDRIVER had, PACMFILTERDETAILSA pafd, 
  ACMFILTERENUMCBA fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
  dprintf(("(0x%08x, %p, %p, %ld, %ld): stub\n",
    had, pafd, fnCallback, dwInstance, fdwEnum
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterEnumW (MSACM32.18)
 */
MMRESULT WINAPI acmFilterEnumW(
  HACMDRIVER had, PACMFILTERDETAILSW pafd, 
  ACMFILTERENUMCBW fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
  dprintf(("(0x%08x, %p, %p, %ld, %ld): stub\n",
    had, pafd, fnCallback, dwInstance, fdwEnum
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterTagDetailsA (MSACM32.19)
 */
MMRESULT WINAPI acmFilterTagDetailsA(
  HACMDRIVER had, PACMFILTERTAGDETAILSA paftd, DWORD fdwDetails)
{
  if(fdwDetails & 
     ~(ACM_FILTERTAGDETAILSF_FILTERTAG|
       ACM_FILTERTAGDETAILSF_LARGESTSIZE))
    return MMSYSERR_INVALFLAG;

  /* FIXME
   *   How does the driver know if the ANSI or
   *   the UNICODE variant of PACMFILTERTAGDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  return (MMRESULT) acmDriverMessage(
    had, ACMDM_FILTERTAG_DETAILS,
    (LPARAM) paftd,  (LPARAM) fdwDetails
  );
}

/***********************************************************************
 *           acmFilterTagDetailsW (MSACM32.20)
 */
MMRESULT WINAPI acmFilterTagDetailsW(
  HACMDRIVER had, PACMFILTERTAGDETAILSW paftd, DWORD fdwDetails)
{
  dprintf(("(0x%08x, %p, %ld): stub\n", had, paftd, fdwDetails));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterTagEnumA (MSACM32.21)
 */
MMRESULT WINAPI acmFilterTagEnumA(
  HACMDRIVER had, PACMFILTERTAGDETAILSA  paftd,
  ACMFILTERTAGENUMCBA fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
  dprintf(("(0x%08x, %p, %p, %ld, %ld): stub\n",
    had, paftd, fnCallback, dwInstance, fdwEnum
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterTagEnumW (MSACM32.22)
 */
MMRESULT WINAPI acmFilterTagEnumW(
  HACMDRIVER had, PACMFILTERTAGDETAILSW paftd,
  ACMFILTERTAGENUMCBW fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
  dprintf(("(0x%08x, %p, %p, %ld, %ld): stub\n",
    had, paftd, fnCallback, dwInstance, fdwEnum
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}
