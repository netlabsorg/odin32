/* $Id: hmcomm.h,v 1.4 1999-11-27 12:48:26 achimha Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 COM device access class
 *
 * 1999 Achim Hasenmueller <achimha@innotek.de>
 *
 */

#ifndef _HM_COMM_H_
#define _HM_COMM_H_

class HMDeviceCommClass : public HMDeviceHandler
{
  public:

  HMDeviceCommClass(LPCSTR lpDeviceName);

  /* this is the handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is the handler method for calls to CloseHandle() */
  virtual DWORD  CloseHandle(PHMHANDLEDATA pHMHandleData);

  /* this is the handler method for SetComm() */
  virtual BOOL SetupComm(PHMHANDLEDATA pHMHandleData, DWORD dwInQueue, DWORD dwOutQueue);

};


#endif // _HM_COMM_H_
