/* $Id: objhandle.cpp,v 1.23 2002-07-15 12:44:27 sandervl Exp $ */
/*
 * Win32 Handle Management Code for OS/2
 *
 *
 * Copyright 2000-2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: The table should be dynamically increased when necessary
 *       This is just a quick and dirty implementation
 *
 *       System objects can't be deleted (TODO: any others?? (fonts?))!!!!)
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
  DWORD      dwUserData;
  DWORD      dwGDI32Data;
  DWORD      dwFlags;
  DWORD      dwType;
} GdiObject;

static GdiObject *objHandleTable  = NULL;
static ULONG      lowestFreeIndex = 1;
static VMutex     objTableMutex;

//******************************************************************************
//******************************************************************************
BOOL WIN32API ObjAllocateHandle(HANDLE *hObject, DWORD dwUserData, DWORD dwType)
{
    objTableMutex.enter();
    if(objHandleTable == NULL) {
        objHandleTable = (GdiObject *)malloc(MAX_OBJECT_HANDLES*sizeof(GdiObject));
        if(objHandleTable == NULL) {
            DebugInt3();
        }
    }

    if(lowestFreeIndex == -1) {
        //oops, out of handles
        objTableMutex.leave();
        dprintf(("ERROR: GDI: ObjAllocateHandle OUT OF GDI OBJECT HANDLES!!"));
        DebugInt3();
        return FALSE;
    }
    if(objHandleTable[0].dwType == HNDL_NONE) {
        //first handle can never be used
        objHandleTable[0].dwType     = HNDL_INVALID;
        objHandleTable[0].dwUserData = -1;
    }
    *hObject = lowestFreeIndex;
    *hObject = MAKE_HANDLE(*hObject);
    objHandleTable[lowestFreeIndex].dwUserData  = dwUserData;
    objHandleTable[lowestFreeIndex].dwType      = dwType;
    objHandleTable[lowestFreeIndex].dwGDI32Data = 0;
    objHandleTable[lowestFreeIndex].dwFlags     = 0;
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
BOOL WIN32API ObjDeleteHandle(HANDLE hObject, DWORD dwType)
{
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES) {
        objTableMutex.enter();
        if(!(objHandleTable[hObject].dwFlags & OBJHANDLE_FLAG_NODELETE)) 
        {
            objHandleTable[hObject].dwUserData = 0;
            objHandleTable[hObject].dwType     = HNDL_NONE;
            if(lowestFreeIndex == -1 || hObject < lowestFreeIndex)
                lowestFreeIndex = hObject;
        }
        else {
            dprintf(("ObjDeleteHandle: unable to delete system object %x", MAKE_HANDLE(hObject)));
        }
        objTableMutex.leave();
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ObjQueryHandleData(HANDLE hObject, DWORD dwType)
{
    DWORD dwUserData = HANDLE_INVALID_DATA;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && 
       ((dwType == HNDL_ANY && objHandleTable[hObject].dwType != HNDL_NONE) || 
       dwType == objHandleTable[hObject].dwType)) 
    {
        dwUserData = objHandleTable[hObject].dwUserData;
    }
    objTableMutex.leave();
    return dwUserData;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ObjSetHandleData(HANDLE hObject, DWORD dwType, DWORD dwUserData)
{
    BOOL fSuccess = FALSE;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && 
       ((dwType == HNDL_ANY && objHandleTable[hObject].dwType != HNDL_NONE) || 
       dwType == objHandleTable[hObject].dwType)) 
    {
        objHandleTable[hObject].dwUserData = dwUserData;
        fSuccess = TRUE;
    }
    objTableMutex.leave();
    return fSuccess;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ObjQueryHandleGDI32Data(HANDLE hObject, DWORD dwType)
{
    DWORD dwGDI32Data = HANDLE_INVALID_DATA;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && 
       ((dwType == HNDL_ANY && objHandleTable[hObject].dwType != HNDL_NONE) || 
       dwType == objHandleTable[hObject].dwType)) 
    {
        dwGDI32Data = objHandleTable[hObject].dwGDI32Data;
    }
    objTableMutex.leave();
    return dwGDI32Data;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ObjSetHandleGDI32Data(HANDLE hObject, DWORD dwType, DWORD dwGDI32Data)
{
    BOOL fSuccess = FALSE;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && 
       ((dwType == HNDL_ANY && objHandleTable[hObject].dwType != HNDL_NONE) || 
       dwType == objHandleTable[hObject].dwType)) 
    {
        objHandleTable[hObject].dwGDI32Data = dwGDI32Data;
        fSuccess = TRUE;
    }
    objTableMutex.leave();
    return fSuccess;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ObjQueryHandleFlags(OBJHANDLE hObject)
{
    DWORD dwFlags = HANDLE_INVALID_DATA;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && objHandleTable[hObject].dwType != HNDL_NONE) 
    {
        dwFlags = objHandleTable[hObject].dwFlags;
    }
    objTableMutex.leave();
    return dwFlags;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ObjSetHandleFlag(HANDLE hObject, DWORD dwFlag, BOOL fSet)
{
    BOOL fSuccess = FALSE;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && objHandleTable[hObject].dwType != HNDL_NONE) {
        if(fSet) {
             objHandleTable[hObject].dwFlags |= dwFlag;
        }
        else objHandleTable[hObject].dwFlags &= ~dwFlag;

        dprintf(("ObjSetHandleFlag %x -> %x", MAKE_HANDLE(hObject), dwFlag));

        fSuccess = TRUE;
    }
    objTableMutex.leave();
    return fSuccess;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ObjQueryHandleType(HANDLE hObject)
{
    DWORD objtype = 0;

    objTableMutex.enter();
    hObject &= OBJHANDLE_MAGIC_MASK;
    if(hObject < MAX_OBJECT_HANDLES && objHandleTable[hObject].dwType != HNDL_NONE) {
        objtype = objHandleTable[hObject].dwType;
    }
    objTableMutex.leave();
    return objtype;
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

    handleType = GetObjectType(hObj);
    dprintf2(("GDI32: SelectObject %x %x type %s", hdc, hObj, DbgGetGDITypeName(handleType)));
    if(handleType == HNDL_REGION) {
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
    if(rc != 0 && GetObjectType(rc) == OBJ_BITMAP && DIBSection::getSection != NULL)
    {
        DIBSection *dsect = DIBSection::findObj(rc);
        if(dsect)
        {
            dsect->UnSelectDIBObject();
        }
    }
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
DWORD WIN32API GetObjectType( HGDIOBJ hObj)
{
    DWORD objtype = ObjQueryHandleType(hObj);

    switch(objtype) {
    case HNDL_PEN:
        objtype = OBJ_PEN;
        break;
    case HNDL_BRUSH:
        objtype = OBJ_BRUSH;
        break;
    case HNDL_DC:
        objtype = OBJ_DC;
        break;
    case HNDL_METADC:
        objtype = OBJ_METADC;
        break;
    case HNDL_PALETTE:
        objtype = OBJ_PAL;
        break;
    case HNDL_FONT:
        objtype = OBJ_FONT;
        break;
    case HNDL_BITMAP:
    case HNDL_DIBSECTION:
        objtype = OBJ_BITMAP;
        break;
    case HNDL_REGION:
        objtype = OBJ_REGION;
        break;
    case HNDL_METAFILE:
        objtype = OBJ_METAFILE;
        break;
    case HNDL_ENHMETAFILE:
        objtype = OBJ_ENHMETAFILE;
        break;
    case HNDL_MEMDC:
        objtype = OBJ_MEMDC;
        break;
    case HNDL_EXTPEN:
        objtype = OBJ_EXTPEN;
        break;
    case HNDL_ENHMETADC:
        objtype = OBJ_ENHMETADC;
        break;
    default:
        objtype = 0;
        break;
    }
    dprintf2(("GDI32: GetObjectType %x objtype %d (%s)", hObj, objtype, DbgGetGDITypeName(objtype)));
    return objtype;
}
//******************************************************************************
//TODO: System objects can't be deleted (TODO: any others?? (fonts?))!!!!)
//******************************************************************************
BOOL WIN32API DeleteObject(HANDLE hObj)
{
    DWORD objtype;

    dprintf(("GDI32: DeleteObject %x", hObj));

    STATS_DeleteObject(hObj, objtype);

    if(ObjQueryHandleType(hObj) == HNDL_REGION)
    {
        OSLibDeleteRegion(ObjQueryHandleData(hObj, HNDL_REGION));
        ObjDeleteHandle(hObj, HNDL_REGION);
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
