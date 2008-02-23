#ifndef __INITDSOUND_H__
#define __INITDSOUND_H__

extern BOOL fdsMMPMAvailable;

DWORD APIENTRY dsmciSendCommand(WORD   wDeviceID,
                                    WORD   wMessage,
                                    DWORD  dwParam1,
                                    PVOID  dwParam2,
                                    WORD   wUserParm);
DWORD APIENTRY dsmciGetErrorString(DWORD   dwError,
                                    LPSTR   lpstrBuffer,
                                    WORD    wLength);


#endif //__INITDSOUND_H__