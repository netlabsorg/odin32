/* $Id: objhandle.h,v 1.1 2000-06-14 13:19:44 sandervl Exp $ */
/*
 * Win32 Handle Management Code for OS/2 (GDI32)
 *
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OBJHANDLE_H__
#define __OBJHANDLE_H__

#define MAX_OBJECT_HANDLES	 	1024
#define OBJHANDLE_MAGIC_MASK  		0x00FFFFFF
#define HANDLE_OBJ_ERROR                -1

typedef enum {
  GDIOBJ_REGION  = 0xD1,
  GDIOBJ_BITMAP  = 0xD2,
  GDIOBJ_BRUSH   = 0xD3, 
  GDIOBJ_PALETTE = 0xD4, 
  GDIOBJ_FONT    = 0xD5,
  GDIOBJ_ERROR   = 0xFF
} ObjectType;

#define GET_OBJTYPE(a)			(a >> 24)
#define MAKE_HANDLE(a)			(a << 24)
//...

BOOL  ObjAllocateHandle(HANDLE *hObject, DWORD dwUserData, ObjectType type);
void  ObjFreeHandle(HANDLE hObject);
DWORD ObjGetHandleData(HANDLE hObject);

#define ObjWinToOS2Region(a)	ObjGetHandleData(a)

ObjectType ObjGetHandleType(HANDLE hObject);

#endif //__OBJHANDLE_H__
