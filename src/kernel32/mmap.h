/* $Id: mmap.h,v 1.6 1999-08-24 18:46:40 sandervl Exp $ */

/*
 * Memory mapped class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __MMAP_H__
#define __MMAP_H__

#include <vmutex.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE   4096
#endif

class Win32MemMap
{
public:
   Win32MemMap(HFILE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName);
  ~Win32MemMap();

   BOOL   Init(HANDLE hMemMap);
   BOOL   flushView(LPVOID lpvBase, ULONG cbFlush);
   LPVOID mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess);
   BOOL   unmapViewOfFile();

   HFILE  getMapHandle()                 { return hMemMap; };
   LPSTR  getMemName()                   { return lpszMapName; };
   DWORD  getProtFlags()                 { return mProtFlags; };

   void   AddRef()                       { ++referenced; };
   void   Release()                      { if(--referenced == 0) delete this; };

static Win32MemMap *findMap(LPSTR lpszName);
static Win32MemMap *findMap(ULONG address);

protected:
   HFILE  hMemMap, hMemFile;
   ULONG  mSize;
   ULONG  mProtFlags;
   ULONG  mMapAccess;
   LPSTR  lpszMapName;
   void  *pMapping;
   BOOL   fMapped;

   ULONG  referenced;

   VMutex mapMutex;

private:
   static Win32MemMap *memmaps;
	  Win32MemMap *next;
};

#endif //__MMAP_H__
