#include <os2win.h>
#include "cdrom.h"

int	CDAUDIO_Open(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Open not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Close(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Close not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Reset(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Reset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Play(WINE_CDAUDIO* wcda, DWORD start, DWORD stop)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Play not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Stop(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Stop not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Pause(WINE_CDAUDIO* wcda, int pauseOn)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Pause not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Seek(WINE_CDAUDIO* wcda, DWORD at)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Seek not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_SetDoor(WINE_CDAUDIO* wcda, int open)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_SetDoor not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

UINT16 	CDAUDIO_GetNumberOfTracks(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_GetNumberOfTracks not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

BOOL 	CDAUDIO_GetTracksInfo(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_GetTracksInfo not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

BOOL	CDAUDIO_GetCDStatus(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_GetCDStatus not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


