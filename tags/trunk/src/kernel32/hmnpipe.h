/* $Id: hmnpipe.h,v 1.2 2001-04-26 13:22:46 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 Named Pipes device access class
 *
 * Copyright 2000 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
 *
 */

#ifndef _HM_NPIPE_H_
#define _HM_NPIPE_H_

class HMDeviceNamedPipeClass : public HMDeviceFileClass
{
  public:
  HMDeviceNamedPipeClass(LPCSTR lpDeviceName) : HMDeviceFileClass(lpDeviceName) {}
 
  virtual BOOL  PeekNamedPipe(PHMHANDLEDATA pHMHandleData,
                              LPVOID lpvBuffer,
                              DWORD   cbBuffer,
                              LPDWORD lpcbRead,
                              LPDWORD lpcbAvail,
                              LPDWORD lpcbMessage);

  virtual DWORD CreateNamedPipe(PHMHANDLEDATA pHMHandleData, LPCTSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                                DWORD  nMaxInstances, DWORD  nOutBufferSize,
                                DWORD  nInBufferSize, DWORD  nDefaultTimeOut,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes);

  /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is a handler method for calls to CloseHandle() */
  virtual BOOL CloseHandle(PHMHANDLEDATA pHMHandleData);

  virtual BOOL ConnectNamedPipe(PHMHANDLEDATA pHMHandleData, LPOVERLAPPED lpOverlapped);

  virtual BOOL DisconnectNamedPipe(PHMHANDLEDATA pHMHandleData);

  virtual BOOL GetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                        LPDWORD lpState,
                                        LPDWORD lpCurInstances,
                                        LPDWORD lpMaxCollectionCount,
                                        LPDWORD lpCollectDataTimeout,
                                        LPTSTR  lpUserName,
                                        DWORD   nMaxUserNameSize);

  virtual BOOL GetNamedPipeInfo(PHMHANDLEDATA pHMHandleData,
                                LPDWORD lpFlags,
                                LPDWORD lpOutBufferSize,
                                LPDWORD lpInBufferSize,
                                LPDWORD lpMaxInstances);


  virtual DWORD TransactNamedPipe(PHMHANDLEDATA pHMHandleData,
                                  LPVOID lpvWriteBuf,
                                  DWORD cbWriteBuf,
                                  LPVOID lpvReadBuf,
                                  DWORD cbReadBuf,
                                  LPDWORD lpcbRead,
                                  LPOVERLAPPED lpo);

  virtual BOOL SetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                      LPDWORD lpdwMode,
                                      LPDWORD lpcbMaxCollect,
                                      LPDWORD lpdwCollectDataTimeout);

  virtual BOOL GetOverlappedResult (PHMHANDLEDATA pHMHandleData,
                                    LPOVERLAPPED  lpOverlapped,
                                    LPDWORD       arg3,
                                    BOOL          arg4);

  virtual BOOL  CreatePipe(PHMHANDLEDATA pHMHandleDataRead,
                           PHMHANDLEDATA pHMHandleDataWrite,
                           LPSECURITY_ATTRIBUTES lpsa, 
                           DWORD         cbPipe);


};


#endif // _HM_NPIPE_H_
