/* $Id: winimagepe2lx.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

/*
 * Win32 PE2LX Image base class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINIMAGEPE2LX_H__
#define __WINIMAGEPE2LX_H__

#include <winimagebase.h>

#define NO_NAMETABLE            0x77777777
#define NO_LOOKUPTABLE          0x888888
#define GET_CONSOLE(a)          (a >> 24)
#define SET_CONSOLE(a)          (a << 24)

#define RESID_CONVERTEDNAMES    63*1024

#pragma pack(1)
typedef struct {
  int    id;
  char   name[1];
} NameId;
#pragma pack()

class Win32Pe2LxImage : public virtual Win32ImageBase
{
public:
         Win32Pe2LxImage(HINSTANCE hinstance, int NameTableId, int Win32TableId);
virtual ~Win32Pe2LxImage();

virtual HRSRC findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST);
virtual ULONG getResourceSizeA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST);

        void  setVersionId(int id) { VersionId = id;   };

virtual ULONG getVersionSize();
virtual BOOL  getVersionStruct(char *verstruct, ULONG bufLength);

protected:
        int   getWin32ResourceId(int id);
        int   convertNameId(char *lpszName);

        int   getVersionId()       { return VersionId; };

        int                   NameTableId;
        int                   Win32TableId;
        int                   VersionId;

        ULONG                *Win32Table;
        NameId               *NameTable;

private:
};

#endif //__WINIMAGEPE2LX_H__

