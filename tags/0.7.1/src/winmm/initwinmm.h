#ifndef __INITWINMM_H__
#define __INITWINMM_H__

extern BOOL fMMPMAvailable;

DWORD APIENTRY mymciSendCommand(WORD   wDeviceID,
                                    WORD   wMessage,
                                    DWORD  dwParam1,
                                    PVOID  dwParam2,
                                    WORD   wUserParm);
DWORD APIENTRY mymciGetErrorString(DWORD   dwError,
                                    LPSTR   lpstrBuffer,
                                    WORD    wLength);


#endif //__INITWINMM_H__