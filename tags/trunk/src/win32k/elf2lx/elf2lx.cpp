/* $Id: elf2lx.cpp,v 1.5 2000-12-11 06:53:48 bird Exp $
 *
 * Elf2Lx - implementation.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FOR_EXEHDR 1                    /* To make all object flags OBJ???. */
#define INCL_DOSERRORS                  /* DOS Error codes. */
#ifdef RING0
    #define INCL_NOAPI                  /* RING0: No apis. */
#else /*RING3*/
    #define INCL_DOSPROCESS             /* RING3: DosSleep. */
#endif

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */
#include "types.h"                      /* Types used by the next two files. */
#include <newexe.h>                     /* OS/2 NE structs and definitions. */
#include <exe386.h>                     /* OS/2 LX structs and definitions. */

#include "devSegDf.h"                   /* Win32k segment definitions. */

#include "malloc.h"                     /* win32k malloc (resident). Not C library! */
#include "smalloc.h"                    /* win32k swappable heap. */
#include "rmalloc.h"                    /* win32k resident heap. */

#include <string.h>                     /* C library string.h. */
#include <stdlib.h>                     /* C library stdlib.h. */
#include <stddef.h>                     /* C library stddef.h. */
#include <stdarg.h>                     /* C library stdarg.h. */

#include "vprintf.h"                    /* win32k printf and vprintf. Not C library! */
#include "dev32.h"                      /* 32-Bit part of the device driver. (SSToDS) */
#include "OS2Krnl.h"                    /* kernel structs.  (SFN) */
#ifdef RING0
    #include "ldrCalls.h"               /* ldr* calls. (ldrRead) */
#endif

#include "elf.h"                        /* Elf binary format definitions. */
#include "modulebase.h"                 /* ModuleBase class definitions, ++. */
#include "elf2lx.h"                     /* Elf2Lx class definitions.  */



/**
 * Initiation and termination code for libraries.
 *
 * How will OS2NIX initiate libraries?
 * -A library will register with the OS2NIX dll on init and deregister on
 *  termintation.
 * -If this is before the executable has registered its self, the library
 *  will not be initiated yet. The initiation will take place when the
 *  executable register.
 * -If it is a dynamically loaded library, we'll initiate it during registation.
 *  Or we could initiate it when the load library call returns, ie. after
 *  DosLoadModule. It depends on how we may resolv addresses of external symbols.
 *
 */
static UCHAR achInitTermCode[] =
{
    0xCC
};


/**
 * Startup code for executables.
 *
 * How will OS2NIX start an Elf executable?
 * -An executable will register with the OS2NIX dll on startup.
 * -During this registration it'll resolv addresses of external symbols for
 *  the executable and all the libraries registered at this time. It will then
 *  call the initiation routines for the libraries. Finally it will start
 *  the executable.
 * -If this was a forked process then other steps has to be taken during registration?
 *
 */
static UCHAR achStartupCode[] =
{
    0xCC
};



/**
 * Constructor. Initiates all data members and sets hFile.
 * @param     hFile  Filehandle.
 * @status    Partially implemented.
 * @author    knut st. osmundsen
 * @remark    Remember to update this everytime a new parameter is added.
 */
Elf2Lx::Elf2Lx(SFN hFile) :
    ModuleBase(hFile)
{
    memset(&LXHdr, 0, sizeof(LXHdr));
    LXHdr.e32_magic[0]  = E32MAGIC1;
    LXHdr.e32_magic[1]  = E32MAGIC2;
    LXHdr.e32_border    = E32LEBO;
    LXHdr.e32_worder    = E32LEWO;
    LXHdr.e32_level     = E32LEVEL;
    LXHdr.e32_cpu       = E32CPU386;
    LXHdr.e32_os        = NE_OS2;
    LXHdr.e32_pagesize  = PAGESIZE;
    LXHdr.e32_objtab    = sizeof(LXHdr);
}


/**
 * Destructor.
 * @status    stub
 * @author    knut st. osmundsen
 */
Elf2Lx::~Elf2Lx()
{

}


