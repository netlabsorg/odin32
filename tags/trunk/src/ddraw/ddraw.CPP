#include <memory.h>

#include <builtin.h>
#define INITGUID
#include "os2ddraw.h"
// define the following as we include winnt.h
#define _OS2WIN_H
#define FAR
#include "misc.h"

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectDrawCreate( GUID FAR *lpGUID,
                                      LPDIRECTDRAW FAR *lplpDD,
                                      IUnknown FAR *pUnkOuter)
{
  OS2IDirectDraw *newdraw;
  HRESULT         rc;

  WriteLog("DirectDrawCreate %X %X %X\n", lpGUID, lplpDD, pUnkOuter);
  newdraw = new OS2IDirectDraw(lpGUID);

  if(newdraw == NULL)
    return(DDERR_NODIRECTDRAWHW);

//  newdraw->Vtbl.AddRef((IDirectDraw *)newdraw);
  rc = newdraw->GetLastError();
  if(rc != DD_OK)
  {
    *lplpDD = NULL;
    delete newdraw;
  }
  else
    *lplpDD = (LPDIRECTDRAW)newdraw;

  return(rc);
}
//******************************************************************************
typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKA)(GUID FAR *, LPSTR, LPSTR, LPVOID);
//******************************************************************************
HRESULT WIN32API OS2DirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback,
               LPVOID lpContext)
{
  WriteLog("DirectDrawEnumerateA\n Callback for DIVE\n");
  //call it twice for the DirectDraw & Direct3D classes
 if(lpCallback(NULL, "DIVE DirectDraw for OS/2",
                "DirectDraw/2 v0.1", lpContext) == DDENUMRET_CANCEL)
  {
    WriteLog("Cancel Callback\n");
    return(DD_OK);
  }
  else //now for Direct3D
  {
    WriteLog("Callback for 3Dfx Voodoo");
    if(lpCallback((GUID *)&IID_IDirect3D, "3Dfx Voodoo Direct3D/2",
      "Direct3D/2 v0.1", lpContext) == DDENUMRET_CANCEL)
    {
      WriteLog("Cancel Callback\n");
      return(DD_OK);
    }
  }
  WriteLog("Done Enumeration\n\n");

  return(DD_OK);
}
//******************************************************************************
typedef struct
{
  LPDDENUMCALLBACKEXA lpCallbackEx;
  LPVOID lpContext;
} ENUMDATA, *PENUMDATA;

BOOL FAR PASCAL SimpleEnum ( GUID FAR *lpGUID,
                             LPSTR  lpDriverDescription,
                             LPSTR  lpDriverName,
                             LPVOID lpContext)
{
  BOOL rc;
  PENUMDATA pData;

  WriteLog("SimpleEnum\n");

  pData = (PENUMDATA)lpContext;
  rc = pData->lpCallbackEx( lpGUID,
                            lpDriverDescription,
                            lpDriverName,
                            pData->lpContext,
                            NULL);

  WriteLog("Callback returned");
  return rc;
}

//******************************************************************************
HRESULT WIN32API OS2DirectDrawEnumerateExA( LPDDENUMCALLBACKEXA lpCallbackEx,
                                            LPVOID lpContext,
                                            DWORD dwFlags)
{
  ENUMDATA data;

  WriteLog("DirectDrawEnumerateExA\n");

  data.lpCallbackEx = lpCallbackEx;
  data.lpContext    = lpContext;

   OS2DirectDrawEnumerateA( SimpleEnum,
                            &data);
  return (DD_OK);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DDHAL32_VidMemFree(DWORD address)
{
  WriteLog("DDHAL32_VidMemFree, not supported\n");
  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DDHAL32_VidMemAlloc(DWORD size)
{
  WriteLog("DDHAL32_VidMemAlloc, not supported\n");
  return(0);
}
//******************************************************************************
//******************************************************************************
