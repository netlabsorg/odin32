/* $Id: winimagebase.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

/*
 * Win32 PE Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINIMAGEBASE_H__
#define __WINIMAGEBASE_H__

#include <peexe.h>

#ifdef DEBUG
#define MAGIC_WINIMAGE          0x11223344
#endif

#ifndef CCHMAXPATH
#define CCHMAXPATH 260
#endif

#define LANG_GETFIRST           0x80000000

class Win32Resource;

class Win32ImageBase
{
#ifdef DEBUG
protected:
        DWORD magic;
public:
        void checkObject()
        {
                if(magic != MAGIC_WINIMAGE) {
                        eprintf(("Corrupt this pointer %X %X!!", this, magic));
                        DebugInt3();
                }
        };
#endif

public:
         // Constructors and destructors
         Win32ImageBase(HINSTANCE hInstance);
virtual ~Win32ImageBase();

        ULONG     getError()          { return errorState; };
        HINSTANCE getInstanceHandle() { return hinstance; };

virtual void setFullPath(char *name);
        char *getFullPath()           { return fullpath; };

	char *getModuleName()	      { return szModule; };

virtual HRSRC findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST) = 0;
        HRSRC findResourceW(LPWSTR lpszName, LPWSTR lpszType, ULONG lang = LANG_GETFIRST);
virtual ULONG getResourceSizeA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST) = 0;
        ULONG getResourceSizeW(LPCWSTR lpszName, LPWSTR lpszType, ULONG lang = LANG_GETFIRST);

virtual ULONG getVersionSize() = 0;
virtual BOOL  getVersionStruct(char *verstruct, ULONG bufLength) = 0;

static  BOOL  isPEImage(char *szFileName);

	void  setEntryPoint(ULONG startAddress) { entryPoint = startAddress; };

        void  setTLSAddress(LPVOID dwTlsAddress)     	{ tlsAddress = dwTlsAddress; };
        void  setTLSIndexAddr(LPDWORD dwTlsIndexAddr)	{ tlsIndexAddr = dwTlsIndexAddr; };
        void  setTLSInitSize(ULONG dwTlsSize)		{ tlsInitSize = dwTlsSize; };
        void  setTLSTotalSize(ULONG dwTlsSize)		{ tlsTotalSize = dwTlsSize; };
        void  setTLSCallBackAddr(PIMAGE_TLS_CALLBACK *dwTlsCallBackAddr)	
	{
	   tlsCallBackAddr = dwTlsCallBackAddr;
	};

	void  tlsAttachThread();	//setup TLS structures for new thread
	void  tlsDetachThread();	//destroy TLS structures

virtual BOOL  isDll() = 0;

protected:
	void tlsAlloc();		//Allocate TLS index for this module
	void tlsDelete();		//Destroy TLS index for this module

        Win32Resource        *winres;

        ULONG                 errorState, entryPoint;

        char                 *fullpath;
	char      	      szModule[CCHMAXPATH];
	char                  szFileName[CCHMAXPATH];

        HINSTANCE             hinstance;

  	LPVOID 	              tlsAddress;		//address of TLS data
  	LPDWORD		      tlsIndexAddr;		//address of DWORD that receives the TLS index
  	ULONG 		      tlsInitSize;		//size of initialized TLS memory block
  	ULONG 		      tlsTotalSize;		//size of TLS memory block
  	PIMAGE_TLS_CALLBACK  *tlsCallBackAddr;	//ptr to TLS callback array
	ULONG                 tlsIndex;		//module TLS index

private:

        friend class Win32Resource;
        friend ULONG SYSTEM GetVersionSize(char *modname);
};

//SvL: This structure is placed at the end of the first page of the image (header 
//     page), so we can determine the Win32Image pointer from a HINSTANCE variable
//     (which is actually the address of the win32 module)
typedef struct 
{
  Win32ImageBase *image;
  ULONG           magic; 
} WINIMAGE_LOOKUP;

#define WINIMAGE_LOOKUPADDR(a)	(WINIMAGE_LOOKUP *)((ULONG)a + PAGE_SIZE - sizeof(WINIMAGE_LOOKUP))

#endif //__WINIMAGEBASE_H__
