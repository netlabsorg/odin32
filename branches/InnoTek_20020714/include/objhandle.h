/*
 * Win32 Handle Management Code for OS/2 (GDI32)
 *
 */
#ifndef __OBJHANDLE_H__
#define __OBJHANDLE_H__

#ifdef NEW_GDIHANDLES

#define MAX_OBJECT_HANDLES          	1024
#define HANDLE_OBJ_ERROR            	-1
#define HANDLE_INVALID_DATA             0

#define OBJHANDLE_MAGIC_MASK        	0x00FFFFFF
#define MAKE_HANDLE(a)          	(a | 0x80000000)
#define OBJHANDLE_MAGIC(a)      	(a & ~0x80000000)

#define OBJHANDLE_FLAG_NODELETE     		0x01	//system handle; do not delete
#define OBJHANDLE_FLAG_BMP_SCREEN_COMPATIBLE  	0x02	//Bitmap with screen properties (bpp)
#define OBJHANDLE_FLAG_BMP_RGB565	  	0x04	//RGB565 bitmap
#define OBJHANDLE_FLAG_BMP_DIBSECTION  		0x08	//DIB section handle


#define HNDL_NONE                       0x00
#define HNDL_PEN          		0x01
#define HNDL_BRUSH        		0x02
#define HNDL_DC           		0x03
#define HNDL_METADC       		0x04
#define HNDL_PALETTE      		0x05
#define HNDL_FONT         		0x06
#define HNDL_BITMAP       		0x07
#define HNDL_DIBSECTION                 0x08
#define HNDL_REGION       		0x09
#define HNDL_METAFILE     		0x0A
#define HNDL_ENHMETAFILE                0x0B
#define HNDL_MEMDC        		0x0C
#define HNDL_EXTPEN       		0x0D
#define HNDL_ENHMETADC    		0x0E
#define HNDL_MENU 	  		0x0F
#define HNDL_ACCEL	  		0x10
#define HNDL_CURSORICON  		0x11
#define HNDL_DDELP     			0x12

#define HNDL_MAX                        0x12
#define HNDL_INVALID                    0xFE
#define HNDL_ANY                        0xFF

//...

typedef ULONG OBJHANDLE;

BOOL   WIN32API ObjAllocateHandle(OBJHANDLE *hObject, ULONG dwUserData, ULONG dwType);
BOOL   WIN32API ObjDeleteHandle(OBJHANDLE hObject, ULONG dwType);
ULONG  WIN32API ObjQueryHandleData(OBJHANDLE hObject, ULONG dwType);
BOOL   WIN32API ObjSetHandleData(OBJHANDLE hObject, ULONG dwType, ULONG dwUserData);
ULONG  WIN32API ObjQueryHandleGDI32Data(OBJHANDLE hObject, ULONG dwType);
BOOL   WIN32API ObjSetHandleGDI32Data(OBJHANDLE hObject, ULONG dwType, ULONG dwGDI32Data);
ULONG  WIN32API ObjQueryHandleType(OBJHANDLE hObject);
ULONG  WIN32API ObjQueryHandleFlags(OBJHANDLE hObject);
BOOL   WIN32API ObjSetHandleFlag(OBJHANDLE hObject, ULONG dwFlag, BOOL fSet);

#define ObjWinToOS2Region(a)    ObjQueryHandleData(a, HNDL_REGION)

#else
#define MAX_OBJECT_HANDLES          1024
#define OBJHANDLE_MAGIC_MASK        0x00FFFFFF
#define HANDLE_OBJ_ERROR            -1

typedef enum {
  GDIOBJ_NONE    = 0x00,
  GDIOBJ_REGION  = 0xD1,
  GDIOBJ_BITMAP  = 0xD2,
  GDIOBJ_BRUSH   = 0xD3,
  GDIOBJ_PALETTE = 0xD4,
  GDIOBJ_FONT    = 0xD5,
  USEROBJ_MENU   = 0xD6,
  USEROBJ_ACCEL  = 0xD7,
  USEROBJ_CURSORICON = 0xD8,
  GDIOBJ_OPEN32  = 0xE0,
  GDIOBJ_INVALID = 0xFF
//...
} ObjectType;

#define MAKE_HANDLE(a)          (a << 24)
#define OBJHANDLE_MAGIC(a)      (a >> 24)

//...

BOOL  WIN32API ObjAllocateHandle(HANDLE *hObject, DWORD dwUserData, ObjectType type);
void  WIN32API ObjFreeHandle(HANDLE hObject);
DWORD WIN32API ObjGetHandleData(HANDLE hObject, ObjectType type);

#define ObjWinToOS2Region(a)    ObjGetHandleData(a, GDIOBJ_REGION)

ObjectType WIN32API ObjGetHandleType(HANDLE hObject);

#endif
#endif //__OBJHANDLE_H__
