/* $Id: objhandle.cpp,v 1.6 2000-11-15 13:56:45 sandervl Exp $ */
/*
 * Win32 Handle Management Code for OS/2
 *
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * TODO: The table should be dynamically increased when necessary
 *       This is just a quick and dirty implementation
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <vmutex.h>
#include <objhandle.h>
#include <dcdata.h>
#include <winuser32.h>
#include "oslibgpi.h"
#include "dibsect.h"
#include "region.h"

#define DBG_LOCALLOG    DBG_objhandle
#include "dbglocal.h"

//******************************************************************************

static ULONG  objHandleTable[MAX_OBJECT_HANDLES] = {0};
static ULONG  lowestFreeIndex = 0;
static VMutex objTableMutex;

//******************************************************************************
//******************************************************************************
BOOL ObjAllocateHandle(HANDLE *hObject, DWORD dwUserData, ObjectType type)
{
  objTableMutex.enter(VMUTEX_WAIT_FOREVER);
  if(lowestFreeIndex == -1) {
    //oops, out of handles
    dprintf(("ERROR: GDI: HwAllocateWindowHandle OUT OF GDI OBJECT HANDLES!!"));
    objTableMutex.leave();
    DebugInt3();
    return FALSE;
  }
  *hObject  = lowestFreeIndex;
  *hObject |= MAKE_HANDLE(type);
  objHandleTable[lowestFreeIndex] = dwUserData;

  lowestFreeIndex = -1;

  //find next free handle
  for(int i=0;i<MAX_OBJECT_HANDLES;i++) {
    if(objHandleTable[i] == 0) {
        lowestFreeIndex = i;
        break;
    }
  }
  objTableMutex.leave();
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void ObjFreeHandle(HANDLE hObject)
{
  hObject &= OBJHANDLE_MAGIC_MASK;
  if(hObject < MAX_OBJECT_HANDLES) {
    objTableMutex.enter(VMUTEX_WAIT_FOREVER);
    objHandleTable[hObject] = 0;
    if(lowestFreeIndex == -1 || hObject < lowestFreeIndex)
        lowestFreeIndex = hObject;

    objTableMutex.leave();
  }
}
//******************************************************************************
//******************************************************************************
DWORD ObjGetHandleData(HANDLE hObject)
{
  switch(GET_OBJTYPE(hObject))
  {
  case GDIOBJ_REGION:
    break;
  //case GDIOBJ_BITMAP
  //case GDIOBJ_BRUSH
  //case GDIOBJ_PALETTE
  //case GDIOBJ_FONT
  default:
    return HANDLE_OBJ_ERROR;
  }

  hObject &= OBJHANDLE_MAGIC_MASK;
  if(hObject < MAX_OBJECT_HANDLES) {
    return objHandleTable[hObject];
  }
  return HANDLE_OBJ_ERROR;
}
//******************************************************************************
//******************************************************************************
ObjectType ObjGetHandleType(HANDLE hObject)
{
  switch(GET_OBJTYPE(hObject))
  {
  case GDIOBJ_REGION:
    return GDIOBJ_REGION;
  //case GDIOBJ_BITMAP
  //case GDIOBJ_BRUSH
  //case GDIOBJ_PALETTE
  //case GDIOBJ_FONT
  default:
    return GDIOBJ_ERROR;
  }
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectA( HGDIOBJ hObject, int size, void *lpBuffer)
{
 int rc;

  dprintf(("GDI32: GetObject %X %X %X\n", hObject, size, lpBuffer));
  if(lpBuffer == NULL)
  { //return required size if buffer pointer == NULL
    int objtype = GetObjectType(hObject);
    switch(objtype)
    {
    case OBJ_PEN:
        return sizeof(LOGPEN);

    case OBJ_EXTPEN:
        return sizeof(EXTLOGPEN);

    case OBJ_BRUSH:
        return sizeof(LOGBRUSH);

    case OBJ_PAL:
        return sizeof(USHORT);

    case OBJ_FONT:
        return sizeof(LOGFONTA);

    case OBJ_BITMAP:
        return sizeof(BITMAP); //also default for dib sections??? (TODO: NEED TO CHECK THIS)

    case OBJ_DC:
    case OBJ_METADC:
    case OBJ_REGION:
    case OBJ_METAFILE:
    case OBJ_MEMDC:
    case OBJ_ENHMETADC:
    case OBJ_ENHMETAFILE:
        dprintf(("warning: GetObjectA not defined for object type %d", objtype));
        return 0;
    }
  }
  if(DIBSection::getSection() != NULL)
  {
        DIBSection *dsect = DIBSection::find(hObject);
        if(dsect)
        {
                rc = dsect->GetDIBSection(size, lpBuffer);
                if(rc == 0) {
                        SetLastError(ERROR_INVALID_PARAMETER);
                        return 0;
                }
                SetLastError(ERROR_SUCCESS);
                return rc;
        }
  }

  return O32_GetObject(hObject, size, lpBuffer);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectW( HGDIOBJ hObject, int size, void *lpBuffer)
{
 int ret, objtype;

  dprintf(("GDI32: GetObjectW %X, %d %X", hObject, size, lpBuffer));
  objtype = GetObjectType(hObject);

  switch(objtype)
  {
  case OBJ_FONT:
  {
    LOGFONTA logfonta;

        if(lpBuffer == NULL) {
            return sizeof(LOGFONTW); //return required size if buffer pointer == NULL
        }
        ret = GetObjectA(hObject, sizeof(logfonta), (void *)&logfonta);
        if(ret == sizeof(logfonta))
        {
            LOGFONTW *logfontw = (LOGFONTW *)lpBuffer;

            if(size < sizeof(LOGFONTW)) {
                dprintf(("GDI32: GetObjectW : buffer not big enough for LOGFONTW struct!!")); //is the correct? or copy only part?
                return 0;
            }
            memcpy(logfontw, &logfonta, sizeof(LOGFONTA));
            memset(logfontw->lfFaceName, 0, LF_FACESIZE);
            AsciiToUnicodeN(logfonta.lfFaceName, logfontw->lfFaceName, LF_FACESIZE-1);

            return sizeof(LOGFONTW);
        }
        return 0;
  }
  default:
      return GetObjectA(hObject, size, lpBuffer);
  }
}
//******************************************************************************
//******************************************************************************
HGDIOBJ WIN32API SelectObject(HDC hdc, HGDIOBJ hObj)
{
 HGDIOBJ rc;

    dprintf2(("GDI32: SelectObject %x %x", hdc, hObj));

    if(ObjGetHandleType(hObj) == GDIOBJ_REGION) {
        //Return complexity here; not previously selected clip region
        return (HGDIOBJ)SelectClipRgn(hdc, hObj);
    }

    if(DIBSection::getSection() != NULL)
    {
      DIBSection *dsect;

      dsect = DIBSection::find(hdc);
      if(dsect)
      {
        //remove previously selected dibsection
        dsect->UnSelectDIBObject();
      }
      dsect = DIBSection::find((DWORD)hObj);
      if(dsect)
      {
        dsect->SelectDIBObject(hdc);
      }
    }
    rc = O32_SelectObject(hdc, hObj);
    if(rc != 0 && DIBSection::getSection != NULL)
    {
      DIBSection *dsect = DIBSection::find((DWORD)rc);
      if(dsect)
      {
        dsect->UnSelectDIBObject();
      }
    }
#ifdef USING_OPEN32
    if(O32_GetObjectType(hObj) == OBJ_BITMAP) 
    {
	//SvL: Open32 messes up the height of the hdc (for windows)
        pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
        if(pHps && pHps->hwnd) {
      	      dprintf2(("change back origin"));
              selectClientArea(pHps);
              setPageXForm(pHps);
        }
    }
#endif
    dprintf2(("GDI32: SelectObject %x %x returned %x", hdc, hObj, rc));

    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetObjectType( HGDIOBJ hObj)
{
  dprintf2(("GDI32: GetObjectType\n"));
  if(ObjGetHandleType(hObj) == GDIOBJ_REGION) {
        SetLastError(ERROR_SUCCESS);
    return OBJ_REGION;
  }
  return O32_GetObjectType(hObj);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteObject(HANDLE hObj)
{
  dprintf(("GDI32: DeleteObject %x", hObj));
  if(ObjGetHandleType(hObj) == GDIOBJ_REGION) {
    OSLibDeleteRegion(ObjGetHandleData(hObj));
    ObjFreeHandle(hObj);
        SetLastError(ERROR_SUCCESS);
    return OBJ_REGION;
  }
  DIBSection::deleteSection((DWORD)hObj);
  return O32_DeleteObject(hObj);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetObjectOwner( HGDIOBJ arg1, int arg2 )
{
  // Here is a guess for a undocumented entry
  dprintf(("WARNING: GDI32: SetObjectOwner - stub (TRUE)\n"));
  return TRUE;
}
//******************************************************************************
//******************************************************************************
