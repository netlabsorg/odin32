/* $Id: hmparport.h,v 1.6 2001-12-05 14:16:05 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 Parallel Port device access class
 *
 * 2001 Patrick Haller <patrick.haller@innotek.de>
 *
 */

#ifndef _HM_PARPORT_H_
#define _HM_PARPORT_H_

class HMDeviceParPortClass : public HMDeviceHandler
{
  public:

  HMDeviceParPortClass(LPCSTR lpDeviceName);

  //checks if device name belongs to this class
  virtual BOOL FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength);

  /* this is the handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is the handler method for calls to CloseHandle() */
  virtual BOOL CloseHandle(PHMHANDLEDATA pHMHandleData);

  virtual BOOL GetCommProperties( PHMHANDLEDATA pHMHandleData,
                                 LPCOMMPROP lpcmmp);
  
  virtual BOOL ClearCommError( PHMHANDLEDATA pHMHandleData,
                               LPDWORD lpdwErrors,
                               LPCOMSTAT lpcst);
  
  virtual BOOL SetCommConfig( PHMHANDLEDATA pHMHandleData,
                              LPCOMMCONFIG lpCC,
                              DWORD dwSize );
  
  virtual BOOL GetCommConfig( PHMHANDLEDATA pHMHandleData,
                              LPCOMMCONFIG lpCC,
                              LPDWORD lpdwSize );
  
  virtual BOOL SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                     LPCOMMCONFIG lpCC,
                                     DWORD dwSize);
  
  virtual BOOL GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                     LPCOMMCONFIG lpCC,
                                     LPDWORD lpdwSize);
  
  /* this is a handler method for calls to DeviceIoControl() */
  virtual BOOL   DeviceIoControl    (PHMHANDLEDATA pHMHandleData, DWORD dwIoControlCode,
                                     LPVOID lpInBuffer, DWORD nInBufferSize,
                                     LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                     LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
  
  
  /* this is a handler method for calls to ReadFile/Ex */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to WriteFile/Ex */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  private:
    // The number of physically present parallel ports in the system,
    // LAN-redirectors don't count. See note in ::CreateFile()
    BYTE bNumberOfParallelPorts;
};


#endif // _HM_PARPORT_H_
