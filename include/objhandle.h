/* $Id: objhandle.h,v 1.3 2000-12-09 14:43:35 sandervl Exp $ */
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

#define MAX_OBJECT_HANDLES      1024
#define OBJHANDLE_MAGIC_MASK        0x00FFFFFF
#define HANDLE_OBJ_ERROR                -1

typedef enum {
  GDIOBJ_NONE    = 0x00,
  GDIOBJ_REGION  = 0xD1,
  GDIOBJ_BITMAP  = 0xD2,
  GDIOBJ_BRUSH   = 0xD3,
  GDIOBJ_PALETTE = 0xD4,
  GDIOBJ_FONT    = 0xD5,
  USEROBJ_MENU   = 0xD6,
  USEROBJ_ACCEL  = 0xD7
//...
} ObjectType;

#define MAKE_HANDLE(a)          (a << 24)
#define OBJHANDLE_MAGIC(a)      (a >> 24)

//...

BOOL  ObjAllocateHandle(HANDLE *hObject, DWORD dwUserData, ObjectType type);
void  ObjFreeHandle(HANDLE hObject);
DWORD ObjGetHandleData(HANDLE hObject, ObjectType type);

#define ObjWinToOS2Region(a)    ObjGetHandleData(a, GDIOBJ_REGION)

ObjectType ObjGetHandleType(HANDLE hObject);

#endif //__OBJHANDLE_H__
