/* $Id: winimagebase.h,v 1.24 2004-01-15 10:39:11 sandervl Exp $ */

/*
 * Win32 PE Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINIMAGEBASE_H__
#define __WINIMAGEBASE_H__

#include <peexe.h>
#include "queue.h"
#include <odinpe.h>
#ifdef OS2_INCLUDED
#include <winconst.h>
#else
#include <win\winnls.h>
#endif

#define MAGIC_WINIMAGE          0x11223344

#ifndef CCHMAXPATH
#define CCHMAXPATH 260
#endif

#define ID_GETFIRST             0xF0000000

class Win32Resource;
class Win32DllBase;


class Win32ImageBase
{
protected:
        DWORD magic;
public:
    void checkObject()
    {
        if (magic != MAGIC_WINIMAGE) {
            eprintf(("Corrupt this pointer %X %X!!", this, magic));
            DebugInt3();
        }
    };

public:
         // Constructors and destructors
         Win32ImageBase(HINSTANCE hInstance);
virtual ~Win32ImageBase();

    ULONG     getError()          { return errorState; };
    HINSTANCE getInstanceHandle() { return hinstance; };

//Returns required OS version for this image
virtual ULONG getVersion();

virtual void  setFullPath(char *name);
    char *getFullPath()           { return fullpath; };
    char *getModuleName()         { return szModule; };

        //findResource returns the pointer of the resource's IMAGE_RESOURCE_DATA_ENTRY structure
    HRSRC findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    HRSRC findResourceW(LPWSTR lpszName, LPWSTR lpszType, ULONG lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));

    ULONG getResourceSizeA(LPSTR lpszName, LPSTR lpszType, ULONG lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    ULONG getResourceSizeW(LPWSTR lpszName, LPWSTR lpszType, ULONG lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    BOOL  enumResourceNamesA(HMODULE hmod, LPCTSTR  lpszType, ENUMRESNAMEPROCA lpEnumFunc, LONG lParam);
    BOOL  enumResourceNamesW(HMODULE hmod, LPCWSTR  lpszType, ENUMRESNAMEPROCW lpEnumFunc, LONG lParam);
    BOOL  enumResourceTypesA(HMODULE hmod, ENUMRESTYPEPROCA lpEnumFunc,
                             LONG lParam);
    BOOL  enumResourceTypesW(HMODULE hmod, ENUMRESTYPEPROCW lpEnumFunc,
                             LONG lParam);
    BOOL  enumResourceLanguagesA(HMODULE hmod, LPCSTR lpType, LPCSTR lpName,
                                 ENUMRESLANGPROCA lpEnumFunc, LONG lParam);
    BOOL  enumResourceLanguagesW(HMODULE hmod, LPCWSTR lpType, LPCWSTR lpName,
                                 ENUMRESLANGPROCW lpEnumFunc, LONG lParam);

    ULONG getVersionSize();
    BOOL  getVersionStruct(char *verstruct, ULONG bufLength);

    //Returns pointer to data of resource handle
    char *getResourceAddr(HRSRC hResource);
    ULONG getResourceSize(HRSRC hResource);

//returns ERROR_SUCCESS or error code (Characteristics will contain
//the Characteristics member of the file header structure)
static  ULONG isPEImage(char *szFileName, DWORD *Characteristics, DWORD *subsystem, DWORD *fNEExe = NULL);
static  BOOL  findDll(const char *pszFileName, char *pszFullName,
                      int cchFullName, const char *pszAltPath = NULL);

    void  setEntryPoint(ULONG startAddress)         { entryPoint = startAddress; };

    void  setTLSAddress(LPVOID dwTlsAddress)        { tlsAddress = dwTlsAddress; };
    void  setTLSIndexAddr(LPDWORD dwTlsIndexAddr)   { tlsIndexAddr = dwTlsIndexAddr; };
    void  setTLSInitSize(ULONG dwTlsSize)           { tlsInitSize = dwTlsSize; };
    void  setTLSTotalSize(ULONG dwTlsSize)          { tlsTotalSize = dwTlsSize; };
    void  setTLSCallBackAddr(PIMAGE_TLS_CALLBACK *dwTlsCallBackAddr)
    {
       tlsCallBackAddr = dwTlsCallBackAddr;
    };

    void  tlsAttachThread();    //setup TLS structures for new thread
    void  tlsDetachThread();    //destroy TLS structures

virtual BOOL  insideModule(ULONG address);
virtual BOOL  insideModuleCode(ULONG address);

virtual ULONG getApi(char *name);
virtual ULONG getApi(int ordinal);

virtual ULONG setApi(char *name, ULONG pfnNewProc);
virtual ULONG setApi(int ordinal, ULONG pfnNewProc);

virtual ULONG getImageSize();

virtual BOOL  isDll()     = 0;
        BOOL  isPEImage() { return fIsPEImage; };

static Win32ImageBase * findModule(HMODULE hModule);
    BOOL  matchModName(const char *pszFilename) const;

    /* @cat Depencies */
    //Add image to dependency list of this image
    void  addDependency(Win32DllBase *dll);
    BOOL  dependsOn(Win32DllBase *dll);

