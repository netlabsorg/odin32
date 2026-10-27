#ifndef __INITTERM_H__
#define __INITTERM_H__

extern BOOL fMMPMReal;
BOOL MMPMEnsure();        //loads MMPM/2 on first use
BOOL MMPMInitialized();
#define fMMPMAvailable MMPMEnsure()

DWORD APIENTRY mymciSendCommand(WORD   wDeviceID,
                                    WORD   wMessage,
                                    DWORD  dwParam1,
                                    PVOID  dwParam2,
                                    WORD   wUserParm);
DWORD APIENTRY mymciGetErrorString(DWORD   dwError,
                                    LPSTR   lpstrBuffer,
                                    WORD    wLength);


#endif //__INITTERM_H__