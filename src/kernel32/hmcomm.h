/* $Id: hmcomm.h,v 1.3 1999-11-26 21:10:52 phaller Exp $ */

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
    HMDeviceCommClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}
  
  /* @@@PH here go the API methods
  virtual DWORD CreateEvent (PHMHANDLEDATA         pHMHandleData,
                             LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  fManualReset,
                             BOOL                  fInitialState,
                             LPCTSTR               lpszEventName);
                             
  - strings are ASCII
  - HANDLEs are translated to a PHMHANDLEDATA pointer
                             
  */

};



#endif // _HM_COMM_H_