protected:
    void tlsAlloc();        //Allocate TLS index for this module
    void tlsDelete();       //Destroy TLS index for this module

    ULONG findApi(char *pszName, ULONG ulOrdinal, ULONG pfnNewProc = NULL);
    ULONG findForwarder(ULONG virtaddr, char *apiname, ULONG ordinal);
    virtual void *getPointerFromRVA(ULONG ulRVA, BOOL fOverride = FALSE);
    void *        getPointerFromRVA(const void *pvRVA)  { return getPointerFromRVA((ULONG)pvRVA); }
    virtual ULONG getRVAFromPointer(void *pv, BOOL fOverride = FALSE);

    ULONG                   errorState,
                            entryPoint;

    char *                  fullpath;
    char                    szModule[CCHMAXPATH];
    char                    szFileName[CCHMAXPATH];

    HINSTANCE               hinstance;
    BOOL                    fIsPEImage;

    LPVOID                  tlsAddress;         //address of TLS data
    LPDWORD                 tlsIndexAddr;       //address of DWORD that receives the TLS index
    ULONG                   tlsInitSize;        //size of initialized TLS memory block
    ULONG                   tlsTotalSize;       //size of TLS memory block
    PIMAGE_TLS_CALLBACK *   tlsCallBackAddr;    //ptr to TLS callback array
    ULONG                   tlsIndex;           //module TLS index

    PIMAGE_RESOURCE_DIRECTORY  getResSubDirW(PIMAGE_RESOURCE_DIRECTORY pResDir, LPCWSTR lpszName);
    PIMAGE_RESOURCE_DIRECTORY  getResSubDirA(PIMAGE_RESOURCE_DIRECTORY pResDir, LPCTSTR lpszName);

    PIMAGE_RESOURCE_DATA_ENTRY getResDataLang(PIMAGE_RESOURCE_DIRECTORY pResDir, ULONG language, BOOL fGetDefault = FALSE);

    HRSRC                      getResourceLang(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch);
    HRSRC                      getResourceLangEx(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch,
                                                 DWORD lang);

    PIMAGE_RESOURCE_DIRECTORY   pResRootDir;

    PIMAGE_EXPORT_DIRECTORY     pExportDir;
    PIMAGE_OPTIONAL_HEADER      poh;

    //substracted from RVA data offsets
    ULONG                   ulRVAResourceSection;

    //linked list of dlls loaded on behalf of this executable image (dll or exe)
    Queue                   loadedDlls;

private:
    friend class Win32Resource;
    friend ULONG SYSTEM GetVersionSize(char *modname);
};

#endif //__WINIMAGEBASE_H__
