#ifndef __INITDSOUND_H__
#define __INITDSOUND_H__

extern BOOL fdsMMPMAvailable;

//logs to the file named by the environment variable DSOUND_LOG (if set)
void dslog(const char *fmt, ...);

DWORD APIENTRY dsmciSendCommand(WORD   wDeviceID,
                                    WORD   wMessage,
                                    DWORD  dwParam1,
                                    PVOID  dwParam2,
                                    WORD   wUserParm);
DWORD APIENTRY dsmciGetErrorString(DWORD   dwError,
                                    LPSTR   lpstrBuffer,
                                    WORD    wLength);


#endif //__INITDSOUND_H__