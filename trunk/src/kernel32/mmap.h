/* $Id: mmap.h,v 1.18 2000-12-12 23:57:16 sandervl Exp $ */

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
#include "heapshared.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE       4096
#endif
#ifndef PAGE_SHIFT
#define PAGE_SHIFT  12
#endif

//commit 4 pages at once when the app accesses it
#define NRPAGES_TOCOMMIT    16

#define MEMMAP_ACCESS_READ  1
#define MEMMAP_ACCESS_WRITE 2
#define MEMMAP_ACCESS_EXECUTE   4

class Win32MemMapView;
class Win32PeLdrImage;

//******************************************************************************
//******************************************************************************
class Win32MemMap
{
public:
   Win32MemMap(HFILE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName);
   //Use by PE loader image class only:
   Win32MemMap(Win32PeLdrImage *pImage, ULONG lpImageMem, ULONG size);
  ~Win32MemMap();

   BOOL   Init(HANDLE hMemMap);
   BOOL   flushView(ULONG offset, ULONG cbFlush);
   LPVOID mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess);
   BOOL   unmapViewOfFile(Win32MemMapView *view);

   HFILE  getMapHandle()                 { return hMemMap; };
   HFILE  getFileHandle()                { return hMemFile; };
   LPSTR  getMemName()                   { return lpszMapName; };
   DWORD  getProtFlags()                 { return mProtFlags; };
   LPVOID getMappingAddr()               { return pMapping; };
   DWORD  getProcessId()                 { return mProcessId;};
Win32PeLdrImage *getImage()              { return image; };

   BOOL   isImageMap()                   { return image != NULL; };

   void   AddRef()                       { ++referenced; };
   void   Release()                      { if(--referenced == 0) delete this; };

   void   close();
   BOOL   isClosed()                     { return fClosed; };

   BOOL   commitPage(ULONG offset, BOOL fWriteAccess, int nrpages = NRPAGES_TOCOMMIT);

static Win32MemMap *findMap(LPSTR lpszName);
static Win32MemMap *findMap(ULONG address);

//Should only be called in ExitProcess
static void deleteAll();

#ifdef __DEBUG_ALLOC__
    void *operator new(size_t size, const char *filename, size_t lineno)
    {
        return _umalloc(sharedHeap, size);
    }
    void operator delete(void *location, const char *filename, size_t lineno)
    {
        free(location);
    }
#else
    void *operator new(size_t size)
    {
        return _umalloc(sharedHeap, size);
    }
    void operator delete(void *location)
    {
        free(location);
    }
#endif

protected:
   HFILE  hMemMap, hMemFile;
   ULONG  mSize;
   ULONG  mProtFlags;
   ULONG  mProcessId;
   ULONG  mMapAccess;
   LPSTR  lpszMapName;
   void  *pMapping;
   BOOL   fClosed;  //handle is removed by CloseHandle

   ULONG  nrMappings;

   ULONG  referenced;

   VMutex mapMutex;

   Win32PeLdrImage *image;

private:
   static Win32MemMap *memmaps;
      Win32MemMap *next;
};
//******************************************************************************
//Memory mapped file View Class
//******************************************************************************
class Win32MemMapView
{
public:
   Win32MemMapView(Win32MemMap *map, ULONG offset, ULONG size, ULONG fdwAccess);
  ~Win32MemMapView();

   DWORD  getAccessFlags()               { return mfAccess; };
   DWORD  getSize()                      { return mSize;    };
   LPVOID getViewAddr()                  { return pMapView; };
   ULONG  getOffset()                    { return mOffset;  };

   BOOL   everythingOk()                 { return errorState == 0; };

Win32MemMap *getParentMap()              { return mParentMap;};
   DWORD  getProcessId()                 { return mProcessId;};

static void             deleteViews(Win32MemMap *map);
static Win32MemMap     *findMapByView(ULONG address, ULONG *offset, ULONG accessType, Win32MemMapView **pView=NULL);
static Win32MemMapView *findView(LPVOID address);

#ifdef __DEBUG_ALLOC__
    void *operator new(size_t size, const char *filename, size_t lineno)
    {
        return _umalloc(sharedHeap, size);
    }
    void operator delete(void *location, const char *filename, size_t lineno)
    {
        free(location);
    }
#else
    void *operator new(size_t size)
    {
        return _umalloc(sharedHeap, size);
    }
    void operator delete(void *location)
    {
        free(location);
    }
#endif

protected:
   ULONG  mSize, errorState;
   ULONG  mProcessId;
   ULONG  mfAccess, mOffset;
   void  *pMapView, *pShareViewAddr;

   Win32MemMap *mParentMap;

private:
   static Win32MemMapView *mapviews;
          Win32MemMapView *next;

   friend class Win32MemMap;
};
//******************************************************************************
//******************************************************************************

#endif //__MMAP_H__