/**
 * Initiates the Elf2Lx object - builds the virtual LX image.
 * When this function completes the object is no longer in init-mode.
 * @returns   NO_ERROR on success.
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INVALID_EXE_SIGNATURE
 *            ERROR_BAD_EXE_FORMAT
 *            Error code returned by ReadAt.
 *            Error codes from the make* methods.
 * @param     pszFilename  Module filename.
 * @precond   Called in init-mode.
 * @sketch
 *            0. pszFilename & pszModuleName.
 *            1. Read the Elf header.
 *            2. Verify the header.
 *            3. Read Program headers.
 *            4.
 *
 *            5.
 *            6.
 *            7. Start converting the sections by adding the headerobject. (headerobject, see previous chapter).
 *            8. Iterate thru the sectiontable converting the section to objects.
 *              8a. Convert characteristics to flags
 *              8b. Virtual/physical size (see note in code)
 *              8c. Add object.
 *            9.Find where the TIB fix is to be placed. (see 3.1.1 for placements.) Place the TIB fix.
 *              9a. At the end of the header object.
 *              9b. After MZ-Header (In the dos stub!).
 *              9c.Add separate TIBFix object.
 *            10.Add stack object.
 *            11.Align section. (Fix which is applied to EXEs/Dlls which contain no fixups and has an
 *               alignment which is not a multiple of 64Kb. The sections are concatenated into one big object.
 *            12.Update the LXHeader with info which is finalized now. (Stacksize, GUI/CUI, characteristics,...)
 *            13.Convert exports.
 *            14.Convert base relocations (fixups). Remember to add the fixup for RegisterPe2LxDll/Exe.
 *            15.Make object table.
 *            16.Make object page table.
 *            17.Completing the LX header.
 *            18.Set offLXFile in the object array.
 *            19.The conversion method is completed. Object is now usable.
 *            20.Dump virtual LX-file
 *            return successfully.
 * @status    Completely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG Elf2Lx::init(PCSZ pszFilename)
{
    APIRET      rc;
    unsigned    cb;                     /* helper variable, used to hold sizes in bytes. */

    #ifdef DEBUG
    if (!fInitTime)
    {
        printIPE(("init(..) called when not in init mode!\n"));
        return ERROR_INITMETHOD_NOT_INITTIME;
    }
    #endif

    printInf(("Started processing %s\n", pszFilename));

    /*
     * 0.pszFilename & pszModuleName.
     */
    rc = ModuleBase::init(pszFilename);
    if (rc != NO_ERROR)
        return rc;

    /*
     * 1.Read the Elf header.
     */
    pEhdr = (Elf32_Ehdr*)malloc(sizeof(*pEhdr));
    if (pEhdr == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc = ReadAt(hFile, pEhdr->e_phoff, paPhdrs, pEhdr->e_phentsize * pEhdr->e_phnum);
    if (rc != NO_ERROR)
    {
        printErr(("Failed to read Elf header\n"));
        return rc;
    }

    /*
     * 2.Check that it is a valid header. Fail if invalid.
     */
    if (!Elf2Lx::validHeader(pEhdr))
    {
        printErr(("Not a valid Elf format\n"));
        return ERROR_BAD_EXE_FORMAT;
    }

    /*
     * 3.Read program table.
     */
    cb = pEhdr->e_phentsize * pEhdr->e_phnum;
    paPhdrs = (Elf32_Phdr*)malloc(cb);
    if (paPhdrs == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc = ReadAt(hFile, pEhdr->e_phoff, paPhdrs, cb);
    if (rc != NO_ERROR)
    {
        printErr(("Failed to read program headers, off=0x%08x, size=0x%08x\n",
                  pEhdr->e_phoff, cb));
        return rc;
    }

    /*
     * 4.
     */

    return rc;
}




/**
 * Check if the passed in header struct is an Elf header valid for
 * this system.
 * @returns   TRUE:  valid header.
 *            FALSE: invalid header.
 * @param     pEhdr  Pointer to elf header.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL Elf2Lx::validHeader(Elf32_Ehdr *pEhdr)
{
    /*
     * Checks.
     */
    if (*(PULONG)pEhdr == ELFMAGICLSB                   /* Elf magic */
        && pEhdr->e_ident[EI_CLASS] == ELFCLASS32       /* 32-bit format */
        && pEhdr->e_ident[EI_DATA] == ELFDATA2LSB       /* Little endian */
        && pEhdr->e_ident[EI_VERSION] == EV_CURRENT     /* Format version */
        && (pEhdr->e_type == ET_EXEC ||                 /* Filetype Exec or Library */
            pEhdr->e_type == ET_DYN)
        && (pEhdr->e_machine == EM_386 ||               /* Machine type 386 (or 486) */
            pEhdr->e_machine == EM_486)
        && pEhdr->e_version == EV_CURRENT               /* Format version */
        && pEhdr->e_flags == EF_386_NONE                /* No flags for 386 and 486 */
        && pEhdr->e_ehsize >= sizeof(*pEhdr)            /* Elf header not smaller than ours. */
        && pEhdr->e_phentsize >= sizeof(Elf32_Phdr)     /* Program header size not smaller than ours. */
        && pEhdr->e_shentsize >= sizeof(Elf32_Shdr)     /* Section header size not smaller than ours. */
        && pEhdr->e_phnum < 128                         /* An image with more that 128 program headers is supicious! */
        )
        return TRUE;
    return FALSE;
}

