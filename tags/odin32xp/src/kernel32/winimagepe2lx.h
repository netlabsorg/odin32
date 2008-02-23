/* $Id: winimagepe2lx.h,v 1.10 2004-01-15 10:39:13 sandervl Exp $ */

/*
 * Win32 PE2LX Image base class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINIMAGEPE2LX_H__
#define __WINIMAGEPE2LX_H__

#include <winimagebase.h>


/**
 * Section struct - used to translate RVAs to pointers.
 */
typedef struct _Section
{
    ULONG ulRVA;            /* RVA of section. If not a PE section ~0UL. */
    ULONG cbVirtual;        /* Virtual size (the larger of the physical and virtual) of the section. */
    ULONG ulAddress;        /* Current load address of the section. */
} SECTION, *PSECTION;



/**
 * Base class for Pe2lx (and Win32k) dlls. There is currently no difference between
 * Pe2Lx and Win32k images, though the image on disk is different...
 * @shortdesc   Pe2Lx and Win32k base image class.
 * @author      knut st. osmundsen, Sander van Leeuwen
 * @approval    -
 */
class Win32Pe2LxImage : public virtual Win32ImageBase
{
public:
    /** @cat constructor and destructor */
    Win32Pe2LxImage(HINSTANCE hinstance, BOOL fWin32k);
    virtual         ~Win32Pe2LxImage();
    virtual DWORD   init();

    /** @cat Queries */
    /** Get the OS/2 module handle.
     * @returns OS/2 module handle. */
    HINSTANCE       getHMOD() const { return hmod; }

private:
    /** @cat constructor helpers */
    ULONG           getSections();
    ULONG           setSectionRVAs();
    ULONG           doResources();
    ULONG           doTLS();
    ULONG           doImports();
    Win32ImageBase *importsGetModule(const char *pszModName);
    Win32DllBase   *importsLoadModule(const char *pszModName);
    ULONG           importsByOrdinal(Win32ImageBase *pModule, unsigned uOrdinal, void **ppvAddr);
    ULONG           importsByName(Win32ImageBase *pModule, const char *pszSymName, void **ppvAddr);
    VOID            cleanup();

protected:
    /** @cat RVA -> pointer */
    /* these should be moved to winimagebase some day... */
    void *          getPointerFromRVA(ULONG ulRVA, BOOL fOverride = FALSE);
    ULONG           getRVAFromPointer(void *pv, BOOL fOverride = FALSE);
    PVOID           getPointerFromPointer(PVOID pv);
    LONG            getSectionIndexFromRVA(ULONG ulRVA);
    BOOL            validateRealPointer(PVOID pv);

    /** Used by getPointerFromRVA and created by getSections and setSectionRVAs. */
    PSECTION            paSections;
    /** Count of entires in the section array (paSection) */
    int                 cSections;

    /** @cat Misc */
    /** Pointer to NT headers. */
    PIMAGE_NT_HEADERS   pNtHdrs;
    /** Flag which indicates wether this is a Win32k loaded
     * module (TRUE) or and Pe2Lx module (FALSE). */
    BOOL                fWin32k;
    /** OS/2 handle of the module. */
    HMODULE             hmod;
    /** Set by Win32Pe2LxDll. */
    BOOL                fDll;
};


#endif //__WINIMAGEPE2LX_H__

