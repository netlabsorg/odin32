/* $Id: mmap.h,v 1.31 2004-01-11 11:52:18 sandervl Exp $ */

/*
 * Memory mapped class
 *
 * Copyright 1999-2003 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __MMAP_H__
#define __MMAP_H__

#include <vmutex.h>
#include "heapshared.h"
#include "asmutil.h"


#ifndef PAGE_SIZE
#define PAGE_SIZE       4096
#endif
#ifndef PAGE_SHIFT
#define PAGE_SHIFT  12
#endif

#define MEMMAP_CRITSECTION_NAME	"\\SEM32\\ODIN_MMAP.SEM"

//commit 16 pages at once when the app accesses it
#define NRPAGES_TOCOMMIT        16

#define MEMMAP_ACCESS_INVALID           0
#define MEMMAP_ACCESS_READ      	1
#define MEMMAP_ACCESS_WRITE     	2
#define MEMMAP_ACCESS_EXECUTE   	4
#define MEMMAP_ACCESS_COPYONWRITE	8

#define MMAP_FLUSHVIEW_ALL		0xFFFFFFFF

#define MMAP_MAX_FILENAME_LENGTH        260

typedef enum
{
    PAGEVIEW_READONLY,
    PAGEVIEW_VIEW,
    PAGEVIEW_GUARD
} PAGEVIEW;

#ifndef _DEF_PFNEXCEPTIONNOTIFY
#define _DEF_PFNEXCEPTIONNOTIFY
typedef BOOL (WIN32API * PFNEXCEPTIONNOTIFY)(LPVOID lpBase, ULONG offset, BOOL fWriteAccess, DWORD dwSize, DWORD dwUserData);
#endif

class Win32MemMapView;
class Win32PeLdrImage;

//******************************************************************************
//Memory mapping class
//******************************************************************************
class Win32MemMap
{
public:
   Win32MemMap(HANDLE hfile, ULONG size, ULONG fdwProtect, LPSTR lpszName);
   //Use by PE loader image class only:
   Win32MemMap(Win32PeLdrImage *pImage, ULONG lpImageMem, ULONG size);
virtual ~Win32MemMap();

virtual BOOL   Init(DWORD aMSize=0);
virtual BOOL   flushView(ULONG viewaddr, ULONG offset, ULONG cbFlush);
virtual LPVOID mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess);
virtual BOOL   unmapViewOfFile(LPVOID addr);

        BOOL   updateViewPages(ULONG offset, ULONG size, PAGEVIEW flags);
        BOOL   allocateMap();

   HANDLE getFileHandle()                { return hMemFile; };
   LPSTR  getMemName()                   { return lpszMapName; };
   DWORD  getMapSize()                   { return mSize; };
   DWORD  getProtFlags()                 { return mProtFlags; };
   void   setProtFlags(DWORD dwNewFlags) { mProtFlags = dwNewFlags; };
   LPVOID getMappingAddr()               { return pMapping; };
   DWORD  getProcessId()                 { return mProcessId;};
Win32PeLdrImage *getImage()              { return image; };

   BOOL   isImageMap()                   { return image != NULL; };

   void   AddRef()                       { ++referenced; };
   int    Release();

   void   AddView()                      { ++nrMappings; };
   void   RemoveView()                   { --nrMappings; };


   void   markDirtyPages(int startpage, int nrpages);
   void   clearDirtyPages(int startpage, int nrpages);
   BOOL   isDirtyPage(int pagenr)        { return test_bit(pagenr, pWriteBitmap) != 0; };

virtual BOOL   invalidatePages(ULONG offset, ULONG size);
virtual BOOL   commitPage(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess, int nrpages = NRPAGES_TOCOMMIT);
virtual BOOL   commitGuardPage(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess);
        BOOL   commitRange(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess, int nrpages);

static Win32MemMap *findMap(LPSTR lpszName);
static Win32MemMap *findMapByFile(HANDLE hFile);
static Win32MemMap *findMap(ULONG address);

//Should only be called in ExitProcess
static void deleteAll();

#ifdef __DEBUG_ALLOC__
    void *operator new(size_t size, const char *filename, size_t lineno)
    {
        return _smalloc(size);
    }
    void operator delete(void *location, const char *filename, size_t lineno)
    {
        free(location);
    }
#else
    void *operator new(size_t size)
    {
        return _smalloc(size);
    }
    void operator delete(void *location)
    {
        free(location);
    }
#endif

protected:
   HANDLE hMemFile;
   HANDLE hOrgMemFile;
   LPSTR  lpszFileName;
   ULONG  mSize;
   ULONG  mProtFlags;
   ULONG  mProcessId;
   ULONG  mMapAccess;
   LPSTR  lpszMapName;
   void  *pMapping;

   char  *pWriteBitmap;

   ULONG  nrMappings;

   ULONG  referenced;

   VMutex mapMutex;

   Win32PeLdrImage *image;

   Win32MemMapView *views;

private:
   static Win32MemMap *memmaps;
          Win32MemMap *next;
};
//******************************************************************************
//Duplicate memory mapping class (duplicate map with different protection flags
//associated with an existing memory map)
//******************************************************************************
class Win32MemMapDup : public Win32MemMap
{
public:
            Win32MemMapDup(Win32MemMap *parent, HANDLE hFile, ULONG size, ULONG fdwProtect, LPSTR lpszName);
   virtual ~Win32MemMapDup();

virtual BOOL   Init(DWORD aMSize=0);
virtual BOOL   flushView(ULONG viewaddr, ULONG offset, ULONG cbFlush);
virtual LPVOID mapViewOfFile(ULONG size, ULONG offset, ULONG fdwAccess);
virtual BOOL   unmapViewOfFile(LPVOID addr);

virtual BOOL   invalidatePages(ULONG offset, ULONG size);
virtual BOOL   commitGuardPage(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess);
virtual BOOL   commitPage(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess, int nrpages = NRPAGES_TOCOMMIT);

protected:
            Win32MemMap *parent;

            HANDLE       hDupMemFile;
private:
};
//******************************************************************************
//Memory mapped file View Class
//******************************************************************************
class Win32MemMapView
{
public:
   Win32MemMapView(Win32MemMap *parent, ULONG offset, ULONG size, ULONG fdwAccess, Win32MemMap *owner = NULL);
  ~Win32MemMapView();

   BOOL   changePageFlags(ULONG offset, ULONG size, PAGEVIEW flags);

   DWORD  getAccessFlags()               { return mfAccess; };
   DWORD  getSize()                      { return mSize;    };
   ULONG  getOffset()                    { return mOffset;  };
   LPVOID getViewAddr()                  { return pMapView; };

   BOOL   everythingOk()                 { return errorState == 0; };

Win32MemMap *getParentMap()              { return mParentMap;};
Win32MemMap *getOwnerMap()               { return mOwnerMap; };

   DWORD  getProcessId()                 { return mProcessId;};

   void   markCOWPages(int startpage, int nrpages);
   BOOL   isCOWPage(int pagenr)          { return (pCOWBitmap) ? (test_bit(pagenr, pCOWBitmap) != 0) : FALSE; };

static void             deleteViews(Win32MemMap *map);
static Win32MemMap     *findMapByView(ULONG address, ULONG *offset = NULL,
                                      ULONG accessType = MEMMAP_ACCESS_READ);
static int              findViews(Win32MemMap *map, int nrViews, Win32MemMapView *viewarray[]);
static Win32MemMapView *findView(ULONG address);

#ifdef __DEBUG_ALLOC__
    void *operator new(size_t size, const char *filename, size_t lineno)
    {
        return _smalloc(size);
    }
    void operator delete(void *location, const char *filename, size_t lineno)
    {
        free(location);
    }
#else
    void *operator new(size_t size)
    {
        return _smalloc(size);
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

   char  *pCOWBitmap;

   //parent map object; memory map that contains the original memory map
   Win32MemMap *mParentMap;
   //owner map object (can be NULL); duplicate memory map that created this view
   Win32MemMap *mOwnerMap;

private:
   static Win32MemMapView *mapviews;
          Win32MemMapView *next;

   friend class Win32MemMap;
};
//******************************************************************************
//Notification memory mapped file Class
//******************************************************************************
class Win32MemMapNotify
{
public:
   Win32MemMapNotify(PFNEXCEPTIONNOTIFY pfnNotify, LPVOID lpViewaddr, ULONG size, ULONG dwUserData);
  ~Win32MemMapNotify();

   LPVOID getViewAddr()                  { return pMapView; };
   DWORD  getSize()                      { return mSize;    };
   DWORD  getUserData()                  { return dwUserData; };

   BOOL   notify(ULONG ulFaultAddr, ULONG offset, BOOL fWriteAccess);

static Win32MemMapNotify *findMapByView(ULONG address,
                                        ULONG *offset,
                                        ULONG accessType);
static Win32MemMapNotify *findView(ULONG address);


protected:
   ULONG  mSize, dwUserData;
   LPVOID pMapView;
   PFNEXCEPTIONNOTIFY pfnNotify;

private:
   static Win32MemMapNotify *mapviews;
          Win32MemMapNotify *next;
};
//******************************************************************************
//******************************************************************************

#pragma data_seg(_GLOBALDATA)
extern CRITICAL_SECTION_OS2 globalmapcritsect;
#pragma data_seg()

void InitializeMemMaps();
void FinalizeMemMaps();

#endif //__MMAP_H__
