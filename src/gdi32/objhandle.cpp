/* $Id: objhandle.cpp,v 1.21 2002-04-07 14:38:08 sandervl Exp $ */
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
#include <unicode.h>
#include "font.h"
#include <stats.h>

#define DBG_LOCALLOG    DBG_objhandle
#include "dbglocal.h"

//TODO: must use 16 bits gdi object handles
#define GDIOBJ_PREFIX 0xe7000000

//******************************************************************************

typedef struct {
  ULONG      dwUserData;
  ObjectType type;
} GdiObject;

static GdiObject objHandleTable[MAX_OBJECT_HANDLES] = {0};
static ULONG     lowestFreeIndex = 1;
static VMutex    objTableMutex;

#ifdef DEBUG
static DWORD GetObjectTypeNoDbg( HGDIOBJ hObj);
#else
#define GetObjectTypeNoDbg GetObjectType
#endif

//******************************************************************************
//******************************************************************************
BOOL WIN32API ObjAllocateHandle(HANDLE *hObject, DWORD dwUserData, ObjectType type)
{
    objTableMutex.enter();
    if(lowestFreeIndex == -1) {
        //oops, out of handles
        objTableMutex.leave();
        dprintf(("ERROR: GDI: ObjAllocateHandle OUT OF GDI OBJECT HANDLES!!"));
        DebugInt3();
        return FALSE;
    }
    if(objHandleTable[0].type == 0) {
        //first handle can never be used
        objHandleTable[0].type       = GDIOBJ_INVALID;
        objHandleTable[0].dwUserData = -1;
    }
    *hObject  = lowestFreeIndex;
    *hObject |= MAKE_HANDLE(type);
    objHandleTable[lowestFreeIndex].dwUserData = dwUserData;
    objHandleTable[lowestFreeIndex].type       = type;

    lowestFreeIndex = -1;

    //find next free handle
    for(int i=0;i<MAX_OBJECT_HANDLES;i++) {
        if(objHandleTable[i].dwUserData == 0) {
            lowestFreeIndex = i;
            break;
        }
    }
    objTableMutex.leave();
    return TRUE;
}
//******************************************************************************
//******************************************************************************
void WIN32API ObjFreeHandle(HANDLE hObject)
{
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES) {
        objTableMutex.enter();
        objHandleTable[hObject].dwUserData = 0;
        objHandleTable[hObject].type = GDIOBJ_NONE;
        if(lowestFreeIndex == -1 || hObject < lowestFreeIndex)
            lowestFreeIndex = hObject;

        objTableMutex.leave();
    }
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ObjGetHandleData(HANDLE hObject, ObjectType type)
{
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && type == objHandleTable[hObject].type) {
        return objHandleTable[hObject].dwUserData;
    }
    return HANDLE_OBJ_ERROR;
}
//******************************************************************************
//******************************************************************************
ObjectType WIN32API ObjGetHandleType(HANDLE hObject)
{
 DWORD objtype;

    switch(OBJHANDLE_MAGIC(hObject))
    {
    case GDIOBJ_REGION:
        hObject &= OBJHANDLE_MAGIC_MASK;
        if(hObject < MAX_OBJECT_HANDLES && objHandleTable[hObject].type == GDIOBJ_REGION) {
            return GDIOBJ_REGION;
        }
        break;

    case USEROBJ_MENU:
        hObject &= OBJHANDLE_MAGIC_MASK;
        if(hObject < MAX_OBJECT_HANDLES && objHandleTable[hObject].type == USEROBJ_MENU) {
            return USEROBJ_MENU;
        }
        break;

    case GDIOBJ_NONE:
        //could be a cutoff menu handle, check this
        //TODO: dangerous assumption! (need to rewrite object handle management)
        hObject &= OBJHANDLE_MAGIC_MASK;
        if(hObject < MAX_OBJECT_HANDLES && objHandleTable[hObject].dwUserData != 0 && objHandleTable[hObject].type == USEROBJ_MENU) {
            return USEROBJ_MENU;
        }
        break;

    case GDIOBJ_BITMAP:
    case GDIOBJ_BRUSH:
    case GDIOBJ_PALETTE:
    case GDIOBJ_FONT:
    case USEROBJ_ACCEL:
    default:
        break;
    }
    return GDIOBJ_NONE;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectA( HGDIOBJ hObject, int size, void *lpBuffer)
{
 int rc;

  dprintf(("GDI32: GetObject %X %X %X\n", hObject, size, lpBuffer));
  //TODO: must use 16 bits gdi object handles
  if(HIWORD(hObject) == 0) {
        hObject |= GDIOBJ_PREFIX;
  }
  if(lpBuffer == NULL)
  { //return required size if buffer pointer == NULL
    int objtype = GetObjectTypeNoDbg(hObject);
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
        DIBSection *dsect = DIBSection::findObj(hObject);
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

  //TODO: must use 16 bits gdi object handles
  if(HIWORD(hObject) == 0) {
        hObject |= GDIOBJ_PREFIX;
  }
  objtype = GetObjectTypeNoDbg(hObject);

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
#ifdef DEBUG
static char *gditypenames[] = {
"NULL",
"OBJ_PEN", 
"OBJ_BRUSH", 
"OBJ_DC", 
"OBJ_METADC", 
"OBJ_PAL", 
"OBJ_FONT", 
"OBJ_BITMAP", 
"OBJ_REGION", 
"OBJ_METAFILE", 
"OBJ_MEMDC", 
"OBJ_EXTPEN", 
"OBJ_ENHMETADC", 
"OBJ_ENHMETAFILE"
};

char *DbgGetGDITypeName(DWORD handleType)
{
    if(handleType <= OBJ_ENHMETAFILE) {
        return gditypenames[handleType];
    }
    return "UNKNOWN TYPE";
}
#endif
//******************************************************************************
//******************************************************************************
HGDIOBJ WIN32API SelectObject(HDC hdc, HGDIOBJ hObj)
{
 HGDIOBJ rc;
 DWORD   handleType;

    //TODO: must use 16 bits gdi object handles
    if(HIWORD(hObj) == 0) {
        hObj |= GDIOBJ_PREFIX;
    }

    handleType = GetObjectTypeNoDbg(hObj);
    dprintf2(("GDI32: SelectObject %x %x type %s", hdc, hObj, DbgGetGDITypeName(handleType)));
    if(handleType == GDIOBJ_REGION) {
        //Return complexity here; not previously selected clip region
        return (HGDIOBJ)SelectClipRgn(hdc, hObj);
    }

    if(handleType == OBJ_BITMAP && DIBSection::getSection() != NULL)
    {
        DIBSection *dsect;

        dsect = DIBSection::findHDC(hdc);
        if(dsect)
        {
            //remove previously selected dibsection
            dsect->UnSelectDIBObject();
        }
        dsect = DIBSection::findObj(hObj);
        if(dsect)
        {
            dsect->SelectDIBObject(hdc);
        }
    }
    rc = O32_SelectObject(hdc, hObj);
    if(rc != 0 && GetObjectTypeNoDbg(rc) == OBJ_BITMAP && DIBSection::getSection != NULL)
    {
        DIBSection *dsect = DIBSection::findObj(rc);
        if(dsect)
        {
            dsect->UnSelectDIBObject();
        }
    }
#ifdef USING_OPEN32
    if(handleType == OBJ_BITMAP)
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
//Called from user32 ReleaseDC (for non CS_OWNDC hdcs)
//******************************************************************************
VOID WIN32API UnselectGDIObjects(HDC hdc)
{
    DIBSection *dsect;

    dsect = DIBSection::findHDC(hdc);
    if(dsect)
    {
        //remove previously selected dibsection
        dsect->UnSelectDIBObject();
    }
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
static DWORD GetObjectTypeNoDbg( HGDIOBJ hObj)
{
    DWORD objtype;

    //TODO: must use 16 bits gdi object handles
    if(HIWORD(hObj) == 0) {
        hObj |= GDIOBJ_PREFIX;
    }
    if(ObjGetHandleType(hObj) == GDIOBJ_REGION) {
        dprintf2(("GDI32: GetObjectType %x REGION", hObj));
        SetLastError(ERROR_SUCCESS);
        return OBJ_REGION;
    }
    return O32_GetObjectType(hObj);
}
#endif
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetObjectType( HGDIOBJ hObj)
{
    DWORD objtype;

    //TODO: must use 16 bits gdi object handles
    if(HIWORD(hObj) == 0) {
        hObj |= GDIOBJ_PREFIX;
    }
    if(ObjGetHandleType(hObj) == GDIOBJ_REGION) {
        dprintf2(("GDI32: GetObjectType %x REGION", hObj));
        SetLastError(ERROR_SUCCESS);
        return OBJ_REGION;
    }
    objtype = O32_GetObjectType(hObj);
    dprintf2(("GDI32: GetObjectType %x objtype %d", hObj, objtype));
    return objtype;
}
//******************************************************************************
//TODO: System objects can't be deleted (TODO: any others?? (fonts?))!!!!)
//******************************************************************************
BOOL WIN32API DeleteObject(HANDLE hObj)
{
    DWORD objtype;

    dprintf(("GDI32: DeleteObject %x", hObj));

    //TODO: must use 16 bits gdi object handles
    if(HIWORD(hObj) == 0)
    {
        hObj |= GDIOBJ_PREFIX;
    }

    //System objects can't be deleted (TODO: any others?? (fonts?))!!!!)
    objtype = GetObjectTypeNoDbg(hObj);
    switch (objtype)
    {
        case OBJ_PEN:
            if(IsSystemPen(hObj))
            {
                SetLastError(ERROR_SUCCESS);
                return TRUE;
            }
            else
                break;

        case OBJ_BRUSH:
            if(IsSystemBrush(hObj))
            {
                SetLastError(ERROR_SUCCESS);
                return TRUE;
            }
            else
                break;

        case OBJ_FONT:
            if(IsSystemFont(hObj))
            {
                SetLastError(ERROR_SUCCESS);
                return TRUE;
            }
            else
                break;
            
        // add more system-type objects as required ...
    }

    STATS_DeleteObject(hObj, objtype);

    if(ObjGetHandleType(hObj) == GDIOBJ_REGION)
    {
        OSLibDeleteRegion(ObjGetHandleData(hObj, GDIOBJ_REGION));
        ObjFreeHandle(hObj);
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    DIBSection::deleteSection((DWORD)hObj);
    return O32_DeleteObject(hObj);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumObjects( HDC hdc, int objType, GOBJENUMPROC objFunc, LPARAM lParam)
{
    //calling convention differences
    dprintf(("!ERROR!: GDI32: EnumObjects STUB"));
//    return O32_EnumObjects(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentObject( HDC hdc, UINT arg2)
{
    dprintf(("GDI32: GetCurrentObject %x %x", hdc, arg2));
    return (HANDLE)O32_GetCurrentObject(hdc, arg2);
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
BOOL WIN32API UnrealizeObject( HGDIOBJ hObject)
{
    dprintf(("GDI32: UnrealizeObject %x", hObject));
    return O32_UnrealizeObject(hObject);
}
//******************************************************************************
//******************************************************************************
