/* $Id: winimagepe2lx.h,v 1.2 1999-10-14 01:39:13 bird Exp $ */

/*
 * Win32 PE2LX Image base class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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
    Win32Pe2LxImage(HINSTANCE hinstance, BOOL fWin32k) throw(ULONG);
    virtual ~Win32Pe2LxImage();

private:
    /** @cat constructor helpers */
    ULONG    getSections();
    ULONG    setSectionRVAs();
    VOID     cleanup();

protected:
    /** @cat RVA -> pointer */
    /* this should be moved to winimagebase some day... */
    PVOID    getPointerFromRVA(ULONG ulRVA);

    PSECTION            paSections; /* Used by getPointerFromRVA and created by getSections and
                                     * setSectionRVAs. */
    WORD                cSections;  /* Count of entires in the section array (paSection) */

    /** @cat Misc */
    PIMAGE_NT_HEADERS   pNtHdrs;    /* Pointer to NT headers. */
    BOOL                fWin32k;    /* flag which indicates wether this is a Win32k loaded
                                     * module (TRUE) or and Pe2Lx module (FALSE). */
};


#endif //__WINIMAGEPE2LX_H__

