/* $Id: compobj.h,v 1.1 1999-05-24 20:19:09 ktk Exp $ */

#ifndef __WINE_COMPOBJ_H
#define __WINE_COMPOBJ_H

/* All private prototye functions used by OLE will be added to this header file */

#include "objbase.h"

/* This function initialize the Running Object Table */
HRESULT WINAPI RunningObjectTableImpl_Initialize();

/* This function uninitialize the Running Object Table */
HRESULT WINAPI RunningObjectTableImpl_UnInitialize();

/* This function decompose a String path to a String Table containing all the elements ("\" or "subDirectory" or "Directoy" or "FileName") of the path */
int WINAPI FileMonikerImpl_DecomposePath(LPOLESTR str, LPOLESTR** stringTable);

#endif /* __WINE_COMPOBJ_H */
