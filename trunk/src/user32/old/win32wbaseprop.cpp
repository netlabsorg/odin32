/* $Id: win32wbaseprop.cpp,v 1.1 2001-05-11 08:36:28 sandervl Exp $ */
/*
 * Window properties
 *
 * Ported Wine code (win\property.c):
 * Copyright 1995, 1996 Alexandre Julliard
 *
 * TODO: Not thread safe!!
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <string.h>
#include <user32.h>
#include <heapstring.h>
#include <misc.h>
#include "win32wbase.h"

#define DBG_LOCALLOG    DBG_win32wbaseprop
#include "dbglocal.h"

/***********************************************************************
 *           PROP_FindProp
 */
PROPERTY *Win32BaseWindow::findWindowProperty(LPCSTR str)
{
  PROPERTY *prop;
  ATOM      atom;

    if (HIWORD(str))
    {
        atom = GlobalFindAtomA(str);
        for (prop = propertyList; prop; prop = prop->next)
        {
            if (HIWORD(prop->string))
            {
                 if (!lstrcmpiA( prop->string, str )) goto END;
            }
            else if (LOWORD(prop->string) == atom) goto END;
        }
    }
    else  /* atom */
    {
        atom = LOWORD(str);
        for (prop = propertyList; (prop); prop = prop->next)
        {
            if (HIWORD(prop->string))
            {
                 if (GlobalFindAtomA( prop->string ) == atom) goto END;
            }
            else if (LOWORD(prop->string) == atom) goto END;
        }
    }
    if(HIWORD(str)) {
         dprintf2(("Property %s for window %x NOT FOUND!", str, getWindowHandle()));
    }
    else dprintf2(("Property %x for window %x NOT FOUND!", str, getWindowHandle()));

    prop = NULL;
END:
    return prop;
}

/***********************************************************************
 *           GetPropA   (USER32.281)
 */
HANDLE Win32BaseWindow::getProp(LPCSTR str)
{
    PROPERTY *prop = findWindowProperty(str);

    if(HIWORD(str)) {
         dprintf2(("GetProp %x %s %x", getWindowHandle, str, prop ? prop->handle : 0));
    }
    else dprintf2(("GetProp %x %x %x", getWindowHandle, str, prop ? prop->handle : 0));

    return prop ? prop->handle : 0;
}

/***********************************************************************
 *           SetPropA   (USER32.497)
 */
BOOL Win32BaseWindow::setProp(LPCSTR str, HANDLE handle)
{
    PROPERTY *prop;

    if (HIWORD(str)) {
         dprintf2(("SetProp %x %s %x", getWindowHandle, str, handle));
    }
    else dprintf2(("SetProp %x %x %x", getWindowHandle, str, handle));

    if (!(prop = findWindowProperty(str)))
    {
        /* We need to create it */
        if (!(prop = (PROPERTY *)HeapAlloc(GetProcessHeap(), 0, sizeof(*prop) )))
        {
             dprintf(("setProp: malloc failed!!"));
             return FALSE;
        }
        if(HIWORD(str))
        {
             if (!(prop->string = HEAP_strdupA(GetProcessHeap(), 0, str)))
             {
                HeapFree( GetProcessHeap(), 0, prop );
                return FALSE;
             }
        }
        else prop->string = (char *)str;  //atom

        prop->next   = propertyList;
        propertyList = prop;
    }
    prop->handle = handle;
    return TRUE;
}

/***********************************************************************
 *           RemovePropA   (USER32.442)
 */
HANDLE Win32BaseWindow::removeProp(LPCSTR str)
{
    ATOM atom;
    HANDLE handle;
    PROPERTY **pprop, *prop;

    if (HIWORD(str)) {
         dprintf2(("RemoveProp %x %s", getWindowHandle(), str ));
    }
    else dprintf2(("RemoveProp %x %x", getWindowHandle(), str ));

    if (HIWORD(str))
    {
        atom = GlobalFindAtomA( str );
        for (pprop=(PROPERTY**)&propertyList; (*pprop); pprop = &(*pprop)->next)
        {
            if (HIWORD((*pprop)->string))
            {
                 if (!lstrcmpiA( (*pprop)->string, str )) break;
            }
            else if (LOWORD((*pprop)->string) == atom) break;
        }
    }
    else  /* atom */
    {
        atom = LOWORD(str);
        for (pprop=(PROPERTY**)&propertyList; (*pprop); pprop = &(*pprop)->next)
        {
            if (HIWORD((*pprop)->string))
            {
                 if (GlobalFindAtomA( (*pprop)->string ) == atom) break;
            }
            else if (LOWORD((*pprop)->string) == atom) break;
        }
    }
    if (!*pprop) return 0;

    prop   = *pprop;
    handle = prop->handle;
    *pprop = prop->next;
    if(HIWORD(prop->string))
        HeapFree(GetProcessHeap(), 0, prop->string);
    HeapFree( GetProcessHeap(), 0, prop );
    return handle;
}

/***********************************************************************
 *           PROPERTY_RemoveWindowProps
 *
 * Remove all properties of a window.
 */
void Win32BaseWindow::removeWindowProps()
{
    PROPERTY *prop, *next;

    for (prop = propertyList; (prop); prop = next)
    {
        next = prop->next;
        if(HIWORD(prop->string))
            HeapFree(GetProcessHeap(), 0, prop->string);
        HeapFree(GetProcessHeap(), 0, prop);
    }
    propertyList = NULL;
}

/***********************************************************************
 *           EnumPropsExA   (USER32.187)
 */
INT Win32BaseWindow::enumPropsExA(PROPENUMPROCEXA func, LPARAM lParam)
{
    PROPERTY *prop, *next;
    INT ret = -1;

    dprintf(("EnumPropsExA %x %x %x", getWindowHandle, func, lParam));

    for (prop = propertyList; (prop); prop = next)
    {
        /* Already get the next in case the callback */
        /* function removes the current property.    */
        next = prop->next;

        if(HIWORD(prop->string)) {
             dprintf2(("EnumPropsExA: Callback: handle=%08x str=%s", prop->handle, prop->string));
        }
        else dprintf2(("EnumPropsExA: Callback: handle=%08x str=%x", prop->handle, prop->string));

        ret = func( getWindowHandle(), prop->string, prop->handle, lParam );
        if (!ret) break;
    }
    return ret;
}


/***********************************************************************
 *           EnumPropsExW   (USER32.188)
 */
INT Win32BaseWindow::enumPropsExW(PROPENUMPROCEXW func, LPARAM lParam)
{
    PROPERTY *prop, *next;
    INT ret = -1;

    dprintf(("EnumPropsExW %x %x %x", getWindowHandle, func, lParam));

    for (prop = propertyList; (prop); prop = next)
    {
        /* Already get the next in case the callback */
        /* function removes the current property.    */
        next = prop->next;

        if(HIWORD(prop->string)) {
             dprintf2(("EnumPropsExW: Callback: handle=%08x str=%s", prop->handle, prop->string));
        }
        else dprintf2(("EnumPropsExW: Callback: handle=%08x str=%x", prop->handle, prop->string));

        if (HIWORD(prop->string))
        {
            LPWSTR str = HEAP_strdupAtoW( GetProcessHeap(), 0, prop->string );
            ret = func( getWindowHandle(), str, prop->handle, lParam );
            HeapFree( GetProcessHeap(), 0, str );
        }
        else
            ret = func( getWindowHandle(), (LPCWSTR)(UINT)LOWORD( prop->string ),
                        prop->handle, lParam );
        if (!ret) break;
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
