/* $Id: pe2lx.cpp,v 1.32 2001-08-08 17:24:45 bird Exp $
 *
 * Pe2Lx class implementation. Ring 0 and Ring 3
 *
 * Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * Copyright (c) 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright (c) 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FOR_EXEHDR 1                    /* To make all object flags OBJ???. */
#define INCL_DOSERRORS                  /* DOS Error codes. */
#define INCL_OS2KRNL_LDR                /* Loader definitions. */
#define INCL_OS2KRNL_PTDA               /* PTDA definitions. */
#ifdef RING0
    #define INCL_NOAPI                  /* RING0: No apis. */
#else /*RING3*/
    #define INCL_DOSPROCESS             /* RING3: DosSleep. */
    #define INCL_OS2KRNL_LDR_NOAPIS     /* No apis */
#endif


/*
 * Configuration
 */
#define ORD_REGISTERPE2LXEXE        1203UL  /* ordinal entry number for KERNEL32.RegisterPe2LxExe. */
#define ORD_REGISTERPE2LXDLL        1209UL  /* Ordinal entry number for KERNEL32.RegisterPe2LxDll. */
#define TIBFIX_OFF_CALLADDRESS        14UL  /* offset of 'mov cx, KERNEL32:RegisterPe2LxDll/Exe' */
#define EXTRA_FIXUPS                   1    /* TIBFIX call to KERNEL32:RegisterPe2LxDll/Exe. */
#define SIZEOF_TIBFIX     sizeof(achTIBFix)

#define MIN_STACK_SIZE              0x20000 /* 128KB stack */


/*
 * (macro)
 * Allocates more memory for pointer 'pointer'.
 * @returns   ERROR_NOT_ENOUGH_MEMORY if realloc or malloc failes.
 * @param     test         Test which is TRUE if more memory have to be allocated.
 * @param     pointer      Variable name of the pointer which holds/will hold the memory.
 *                         This variable may be changed to point on new memory block.
 * @param     pointertype  Type of the pointer.
 * @param     cballocated  Variable which holds the count of bytes currently allocated.
 *                         This variable will be updated with the new amount of memory.
 * @param     initsize     Initial count of bytes allocated.
 * @param     addsize      Count of bytes to add when the amount of memory is increased.
 * @author    knut st. osmundsen
 */
#define AllocateMoreMemory(test, pointer, pointertype, cballocated, initsize, addsize) \
    if (test)                                                             \
    {                                                                     \
        if (pointer != NULL)                                              \
        {                                                                 \
            PVOID pv = realloc(pointer, (size_t)(cballocated + addsize)); \
            if (pv == NULL)                                               \
                return ERROR_NOT_ENOUGH_MEMORY;                           \
            pointer = (pointertype)pv;                                    \
            cballocated += addsize;                                       \
        }                                                                 \
        else                                                              \
        {   /* first block */                                             \
            pointer = (pointertype)malloc((size_t)(initsize));            \
            if (pointer == NULL)                                          \
                return ERROR_NOT_ENOUGH_MEMORY;                           \
            cballocated = initsize;                                       \
        }                                                                 \
    }



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */
#include <peexe.h>                      /* Wine PE structs and definitions. */
#include <neexe.h>                      /* Wine NE structs and definitions. */
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
    #include "avl.h"                    /* AVL tree. (ldr.h need it) */
    #include "ldr.h"                    /* ldr helpers. (ldrGetExePath) */
    #include "env.h"                    /* Environment helpers. */
    #include "dev32hlp.h"               /* 32-bit devhlpers. Needs LOCKHANDLE. */
    #include "PerTaskW32kData.h"        /* Per Task Win32k Data. */
#endif
#include "modulebase.h"                 /* ModuleBase class definitions, ++. */
#include "pe2lx.h"                      /* Pe2Lx class definitions, ++. */
#include <versionos2.h>                 /* Pe2Lx version. */
#include "yield.h"                      /* Yield CPU. */
#include "options.h"                    /* Win32k options. */


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/**
 * TIBFix code. This is the entry code for both EXEs and DLLs. All it does is
 * calling a KERNEL32 function with tree parameters.
 * For EXE: RegisterPe2LxExe; DLL: RegisterPe2LxDll.
 */
static UCHAR achTIBFix[] =
{
    /* push  [esp+8] */
    0xFF, 0x74, 0x24, 0x08,
    /* push  [esp+4] */
    0xFF, 0x74, 0x24, 0x08,
    /* push internal pe2lx version */
    0x68, (UCHAR)(PE2LX_VERSION),
          (UCHAR)((PE2LX_VERSION) >> 8),
          (UCHAR)((PE2LX_VERSION) >> 16),
    #ifdef RING0                                    /* RING0:                                             */
          (UCHAR)(((PE2LX_VERSION) >> 24) | 0x80),  /* Win32k flag (high bit of the version dword) set.   */
    #else                                           /* RING3:                                             */
          (UCHAR)((PE2LX_VERSION) >> 24),           /* Win32k flag (high bit of the version dword) clear. */
    #endif
    /* mov   ecx, KERNEL32:RegisterPe2LxDll/Exe (stdcall) */
    0xB9, 0x99, 0x99, 0x99, 0x99,
    /* call  ecx */
    0xFF, 0xD1,
    /* ret */
    0xC3
};


/**
 * Conversion table. Used when converting characteristics for sections to flags for objects.
 * Usage: Loop through the table checking if the characterisicts matches ((x & ch) = ch).
 *        When a match is found, the flFlags are ORed to the object flags.
 */
struct Pe2Lx::PeCharacteristicsToLxFlags Pe2Lx::paSecChars2Flags[] =
{   /* PE section characteristics               LX object flags */
    {IMAGE_SCN_CNT_CODE,                        OBJBIGDEF | OBJREAD | OBJEXEC},
    {IMAGE_SCN_MEM_DISCARDABLE,                 OBJBIGDEF | OBJDISCARD},
    {IMAGE_SCN_MEM_SHARED,                      OBJBIGDEF | OBJSHARED},
    {IMAGE_SCN_MEM_EXECUTE,                     OBJBIGDEF | OBJREAD | OBJEXEC},
    {IMAGE_SCN_MEM_READ,                        OBJBIGDEF | OBJREAD},
    {IMAGE_SCN_MEM_WRITE,                       OBJBIGDEF | OBJREAD | OBJWRITE}
};


/**
 * The Pe2Lx LieList. This list is used when there is a name difference between
 * the odin32 module name and the win32 modulename. A reason for such a difference
 * is for example that the modulename exists in OS/2 or another OS/2 product.
 *
 * When this list is updated a similar list in kernel32\xxxx.cpp should also be
 * updated!
 */
struct Pe2Lx::LieListEntry Pe2Lx::paLieList[] =
{   /* Win32 Module name                   Odin32 Module name*/
    {"NETAPI32",                           "WNETAP32"},
    {"NETAPI32.DLL",                       "WNETAP32"},
    {"OLE32",                              "OLE32OS2"},
    {"OLE32.DLL",                          "OLE32OS2"},
    {"OLEAUT32.DLL",                       "OLAUTOS2"},
    {"OLEAUT32",                           "OLAUTOS2"},
    {"OPENGL",                             "OPENGL32"},
    {"OPENGL.DLL",                         "OPENGL32"},
    {"CRTDLL",                             "CRTDLL32"},
    {"CRTDLL.DLL",                         "CRTDLL32"},
    {"MCICDA.DRV",                         "MCICDA.DLL"},
    {"WINSPOOL.DRV",                       "WINSPOOL.DLL"},
    {NULL,                                 NULL} /* end-of-list entry */
};

LONG            Pe2Lx::cLoadedModules;  /* Count of existing objects. Updated by constructor and destructor. */
const char *    Pe2Lx::pszOdin32Path;   /* Odin32 base path (include a slash). */
ULONG           Pe2Lx::cchOdin32Path;   /* Odin32 base path length. */
SFN             Pe2Lx::sfnKernel32;     /* Odin32 Kernel32 filehandle. */



/**
 * Constructor. Initiates all data members and sets hFile.
 * @param     hFile  Filehandle.
 * @status    Completely implemented.
 * @author    knut st. osmundsen
 * @remark    Remember to update this everytime a new parameter is added.
 */
Pe2Lx::Pe2Lx(SFN hFile) :
    ModuleBase(hFile),
    fAllInOneObject(FALSE), paObjects(NULL), cObjects(0), cObjectsAllocated(0),
    paObjTab(NULL), paObjPageTab(NULL),
    pachResNameTable(NULL), offCurResName(0), cchRNTAllocated(0),
    pEntryBundles(NULL), offCurEntryBundle(0), offLastEntryBundle(0),
    ulLastOrdinal(0), cbEBAllocated(0),
    fForwarders(FALSE),
    paulFixupPageTable(NULL), cFixupPTEntries(0), cFPTEAllocated(0),
    pFixupRecords(NULL), offCurFixupRec(0), cbFRAllocated(0),
    pvCrossPageFixup(NULL), cbCrossPageFixup(0),
    pachImpModuleNames(NULL), offCurImpModuleName(0), cchIMNAllocated(0),
    pachImpFunctionNames(NULL), offCurImpFunctionName(0), cchIFNAllocated(0),
    offNtHeaders(0), pNtHdrs(NULL), ulImageBase(0UL), pBaseRelocs(0),
    fApplyFixups(~0UL), fDeltaOnly(0)
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
    cLoadedModules++;
}


/**
 * Destructor.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
Pe2Lx::~Pe2Lx()
{
    if (paObjects != NULL)
    {
        free(paObjects);
        paObjects = NULL;
    }
    if (paObjTab != NULL)
    {
        free(paObjTab);
        paObjTab = NULL;
    }
    if (paObjPageTab != NULL)
    {
        free(paObjPageTab);
        paObjPageTab = NULL;
    }
    if (pachResNameTable != NULL)
    {
        free(pachResNameTable);
        pachResNameTable = NULL;
    }
    if (pEntryBundles != NULL)
    {
        free(pEntryBundles);
        pEntryBundles = NULL;
    }
    if (paulFixupPageTable != NULL)
    {
        free(paulFixupPageTable);
        paulFixupPageTable = NULL;
    }
    if (pFixupRecords != NULL)
    {
        free(pFixupRecords);
        pFixupRecords = NULL;
    }
    if (pvCrossPageFixup != NULL)
    {
        free(pvCrossPageFixup);
        pvCrossPageFixup = NULL;
    }
    if (pachImpModuleNames != NULL)
    {
        free(pachImpModuleNames);
        pachImpModuleNames = NULL;
    }
    if (pachImpFunctionNames != NULL)
    {
        free(pachImpFunctionNames);
        pachImpFunctionNames = NULL;
    }
    if (pNtHdrs != NULL)
    {
        free(pNtHdrs);
        pNtHdrs = NULL;
    }
    if (pBaseRelocs != NULL)
    {
        sfree(pBaseRelocs);
        pBaseRelocs = NULL;
    }
    _res_heapmin();
    _swp_heapmin();

    /*
     * If no Pe2Lx objects left, then invalidate the Odin32Path.
     *  We'll have to do this since we may (theoretically) not receive
     *  a close on a kernel32 handle if loading failes before kernel32
     *  is loaded and the odin32path is determined using ldrOpenPath.
     *  (Ie. no sfnKernel32.)
     */
    if (--cLoadedModules <= 0)
        invalidateOdin32Path();
#ifdef DEBUG
    if (cLoadedModules < 0)
        printIPE(("Pe2Lx::cLoadedModules is %d which is less that zero!\n", cLoadedModules));
#endif
}


/**
 * Initiates the Pe2Lx object - builds the virtual LX image.
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
 *            1. Read the from the start of the file, expect a MZ header.
 *            2. If an PE header was found jump to 4.
 *            3. Verify valid MZ header - if not fail. Determin PE offset.
 *            4. Read PE headers.
 *            5. Validate PE header  (Magics, Machine, subsystem, characteristics,...) - fail if not valid.
 *            6. Read sectiontable.
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
ULONG Pe2Lx::init(PCSZ pszFilename)
{
    APIRET                  rc;
    PIMAGE_DOS_HEADER       pMzHdr;
    int                     i, j;
    PIMAGE_SECTION_HEADER   paSections;     /* Pointer to section headers */

    #ifdef DEBUG
    if (!fInitTime)
    {
        printIPE(("init(..) called when not in init mode!\n"));
        return ERROR_INITMETHOD_NOT_INITTIME;
    }
    #endif

    printInf(("Started processing %s\n", pszFilename));

    /* 0.pszFilename & pszModuleName. */
    rc = ModuleBase::init(pszFilename);
    if (rc != NO_ERROR)
        return rc;

    /* 1.Read the from the start of the file, expect a MZ header. */
    pMzHdr = (PIMAGE_DOS_HEADER)malloc(sizeof(IMAGE_DOS_HEADER));
    if (pMzHdr == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc = ReadAt(hFile, 0UL, pMzHdr, sizeof(IMAGE_DOS_HEADER));
    if (rc != NO_ERROR)
    {
        free(pMzHdr);
        return rc;
    }

    /* 2.If an PE header was found jump to 4. */
    if (*(PULONG)(pMzHdr) != IMAGE_NT_SIGNATURE)
    {
        /* 3.Verify valid MZ header - if not fail. Determin PE offset. */
        if (pMzHdr->e_magic == IMAGE_DOS_SIGNATURE)
        {
            if (pMzHdr->e_lfanew > sizeof(IMAGE_DOS_HEADER) && pMzHdr->e_lfanew < 0x04000000UL) /* Larger than 64 bytes and less that 64MB. */
                offNtHeaders = pMzHdr->e_lfanew;
            else
                rc = ERROR_INVALID_EXE_SIGNATURE;
        }
        else
            rc = ERROR_INVALID_EXE_SIGNATURE;
    }
    else
        offNtHeaders = 0UL;

    free(pMzHdr);
    pMzHdr = NULL;
    if (rc != NO_ERROR)
    {
        printErr(("Not PE executable.\n"));
        return rc;
    }

    /* 4.Read PE headers. */
    pNtHdrs = (PIMAGE_NT_HEADERS)malloc(sizeof(IMAGE_NT_HEADERS));
    if (pNtHdrs == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc = ReadAt(hFile, offNtHeaders, pNtHdrs, sizeof(IMAGE_NT_HEADERS));
    if (rc != NO_ERROR)
        return rc;

    /* 5.Validate PE header  (Magics, Machine, subsystem, characteristics,...) - fail if not valid. */
    if (pNtHdrs->Signature != IMAGE_NT_SIGNATURE)
    {
        printErr(("Invalid PE signature, '%c%c%c%c'\n",
                 ((PCHAR)&pNtHdrs->Signature)[0], ((PCHAR)&pNtHdrs->Signature)[1],
                 ((PCHAR)&pNtHdrs->Signature)[2], ((PCHAR)&pNtHdrs->Signature)[3]));
        return ERROR_INVALID_EXE_SIGNATURE;
    }
    dumpNtHeaders(pNtHdrs);
    if (pNtHdrs->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
    {
        printErr(("Invalid Machine! %#4x\n", pNtHdrs->FileHeader.Machine));
        return ERROR_BAD_EXE_FORMAT;
    }
    if (!(pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
    {
        printErr(("Not executable file!\n"));
        return ERROR_BAD_EXE_FORMAT;
    }
    if (pNtHdrs->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
    {
        printErr(("Invalid optional header.\n"));
        return ERROR_BAD_EXE_FORMAT;
    }
    if (pNtHdrs->FileHeader.SizeOfOptionalHeader
        - ((pNtHdrs->OptionalHeader.NumberOfRvaAndSizes - IMAGE_NUMBEROF_DIRECTORY_ENTRIES) * sizeof(IMAGE_DATA_DIRECTORY))
        != sizeof(IMAGE_OPTIONAL_HEADER))
    {
        printErr(("Invalid optional header size.\n"));
        return ERROR_BAD_EXE_FORMAT;
    }
    if (pNtHdrs->OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_WINDOWS_CUI &&
        pNtHdrs->OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_WINDOWS_GUI &&
        pNtHdrs->OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_NATIVE)
    {
        printErr(("Subsystem not supported. %d\n", pNtHdrs->OptionalHeader.Subsystem));
        return ERROR_BAD_EXE_FORMAT;
    }

    /* 6.Read sectiontable. */
    paSections = (PIMAGE_SECTION_HEADER)malloc(sizeof(IMAGE_SECTION_HEADER) * pNtHdrs->FileHeader.NumberOfSections);
    if (paSections == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc = ReadAt(hFile,
                offNtHeaders + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + pNtHdrs->FileHeader.SizeOfOptionalHeader,
                paSections,
                pNtHdrs->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
    if (rc != NO_ERROR)
        return rc;

    /* 7.Start converting the sections by adding the headerobject. (headerobject, see previous chapter). */
    rc = offNtHeaders + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + pNtHdrs->FileHeader.SizeOfOptionalHeader //could we use OptionalHeader.SizeOfHeaders?
         + pNtHdrs->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
    rc = addObject(0UL, rc, rc, OBJREAD | OBJBIGDEF, 0UL);
    if (rc != NO_ERROR)
    {
        printErr(("Failed to add header object\n"));
        return rc;
    }

    /* 8.Iterate thru the sectiontable converting the section to objects. */
    for (i = 0; i < pNtHdrs->FileHeader.NumberOfSections; i++)
    {
        ULONG cbVirt,cbPhys;
        ULONG flFlags = 0;

        dumpSectionHeader(&paSections[i]);

        /* 8a. Convert characteristics to flags and check/fix incompatible flags! */
        for (j = 0; j < (sizeof(paSecChars2Flags)/sizeof(paSecChars2Flags[0])); j++)
            if ((paSections[i].Characteristics & paSecChars2Flags[j].Characteristics) == paSecChars2Flags[j].Characteristics)
                flFlags |= paSecChars2Flags[j].flFlags;
        if ((flFlags & (OBJEXEC | OBJWRITE)) == (OBJEXEC | OBJWRITE))
            flFlags &= (ULONG)~OBJEXEC;

        /* 8b. Virtual/physical size */
        /* The virtual size and physical size is somewhat problematic. Some linkers sets Misc.VirtualSize and
         * others don't. Some linkers sets the Misc.VirtualSize to the exact size of data in the section, which
         * may cause that the VirtualSize to be less than the SizeOfRawData due to file alignment.
         * We assume/know certain things:
         *    -SizeOfRawData is allways the physical size when PointerToRawData is valid (not 0).
         *     (No matter what Matt Pietrek may have written!) NT maps in the entire physical size.
         *    -The VirtualSize may contain 0, then the virtual size is equal to the Physical size.
         *    -The VirtualSize may be less than the SizeOfRawData.
         * This means that we can't allways use the VirtualSize as the size of LX objects.
         */
        cbPhys = paSections[i].PointerToRawData != 0UL ? paSections[i].SizeOfRawData : 0UL;
        cbVirt = paSections[i].Misc.VirtualSize > paSections[i].SizeOfRawData ?
            paSections[i].Misc.VirtualSize : paSections[i].SizeOfRawData;

        /* 8c. Add object. */
        rc = addObject(paSections[i].VirtualAddress, cbPhys, cbVirt, flFlags, paSections[i].PointerToRawData);
        if (rc != NO_ERROR)
        {
            printErr(("Failed to add object for section no.%d. rc = %d\n", i, rc));
            return rc;
        }
    }
    free(paSections);
    paSections = NULL;

    /* 9.Find where the TIB fix is to be placed. (see 3.1.1 for placements.) Place the TIB fix.  */
    if (PAGESIZE - (paObjects[0].cbVirtual & (PAGESIZE-1)) >= SIZEOF_TIBFIX)
    {   /* 9a. At the end of the header object. */
        paObjects[0].Misc.offTIBFix = paObjects[0].cbVirtual;
        paObjects[0].Misc.fTIBFixObject = TRUE;
        paObjects[0].cbVirtual += SIZEOF_TIBFIX;
        paObjects[0].cbPhysical += SIZEOF_TIBFIX;
        paObjects[0].flFlags |= OBJEXEC;
        printInf(("TIBFix code at end of header object. offset=%#x\n", paObjects[0].Misc.offTIBFix));
    }
    else if (offNtHeaders >= sizeof(IMAGE_DOS_HEADER) + SIZEOF_TIBFIX)
    {   /* 9b. After MZ-Header (In the dos stub!). */
        paObjects[0].Misc.offTIBFix = sizeof(IMAGE_DOS_HEADER);
        paObjects[0].Misc.fTIBFixObject = TRUE;
        paObjects[0].flFlags |= OBJEXEC;
        printInf(("TIBFix code in dos stub. offset=%#x\n", paObjects[0].Misc.offTIBFix));
    }
    else
    {   /* 9c.Add separate TIBFix object. */
        printInf(("TIBFix code in separate object.\n"));
        rc = addTIBFixObject();
        if (rc != NO_ERROR)
        {
            printErr(("Failed to insert TIBFix, rc=%d\n", rc));
            return rc;
        }
    }

    /* 10.Add stack object. */
    if (!(pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL))
    {
        rc = addStackObject(max(pNtHdrs->OptionalHeader.SizeOfStackReserve, MIN_STACK_SIZE));
        if (rc != NO_ERROR)
        {
            printErr(("Failed to insert TIBFix, rc=%d\n", rc));
            return rc;
        }
    }

    /* 11.Align section. (Fix which is applied to EXEs/Dlls which contain no fixups and has an
     *    alignment which is not a multiple of 64Kb. The sections are concatenated into one big object. */
    /* TODO! this test has to be enhanced a bit. WWPack32, new Borland++ depends on image layout. */
    fAllInOneObject =  !isPEOneObjectDisabled()
                       && (   isPEOneObjectForced()
                           || (pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) == IMAGE_FILE_RELOCS_STRIPPED
                           );
    if (fAllInOneObject)
    {
        printInf(("All-In-One-Object fix is applied.\n"));
        if (pNtHdrs->OptionalHeader.ImageBase >= 0x04000000UL && !(pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL))
            printWar(("ImageBase >= 64MB this object may not be runnable! (ImageBase=%#8x)\n",
                     pNtHdrs->OptionalHeader.ImageBase));
    }

    /* 12.Update the LXHeader with info which is finalized now. (Stacksize, GUI/CUI, characteristics,...) */
    for (i = 0, j = 1; i < cObjects; i++, j += !fAllInOneObject ? 1 : 0)
    {
        if (paObjects[i].Misc.fTIBFixObject)
        {
            LXHdr.e32_startobj = j;
            LXHdr.e32_eip = (fAllInOneObject ? paObjects[i].ulRVA : 0UL) + paObjects[i].Misc.offTIBFix;
        }
        else if (paObjects[i].Misc.fStackObject)
        {
            LXHdr.e32_stackobj = j;
            LXHdr.e32_esp = (fAllInOneObject ? paObjects[i].ulRVA : 0UL) + paObjects[i].cbVirtual;
            LXHdr.e32_stacksize = paObjects[i].cbVirtual;
        }
    }
    ulImageBase = pNtHdrs->OptionalHeader.ImageBase;
    LXHdr.e32_mflags = pNtHdrs->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI
        ? E32PMAPI : E32PMW;
    if (pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL)
        LXHdr.e32_mflags |= E32LIBINIT | E32LIBTERM | E32MODDLL;
    else
    {
        LXHdr.e32_mflags |= E32MODEXE;
        if (    pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED     /* Force location if possible. */
            ||  ulImageBase < 0x04000000 && ulImageBase + pNtHdrs->OptionalHeader.SizeOfImage < 0x04000000)
            LXHdr.e32_mflags |= E32NOINTFIX;
    }

    /* 13.Convert exports. */
    rc = makeExports();
    if (rc != NO_ERROR)
    {
        printErr(("Failed to make exports rc=%d\n", rc));
        return rc;
    }
    Yield();

    /* 14.Convert base relocations (fixups). Remember to add the fixup for RegisterPe2LxDll/Exe. */
    rc = makeFixups();
    if (rc != NO_ERROR)
    {
        printErr(("Failed to make fixups rc=%d\n", rc));
        return rc;
    }
    Yield();

    /* 15.Make object table. */
    rc = makeObjectTable();
    if (rc != NO_ERROR)
    {
        printErr(("Failed to make object table rc=%d\n", rc));
        return rc;
    }

    /* 16.Make object page table. */
    rc = makeObjectPageTable();
    if (rc != NO_ERROR)
    {
        printErr(("Failed to make object table rc=%d\n", rc));
        return rc;
    }
    Yield();

    /* 17.Completing the LX header. */
    LXHdr.e32_mpages = getCountOfPages();
    LXHdr.e32_objcnt = fAllInOneObject ? 1 : cObjects;
    LXHdr.e32_objmap = LXHdr.e32_objtab + sizeof(struct o32_obj) * LXHdr.e32_objcnt;
    LXHdr.e32_restab = LXHdr.e32_objmap + LXHdr.e32_mpages * sizeof(struct o32_map);
    LXHdr.e32_enttab = LXHdr.e32_restab + offCurResName;
    LXHdr.e32_fpagetab = LXHdr.e32_enttab + offCurEntryBundle;
    LXHdr.e32_frectab  = LXHdr.e32_fpagetab + (LXHdr.e32_mpages + 1) * sizeof(ULONG);
    LXHdr.e32_impmod   = LXHdr.e32_frectab + offCurFixupRec;
    LXHdr.e32_impproc  = LXHdr.e32_impmod + offCurImpModuleName;
    LXHdr.e32_datapage = LXHdr.e32_impproc + offCurImpFunctionName;

    LXHdr.e32_fixupsize = (LXHdr.e32_mpages + 1) * sizeof(ULONG)       /* fixup page table */
                        + offCurFixupRec                               /* fixup record table */
                        + offCurImpModuleName                          /* import module name table */
                        + offCurImpFunctionName;                       /* import procedure name table */

    LXHdr.e32_ldrsize = LXHdr.e32_objcnt * sizeof(struct o32_obj)      /* object table */
                        + LXHdr.e32_mpages * sizeof(struct o32_map)    /* object page table */
                        + offCurResName                                /* resident names */
                        + offCurEntryBundle                            /* entry tables */
                        + (LXHdr.e32_mpages + 1) * sizeof(ULONG)       /* fixup page table */
                        + offCurFixupRec                               /* fixup record table */
                        + offCurImpModuleName                          /* import module name table */
                        + offCurImpFunctionName;                       /* import procedure name table */

    /* find import module  count */
    i = 0;
    LXHdr.e32_impmodcnt = 0;
    while (i < offCurImpModuleName)
    {
        LXHdr.e32_impmodcnt++;
        /* next */
        i += pachImpModuleNames[i] + 1;
    }

    /* 18.Set offLXFile in the object array. */
    ULONG offObjectData = LXHdr.e32_datapage;
    for (i = 0; i < cObjects; i++)
    {
        if (paObjects[i].cbPhysical > 0UL)
        {
            paObjects[i].offLXFile = offObjectData;
            offObjectData += paObjects[i].cbPhysical;
        }
        else
            paObjects[i].offLXFile = 0UL;
    }

    /* 19.The conversion method is completed. Object is now usable. */
    #ifdef DEBUG
    fInitTime = FALSE;
    #endif
    releaseNtHeaders();

    /* 20.Dump virtual LX-file */
    dumpVirtualLxFile();

    Yield();
    _res_heapmin();
    #ifndef RING0
    #if 1 /* testing */
    testApplyFixups();
    #endif
    #endif

    return NO_ERROR;
}


/**
 * Read data from the virtual LX-file.
 * @param     offLXFile  Offset (into the virtual lx file) of the data to read
 * @param     pvBuffer   Pointer to buffer where data is to be put.
 * @param     cbToRead   Bytes to be read.
 * @param     fpBuffer   Flags which was spesified to the ldrRead call.
 * @parma     pMTE       Pointer to MTE which was specified to the ldrRead call.
 * @return    NO_ERROR if successful something else if not.
 * @status    completely implmented; tested.
 * @author    knut st. osmundsen
 */
ULONG Pe2Lx::read(ULONG offLXFile, PVOID pvBuffer, ULONG fpBuffer, ULONG cbToRead, PMTE pMTE)
{
    APIRET rc = NO_ERROR;   /* Return code. */
    ULONG  cbReadVar;       /* Number of bytes in a read operation. */
    ULONG  offPEFile;       /* Offset into the PE-File of the read operation. */

    /* validate input pointers */
    if (pvBuffer < (PVOID)0x10000)
    {
        printErr(("Invalid parameter, pvBuffer = 0x%08x\n", pvBuffer));
        return ERROR_INVALID_PARAMETER;
    }
    #ifdef DEBUG
    if (fInitTime)
    {
        printErr(("the read method may not be called during init.\n"));
        return ERROR_INVALID_PARAMETER;
    }
    #endif

    printInf(("Pe2Lx::read(%d, 0x%08x, 0x%08x, %d)\n", offLXFile, pvBuffer, fpBuffer, cbToRead));

    /* Could we skip right to the datapages? */
    if (offLXFile < LXHdr.e32_datapage)
    {   /* -no. */
        PVOID pv;
        ULONG cb;
        ULONG off;
        ULONG ulIndex = 0;
        while (ulIndex < 9 && cbToRead > 0UL)
        {
            /* ASSUME a given order of tables! (See near bottom of the init() method) */
            switch (ulIndex)
            {
                case 0: /* LXHdr */
                    off = 0UL;
                    cb = sizeof(LXHdr);
                    break;
                case 1: /* Object Table */
                    off = LXHdr.e32_objtab;
                    cb = sizeof(struct o32_obj) * LXHdr.e32_objcnt;
                    break;
                case 2: /* Object Page Table */
                    off = LXHdr.e32_objmap;
                    cb = LXHdr.e32_mpages * sizeof(struct o32_map);
                    break;
                case 3: /* Resident Name Table */
                    off = LXHdr.e32_restab;
                    cb = LXHdr.e32_enttab - off;
                    break;
                case 4: /* Entry Table */
                    off = LXHdr.e32_enttab;
                    cb = LXHdr.e32_fpagetab - off;
                    break;
                case 5: /* FixupTable */
                    off = LXHdr.e32_fpagetab;
                    cb = LXHdr.e32_frectab - off;
                    break;
                case 6: /* Fixup Record Table */
                    off = LXHdr.e32_frectab;
                    cb = LXHdr.e32_impmod - off;
                    break;
                case 7: /* Import Module Name Table */
                    off = LXHdr.e32_impmod;
                    cb = LXHdr.e32_impproc - off;
                    break;
                case 8: /* Import Procedure Name Table */
                    off = LXHdr.e32_impproc;
                    cb = LXHdr.e32_datapage - off;
                    break;
                default: printIPE(("invalid ulIndex. ulIndex = %d\n", ulIndex)); return ERROR_INTERNAL_PROCESSING_ERROR;
            }

            /* Is it this header part? */
            if ((ulIndex == 0 || off != 0UL) && cb != 0UL && offLXFile < off + cb)
            {
                /* find pointer. */
                switch (ulIndex)
                {
                    case 0: /* LXHdr */
                        pv = &LXHdr;
                        break;
                    case 1: /* Object Table */
                        if (paObjTab == NULL)
                            rc = makeObjectTable();
                        pv = paObjTab;
                        break;
                    case 2: /* Object Page Table */
                        if (paObjPageTab == NULL)
                            rc = makeObjectPageTable();
                        pv = paObjPageTab;
                        break;
                    case 3: /* Resident Name Table */
                        if (pachResNameTable == NULL)
                        {
                            rc = makeExports();
                            releaseNtHeaders();
                            finalizeImportNames();
                        }
                        pv = pachResNameTable;
                        break;
                    case 4: /* Entry Table */
                        if (pEntryBundles == NULL)
                        {
                            rc = makeExports();
                            releaseNtHeaders();
                            finalizeImportNames();
                        }
                        pv = pEntryBundles;
                        break;
                    case 5: /* FixupTable */
                        if (paulFixupPageTable == NULL)
                        {
                            rc = makeFixups();
                            releaseNtHeaders();
                        }
                        pv = paulFixupPageTable;
                        break;
                    case 6: /* Fixup Record Table */
                        if (pFixupRecords == NULL)
                        {
                            rc = makeFixups();
                            releaseNtHeaders();
                        }
                        pv = pFixupRecords;
                        break;
                    case 7: /* Import Module Name Table */
                        if (pachImpModuleNames == NULL)
                        {
                            rc = makeFixups();
                            releaseNtHeaders();
                        }
                        pv = pachImpModuleNames;
                        break;
                    case 8: /* Import Procedure Name Table */
                        if (pachImpFunctionNames == NULL)
                        {
                            rc = makeFixups();
                            releaseNtHeaders();
                        }
                        pv = pachImpFunctionNames;
                        break;
                    default: printIPE(("invalid ulIndex. ulIndex = %d\n", ulIndex)); return ERROR_INTERNAL_PROCESSING_ERROR;
                }

                /* check if any operation has failed */
                if (rc != NO_ERROR)
                {
                    printErr(("A makeXxxxx operation failed with rc=%d, ulIndex=%d.\n", rc, ulIndex));
                    return ERROR_READ_FAULT;
                }

                /* verify that offLXfile is greater or equal to off. */
                if (offLXFile < off)
                {
                    printIPE(("offLXFile (%d) < off(%d)!\n", offLXFile, off));
                    return ERROR_READ_FAULT;
                }

                /* do the actual "read" operation. */
                cbReadVar = cb - (offLXFile - off); /* left of this header part. */
                cbReadVar = min(cbReadVar, cbToRead);
                memcpy(pvBuffer, (PVOID)((ULONG)pv + (offLXFile - off)), (size_t)cbReadVar);

                /* Could this header part be freed now? */
                if (cbReadVar + (offLXFile - off) == cb) /* have read to end of this header part. */
                {
                    switch (ulIndex)
                    {
                        case 0: /* LXHdr - ignore */
                            break;
                        case 1: /* Object Table */
                            free(paObjTab);
                            paObjTab = NULL;
                            break;
                        case 2: /* Object Page Table */
                            free(paObjPageTab);
                            paObjPageTab = NULL;
                            break;
                        case 3: /* Resident Name Table */
                            free(pachResNameTable);
                            pachResNameTable = NULL;
                            cchRNTAllocated = offCurResName = 0UL;
                            break;
                        case 4: /* Entry Table */
                            free(pEntryBundles);
                            pEntryBundles = NULL;
                            offCurEntryBundle = offLastEntryBundle = ulLastOrdinal = cbEBAllocated = 0UL;
                            break;
                        case 5: /* FixupTable */
                            free(paulFixupPageTable);
                            paulFixupPageTable = NULL;
                            cFixupPTEntries = cFPTEAllocated = 0UL;
                            break;
                        case 6: /* Fixup Record Table */
                            free(pFixupRecords);
                            pFixupRecords = NULL;
                            offCurFixupRec = cbFRAllocated = 0UL;
                            break;
                        case 7: /* Import Module Name Table */
                            free(pachImpModuleNames);
                            pachImpModuleNames = NULL;
                            offCurImpModuleName = cchIMNAllocated = 0UL;
                            break;
                        case 8: /* Import Procedure Name Table */
                            free(pachImpFunctionNames);
                            pachImpFunctionNames = NULL;
                            offCurImpFunctionName = cchIFNAllocated = 0UL;
                            break;
                        default: printIPE(("invalid ulIndex. ulIndex = %d\n", ulIndex)); return ERROR_INTERNAL_PROCESSING_ERROR;
                    }
                }

                /* next */
                cbToRead -= cbReadVar;
                offLXFile += cbReadVar;
                pvBuffer = (PVOID)((ULONG)pvBuffer + cbReadVar);

            } /* read it */

            /* next */
            ulIndex++;
        } /* while loop */
    }


    /*
     * data within the objects?
     */
    ULONG iObj = 0UL;
    while (cbToRead > 0UL && rc == NO_ERROR)
    {
        /* find object with requested data */
        while (iObj < cObjects && offLXFile >= paObjects[iObj].offLXFile + paObjects[iObj].cbPhysical)
            iObj++;
        if (iObj >= cObjects)
        {   /* data not found... */
            if (cbReadVar > 0UL)
                printWar(("Read request crossing end of file.\n"));
            else
                printErr(("Read request after end of file.\n"));
            rc = ERROR_NO_DATA; /*??*/
            break;
        }

        /* calc offsets */
        ULONG offObject = offLXFile - paObjects[iObj].offLXFile;
        offPEFile = offObject + paObjects[iObj].offPEFile;

        /* TIBFix object? */
        if (!paObjects[iObj].Misc.fTIBFixObject
            || paObjects[iObj].Misc.offTIBFix + (ULONG)SIZEOF_TIBFIX <= offObject)
        {   /* not TIB object OR after the TIBFix code */
            /* calc PE offset and size of read. */
            cbReadVar = min(paObjects[iObj].cbPhysical - offObject, cbToRead);
            rc = ReadAtF(hFile, offPEFile, pvBuffer, fpBuffer, cbReadVar, pMTE);
        }
        else
        {   /* before or in the TIBFix code. */
            if (offObject < paObjects[iObj].Misc.offTIBFix)
            {   /* before TIBFix code. */
                cbReadVar = min(paObjects[iObj].Misc.offTIBFix - offObject, cbToRead);
                rc = ReadAtF(hFile, offPEFile, pvBuffer, fpBuffer, cbReadVar, pMTE);
            }
            else
            {   /* TIBFix code.*/
                offObject -= paObjects[iObj].Misc.offTIBFix;    /* now offset into the TIBFix. */
                cbReadVar = min(SIZEOF_TIBFIX - offObject, cbToRead);
                memcpy(pvBuffer, &achTIBFix[offObject], (size_t)cbReadVar);
            }
        }

        if (rc == NO_ERROR)
        {   /* success - update variables */
            cbToRead -= cbReadVar;
            offLXFile += cbReadVar;
            pvBuffer = (PVOID)((ULONG)pvBuffer + cbReadVar);
        }
        else
            printErr(("Read operation failed with rc=%d, offPEFile=0x%x, cbReadVar=%d, iObj=%d\n",
                      rc, offPEFile, cbReadVar, iObj));
    }

    NOREF(fpBuffer);
    NOREF(pMTE);
    return rc;
}


/**
 * Applies relocation fixups to a page which is being loaded.
 * @returns    NO_ERROR on success?
 *             error code on error?
 * @param      pMTE           Pointer Module Table Entry.
 * @param      iObject        Index into the object table. (0-based)
 * @param      iPageTable     Index into the page table. (0-based)
 * @param      pvPage         Pointer to the page which is being loaded.
 * @param      ulPageAddress  Address of page.
 * @param      pvPTDA         Pointer to Per Task Data Aera
 *
 * @sketch     Find RVA.
 * @remarks    Some more information on relocations:
 * From Web:
 *   IMAGE_REL_I386_ABSOLUTE Reference is absolute, no relocation is necessary
 *   IMAGE_REL_I386_DIR16    Direct 16-bit reference to the symbols virtual address
 *   IMAGE_REL_I386_REL16    PC-relative 16-bit reference to the symbols virtual address
 *   IMAGE_REL_I386_DIR32    Direct 32-bit reference to the symbols virtual address
 *   IMAGE_REL_I386_DIR32NB  Direct 32-bit reference to the symbols virtual address, base not included
 *   IMAGE_REL_I386_SEG12    Direct 16-bit reference to the segment-selector bits of a 32-bit virtual address
 *   IMAGE_REL_I386_SECTION  ?
 *   IMAGE_REL_I386_SECREL   ?
 *   IMAGE_REL_I386_REL32    PC-relative 32-bit reference to the symbols virtual address
 *
 * From TIS:
 *   Type = 4­bit fixup type. This value has the following definitions:
 *   0h     Absolute. This is a NOP. The fixup is skipped.
 *   1h     High. Add the high 16 bits of the delta to the 16 bit field at Offset.
 *          The 16bit field represents the high value of a 32 bit word.
 *   2h     Low. Add the low 16 bits of the delta to the 16 bit field at Offset.
 *          The 16 bit field represents the low half value of a 32 bit word. This
 *          fixup will only be emitted for a RISC machine when the image Object
 *          Align isn't the default of 64K.
 *   3h     Highlow. Apply the 32 bit delta to the 32 bit field at Offset.
 *   4h     Highadjust. This fixup requires a full 32 bit value. The high 16 bits
 *          is located at Offset, and the low 16 bits is located in the next Offset
 *          array element (this array element is included in the Size field). The
 *          two need to be combined into a signed variable. Add the 32 bit delta.
 *          Then add 0x8000 and store the high 16 bits of the signed variable to
 *          the 16 bit field at Offset.
 *   5h ­ Mipsjmpaddr.
 *
 * TODO: implement the above mentioned fixups.
 */
ULONG  Pe2Lx::applyFixups(PMTE pMTE, ULONG iObject, ULONG iPageTable, PVOID pvPage,
                          ULONG ulPageAddress, PVOID pvPTDA)
{
    APIRET rc;

    if (fApplyFixups != FALSE && pBaseRelocs != NULL)
    {
        ULONG ulRVAPage;
        ULONG ulDelta;
        PSMTE pSMTE = pMTE->mte_swapmte;

        /* validate input */
        if (pSMTE < (PSMTE)0x10000)
        {
            printErr(("Invalid pSMTE(0x%08x)\n", pSMTE));
            return ERROR_INVALID_PARAMETER;
        }
        #ifdef DEBUG
        if (pSMTE->smte_objcnt <= iObject)
        {
            printErr(("Invalid iObject(%d), smte_objcnt=%d\n", iObject, pSMTE->smte_objcnt));
            return ERROR_INVALID_PARAMETER;
        }
        if (cObjects <= iObject)
        {
            printErr(("Invalid iObject(%d), cObjects=%d\n", iObject, cObjects));
            return ERROR_INVALID_PARAMETER;
        }
        #endif

        /* some calculations */
        ulDelta     = pSMTE->smte_objtab[iObject].ote_base - paObjects[iObject].ulRVA - ulImageBase;
        ulRVAPage   = paObjects[iObject].ulRVA + ulPageAddress - pSMTE->smte_objtab[iObject].ote_base;

        /* check if the fixup needs to be applied? */
        if (fApplyFixups == ~0UL)
        {
            fDeltaOnly = TRUE;          /* IMPORTANT: Later code assumes that this is true when fAllInOneObject is true. */
            if (fAllInOneObject)
                fApplyFixups = ulImageBase != pSMTE->smte_objtab[0].ote_base;
            else
            {
                int i = 0;
                #ifdef DEBUG
                if (cObjects != pSMTE->smte_objcnt)
                    printErr(("cObjects(%d) != smte_objcnt(%d)\n", cObjects, pSMTE->smte_objcnt));
                #endif
                fApplyFixups = FALSE;
                while (i < cObjects && (!fApplyFixups || fDeltaOnly))
                {
                    register ULONG ulTmp = pSMTE->smte_objtab[i].ote_base - ulImageBase - paObjects[i].ulRVA;
                    if (ulTmp != 0)
                        fApplyFixups = TRUE;
                    if (ulTmp != ulDelta)
                        fDeltaOnly = FALSE;
                    i++;
                }
            }
            if (!fApplyFixups)
                return NO_ERROR;
        }

        if (fDeltaOnly)
            return applyFixupsDelta(pvPage, ulDelta, ulRVAPage);

        /*
         * Iterate thru the fixup chunks until we find one or two with fixups valid for
         * this page. I say one or two since we might have cross page fixups from the previous page
         * So, we'll have to start working at the previous page.
         * ASSUME: -fixups are sorted ascendingly on page RVA. only one chunk per page.
         *         -only one cross page fixup at each end which doesn't overlapp with other fixups.
         */
        PIMAGE_BASE_RELOCATION pbr = pBaseRelocs;

        while ((unsigned)pbr - (unsigned)pBaseRelocs + 8 < cbBaseRelocs /* 8= VirtualAddress and SizeOfBlock members */
               && pbr->SizeOfBlock >= 8
               && pbr->VirtualAddress < ulRVAPage + PAGESIZE)
        {

            if (pbr->VirtualAddress + PAGESIZE >= ulRVAPage)
            {
                PWORD pwoffFixup   = &pbr->TypeOffset[0];
                ULONG cRelocations = (pbr->SizeOfBlock - offsetof(IMAGE_BASE_RELOCATION, TypeOffset)) / sizeof(WORD); /* note that sizeof(BaseReloc) is 12 bytes! */

                /* Some bound checking just to be sure it works... */
                if ((unsigned)pbr - (unsigned)pBaseRelocs + pbr->SizeOfBlock > cbBaseRelocs)
                {
                    printWar(("Block ends after BaseRelocation datadirectory.\n"));
                    cRelocations = (((unsigned)pBaseRelocs + cbBaseRelocs) - (unsigned)pbr - offsetof(IMAGE_BASE_RELOCATION, TypeOffset)) / sizeof(WORD);
                }


                /*
                 * skip to near end of previous page to save time
                 * I just don't dare to skip to the second last offset,
                 * 7 offsets from the end sounds nice and secure.
                 */
                if (pbr->VirtualAddress < ulRVAPage && cRelocations > 7)
                {
                    pwoffFixup += cRelocations - 7; /*  */
                    cRelocations = 7;
                }


                /*
                 * Loop thru the fixups in this chunk.
                 */
                while (cRelocations != 0)
                {
                    /*
                     * Fixup size table. 0xFF for fixups which aren't supported
                     * or is just non-sense.
                     */
                    #define CBFIXUPMAX 4
                    static char acbFixupTypes[16] = {0xFF, 0x02, 0x02, 0x04,
                                                     0xFF, 0xFF, 0xFF, 0xFF,
                                                     0xFF, 0xFF, 0xFF, 0xFF,
                                                     0xFF, 0xFF, 0xFF, 0xFF};
                    int   offFixup  = *pwoffFixup & (int)(PAGESIZE-1);
                    int   fType     = *pwoffFixup >> 12;
                    int   cbFixup   = acbFixupTypes[fType];

                    if (cbFixup <= CBFIXUPMAX
                        && offFixup + pbr->VirtualAddress + (cbFixup-1) >= ulRVAPage
                        && offFixup + pbr->VirtualAddress < ulRVAPage + PAGESIZE
                        )
                    {
                        ULONG       iObj;
                        ULONG       ulTarget;
                        ULONG       ul;         /* Crosspage fixups: Content of fixup target */

                        if (offFixup + pbr->VirtualAddress < ulRVAPage)
                        {   /*
                             * Crosspagefixup - from the page before
                             *  Start by reading the bytes on the previous page. We have to in case
                             *  of carray.
                             */
                            //printInf(("Crosspagefixup at offset=%d type=%d\n", offFixup - PAGESIZE, fType));
                            printf("Crosspagefixup at offset=%d type=%d\n", offFixup - PAGESIZE, fType);
                            rc = readAtRVA(ulRVAPage + offFixup - PAGESIZE, SSToDS(&ul), sizeof(ul));
                            if (rc != NO_ERROR)
                            {
                                printErr(("readAtRVA(0x%08x, ul(Before)..) failed with rc=%d\n", ulRVAPage + offFixup - PAGESIZE, rc));
                                return rc;
                            }

                            /*
                             * If none-delta and highlow fixup try resolve target address.
                             * Falls back on delta fixup. We have to do this because of
                             * object alignment problems.
                             */
                            iObj = ~0UL;
                            if (!fDeltaOnly && fType == IMAGE_REL_BASED_HIGHLOW)
                            {
                                /* Get target pointer, find it's object and apply the fixup */
                                ulTarget = ul - ulImageBase;        /* ulTarget is now an RVA */
                                iObj = 0UL;
                                while (iObj < cObjects
                                       && ulTarget >= (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                                                      : ALIGN(paObjects[iObj].cbVirtual, PAGESIZE) + paObjects[iObj].ulRVA)
                                       )
                                    iObj++;
                                if (iObj < cObjects)
                                    ul = pSMTE->smte_objtab[iObj].ote_base + ulTarget - paObjects[iObj].ulRVA;
                            }

                            /*
                             * Apply delta fixup.
                             */
                            if (iObj >= cObjects)
                            {
                                switch (fType)
                                {
                                    case IMAGE_REL_BASED_LOW:
                                    case IMAGE_REL_BASED_HIGHLOW:
                                        ul += ulDelta;
                                        break;
                                    case IMAGE_REL_BASED_HIGH:
                                    case IMAGE_REL_BASED_HIGHADJ:  /* According to M$ docs these seems to be the same fixups. */
                                    case IMAGE_REL_BASED_HIGH3ADJ:
                                        ul += ulDelta >> 16;
                                        break;
                                    case IMAGE_REL_BASED_ABSOLUTE:
                                        break;
                                    default:
                                        printErr(("Unknown fixup type %d offset=%0x%08x\n", fType, offFixup));
                                }
                            }

                            /*
                             * Copy the needed fixup data from ul to the page.
                             */
                            if (cbFixup <= CBFIXUPMAX)
                                memcpy(pvPage, (char*)SSToDS(&ul) + PAGESIZE - offFixup, (size_t)(cbFixup - PAGESIZE + offFixup));
                        }
                        else if (offFixup + pbr->VirtualAddress + cbFixup > ulRVAPage + PAGESIZE)
                        {   /*
                             * Crosspagefixup - into the page afterwards
                             */
                            printInf(("Crosspagefixup at offset=%d type=%d\n", PAGESIZE-offFixup, fType));

                            /*
                             * If none-delta and highlow fixup then read the full 4 bytes so we can
                             * resolve the target address and fix it.
                             * If we fail to fix it we'll fall back on delta fixup.
                             */
                            iObj = ~0UL;
                            if (!fDeltaOnly && fType == IMAGE_REL_BASED_HIGHLOW)
                            {
                                rc = readAtRVA(ulRVAPage +  offFixup, SSToDS(&ul), sizeof(ul));
                                if (rc != NO_ERROR)
                                {
                                    printErr(("readAtRVA(0x%08x, ul(After)..) failed with rc=%d\n", ulRVAPage+offFixup, rc));
                                    return rc;
                                }

                                /* Get target pointer, find it's object and apply the fixup */
                                ulTarget = ul - ulImageBase; /* ulTarget is now an RVA */
                                iObj = 0UL;
                                while (iObj < cObjects
                                       && ulTarget >= (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                                                      : ALIGN(paObjects[iObj].cbVirtual, PAGESIZE) + paObjects[iObj].ulRVA)
                                       )
                                    iObj++;
                                if (iObj < cObjects)
                                    ul = pSMTE->smte_objtab[iObj].ote_base + ulTarget - paObjects[iObj].ulRVA;
                            }

                            /*
                             * Apply delta fixup.
                             */
                            if (iObj >= cObjects)
                            {
                                ul = 0;
                                memcpy(SSToDS(&ul), (char*)pvPage + offFixup, (size_t)(PAGESIZE - offFixup));
                                switch (fType)
                                {
                                    case IMAGE_REL_BASED_LOW:
                                    case IMAGE_REL_BASED_HIGHLOW:
                                        ul += ulDelta;
                                        break;
                                    case IMAGE_REL_BASED_HIGH:
                                    case IMAGE_REL_BASED_HIGHADJ:  /* According to M$ docs these seems to be the same fixups. */
                                    case IMAGE_REL_BASED_HIGH3ADJ:
                                        ul += ulDelta >> 8;
                                        break;
                                    case IMAGE_REL_BASED_ABSOLUTE:
                                        break;
                                    default:
                                        printErr(("Unknown fixup type %d offset=%0x%08x\n", fType, offFixup));
                                }
                            }

                            /*
                             * Copy the needed fixup data from ul to the page.
                             */
                            if (cbFixup <= CBFIXUPMAX)
                                memcpy((char*)pvPage + offFixup, (char*)SSToDS(&ul), (size_t)(PAGESIZE - offFixup));
                        }
                        else
                        {   /*
                             * Common fixup
                             */
                            PULONG      pul;        /* Pointer to fixup target */
                            pul = (PULONG)((unsigned)pvPage + offFixup + pbr->VirtualAddress - ulRVAPage);
                            switch (fType)
                            {
                                case IMAGE_REL_BASED_HIGHLOW:
                                    printInfA(("IMAGE_REL_BASED_HIGHLOW offset=0x%08x\n", offFixup));
                                    if (fDeltaOnly)
                                        *pul += ulDelta;
                                    else
                                    {
                                        ulTarget = *pul - ulImageBase;  /* ulTarget is now an RVA */
                                        iObj = 0UL;
                                        while (iObj < cObjects
                                               && ulTarget >= (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                                                              : ALIGN(paObjects[iObj].cbVirtual, PAGESIZE) + paObjects[iObj].ulRVA)
                                               )
                                            iObj++;
                                        if (iObj < cObjects)
                                            *pul = pSMTE->smte_objtab[iObj].ote_base + ulTarget - paObjects[iObj].ulRVA;
                                        else
                                            *pul += ulDelta;
                                    }
                                    break;

                                /* Placeholder. */
                                case IMAGE_REL_BASED_ABSOLUTE:
                                    break;

                                /* Will probably not work very well until the 64KB object alignment is gone! */
                                case IMAGE_REL_BASED_HIGH:
                                case IMAGE_REL_BASED_HIGHADJ:   /* According to M$ docs these seems to be the same fixups. */
                                case IMAGE_REL_BASED_HIGH3ADJ:
                                    printInfA(("IMAGE_REL_BASED_HIGH offset=0x%08x\n", offFixup));
                                    *(PUSHORT)pul += (USHORT)(ulDelta >> 16);
                                    break;
                                /* Will probably not work very well until the 64KB object alignment is gone! */
                                case IMAGE_REL_BASED_LOW:
                                    printInfA(("IMAGE_REL_BASED_LOW  offset=0x%08x\n", offFixup));
                                    *(PUSHORT)pul += (USHORT)ulDelta;
                                    break;
                                default:
                                    printErr(("Unknown fixup type %d offset=%0x%08x\n", fType, offFixup));
                            }
                        }
                    }

                    /*
                     * Next offset/type
                     */
                    pwoffFixup++;
                    cRelocations--;
                } /* while loop */

            } /* else: not touching this page */

            /*
             * Next Fixup chunk. (i.e. next page)
             */
            pbr = (PIMAGE_BASE_RELOCATION)((unsigned)pbr + pbr->SizeOfBlock);
        } /* while loop */
    } /* else: don't need to apply fixups */
    NOREF(iPageTable);
    NOREF(pvPTDA);

    return NO_ERROR;
}


/**
 * applyFixups worker used when only deltas are to be applied.
 */
ULONG  Pe2Lx::applyFixupsDelta(PVOID pvPage, ULONG ulDelta, ULONG ulRVAPage)
{
    /*
     * Iterate thru the fixup chunks until we find one or two with fixups valid for
     * this page. I say one or two since we might have cross page fixups from the previous page
     * So, we'll have to start working at the previous page.
     * ASSUME: -fixups are sorted ascendingly on page RVA. only one chunk per page.
     *         -only one cross page fixup at each end which doesn't overlapp with other fixups.
     */
    PIMAGE_BASE_RELOCATION pbr = pBaseRelocs;

    while ((unsigned)pbr - (unsigned)pBaseRelocs + 8 < cbBaseRelocs /* 8= VirtualAddress and SizeOfBlock members */
           && pbr->SizeOfBlock >= 8
           && pbr->VirtualAddress < ulRVAPage + PAGESIZE)
    {

        if (pbr->VirtualAddress + PAGESIZE >= ulRVAPage)
        {
            PWORD pwoffFixup   = &pbr->TypeOffset[0];
            ULONG cRelocations = (pbr->SizeOfBlock - offsetof(IMAGE_BASE_RELOCATION, TypeOffset)) / sizeof(WORD); /* note that sizeof(BaseReloc) is 12 bytes! */

            /* Some bound checking just to be sure it works... */
            if ((unsigned)pbr - (unsigned)pBaseRelocs + pbr->SizeOfBlock > cbBaseRelocs)
            {
                printWar(("Block ends after BaseRelocation datadirectory.\n"));
                cRelocations = (((unsigned)pBaseRelocs + cbBaseRelocs) - (unsigned)pbr - offsetof(IMAGE_BASE_RELOCATION, TypeOffset)) / sizeof(WORD);
            }


            /*
             * skip to near end of previous page to save time
             * I just don't dare to skip to the second last offset,
             * 7 offsets from the end sounds nice and secure.
             */
            if (pbr->VirtualAddress < ulRVAPage && cRelocations > 7)
            {
                pwoffFixup += cRelocations - 7; /*  */
                cRelocations = 7;
            }


            /*
             * Loop thru the fixups in this chunk.
             */
            while (cRelocations != 0)
            {
                /*
                 * Fixup size table. 0xFF for fixups which aren't supported
                 * or is just non-sense.
                 */
                #define CBFIXUPMAX 4
                static char acbFixupTypes[16] = {0xFF, 0x02, 0x02, 0x04,
                                                 0xFF, 0xFF, 0xFF, 0xFF,
                                                 0xFF, 0xFF, 0xFF, 0xFF,
                                                 0xFF, 0xFF, 0xFF, 0xFF};
                int   offFixup  = *pwoffFixup & (int)(PAGESIZE-1);
                int   fType     = *pwoffFixup >> 12;
                int   cbFixup   = acbFixupTypes[fType];

                if (cbFixup <= CBFIXUPMAX
                    && offFixup + pbr->VirtualAddress + (cbFixup-1) >= ulRVAPage
                    && offFixup + pbr->VirtualAddress < ulRVAPage + PAGESIZE
                    )
                {
                    ULONG       ul;         /* Crosspage fixups: Content of fixup target */

                    if (offFixup + pbr->VirtualAddress < ulRVAPage)
                    {   /*
                         * Crosspagefixup - from the page before
                         *  Start by reading the bytes on the previous page. We have to in case
                         *  of carray.
                         */
                        printInf(("Crosspagefixup at offset=%d type=%d\n", offFixup - PAGESIZE, fType));
                        APIRET rc = readAtRVA(ulRVAPage + offFixup - PAGESIZE, SSToDS(&ul), sizeof(ul));
                        if (rc != NO_ERROR)
                        {
                            printErr(("readAtRVA(0x%08x, ul(Before)..) failed with rc=%d\n", ulRVAPage + offFixup - PAGESIZE, rc));
                            return rc;
                        }

                        /*
                         * Apply delta fixup.
                         */
                        switch (fType)
                        {
                            case IMAGE_REL_BASED_LOW:
                            case IMAGE_REL_BASED_HIGHLOW:
                                ul += ulDelta;
                                break;
                            case IMAGE_REL_BASED_HIGH:
                            case IMAGE_REL_BASED_HIGHADJ:  /* According to M$ docs these seems to be the same fixups. */
                            case IMAGE_REL_BASED_HIGH3ADJ:
                                ul += ulDelta >> 16;
                                break;
                            case IMAGE_REL_BASED_ABSOLUTE:
                                break;
                            default:
                                printErr(("Unknown fixup type %d offset=%0x%08x\n", fType, offFixup));
                        }

                        /*
                         * Copy the needed fixup data from ul to the page.
                         */
                        if (cbFixup <= CBFIXUPMAX)
                            memcpy(pvPage, (char*)SSToDS(&ul) + PAGESIZE - offFixup, (size_t)(cbFixup - PAGESIZE + offFixup));
                    }
                    else if (offFixup + pbr->VirtualAddress + cbFixup > ulRVAPage + PAGESIZE)
                    {   /*
                         * Crosspagefixup - into the page afterwards
                         */
                        printInf(("Crosspagefixup at offset=%d type=%d\n", PAGESIZE-offFixup, fType));

                        /*
                         * Apply delta fixup.
                         */
                        ul = 0;
                        memcpy(SSToDS(&ul), (char*)pvPage + offFixup, (size_t)(PAGESIZE - offFixup));
                        switch (fType)
                        {
                            case IMAGE_REL_BASED_LOW:
                            case IMAGE_REL_BASED_HIGHLOW:
                                ul += ulDelta;
                                break;
                            case IMAGE_REL_BASED_HIGH:
                            case IMAGE_REL_BASED_HIGHADJ:  /* According to M$ docs these seems to be the same fixups. */
                            case IMAGE_REL_BASED_HIGH3ADJ:
                                ul += ulDelta >> 8;
                                break;
                            case IMAGE_REL_BASED_ABSOLUTE:
                                break;
                            default:
                                printErr(("Unknown fixup type %d offset=%0x%08x\n", fType, offFixup));
                        }

                        /*
                         * Copy the needed fixup data from ul to the page.
                         */
                        if (cbFixup <= CBFIXUPMAX)
                            memcpy((char*)pvPage + offFixup, (char*)SSToDS(&ul), (size_t)(PAGESIZE - offFixup));
                    }
                    else
                    {   /*
                         * Common fixup
                         */
                        PULONG      pul;        /* Pointer to fixup target */
                        pul = (PULONG)((unsigned)pvPage + offFixup + pbr->VirtualAddress - ulRVAPage);
                        switch (fType)
                        {
                            case IMAGE_REL_BASED_HIGHLOW:
                                printInfA(("IMAGE_REL_BASED_HIGHLOW offset=0x%08x\n", offFixup));
                                *pul += ulDelta;
                                break;
                            /* Placeholder. */
                            case IMAGE_REL_BASED_ABSOLUTE:
                                break;
                            /* Will probably not work very well until the 64KB object alignment is gone! */
                            case IMAGE_REL_BASED_HIGH:
                            case IMAGE_REL_BASED_HIGHADJ:   /* According to M$ docs these seems to be the same fixups. */
                            case IMAGE_REL_BASED_HIGH3ADJ:
                                printInfA(("IMAGE_REL_BASED_HIGH offset=0x%08x\n", offFixup));
                                *(PUSHORT)pul += (USHORT)(ulDelta >> 16);
                                break;
                            /* Will probably not work very well until the 64KB object alignment is gone! */
                            case IMAGE_REL_BASED_LOW:
                                printInfA(("IMAGE_REL_BASED_LOW  offset=0x%08x\n", offFixup));
                                *(PUSHORT)pul += (USHORT)ulDelta;
                                break;
                            default:
                                printErr(("Unknown fixup type %d offset=%0x%08x\n", fType, offFixup));
                        }
                    }
                }

                /*
                 * Next offset/type
                 */
                pwoffFixup++;
                cRelocations--;
            } /* while loop */

        } /* else: not touching this page */

        /*
         * Next Fixup chunk. (i.e. next page)
         */
        pbr = (PIMAGE_BASE_RELOCATION)((unsigned)pbr + pbr->SizeOfBlock);
    } /* while loop */

    return NO_ERROR;
}

/**
 * openPath - opens file eventually searching loader specific paths.
 * This method is only called for DLLs. (DosLoadModule and Imports.)
 *
 *
 * @returns   OS2 return code.
 *            pLdrLv->lv_sfn  is set to filename handle.
 * @param     pachFilename  Pointer to filename. Not zero terminated!
 * @param     cchFilename   Filename length.
 * @param     pLdrLv        Loader local variables? (Struct from KERNEL.SDF)
 * @param     pful          Pointer to flags which are passed on to ldrOpen.
 * @sketch
 * This is roughly what the original ldrOpenPath does:
 *      if !CLASS_GLOBAL or miniifs then
 *          ldrOpen(pachModName)
 *      else
 *          loop until no more libpath elements
 *              get next libpath element and add it to the modname.
 *              try open the modname
 *              if successfull then break the loop.
 *          endloop
 *      endif
 */
ULONG  Pe2Lx::openPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *pLdrLv, PULONG pful, ULONG lLibPath) /* (ldrOpenPath) */
{
    #ifdef RING0

    /*
     * Mark the SFN invalid in the case of error.
     * Initiate the Odin32 Path static variable and call worker.
     */
    #ifdef DEBUG
    APIRET rc;
    //kprintf(("pe2lx::openPath(%.*s, %d, %x, %x, %x)\n",
    //         cchFilename, pachFilename, cchFilename, pLdrLv, pful, lLibPath));
    rc = openPath2(pachFilename, cchFilename, pLdrLv, pful, lLibPath, initOdin32Path());
    //kprintf(("pe2lx::openPath rc=%d\n", rc));
    return rc;
    #else
    return openPath2(pachFilename, cchFilename, pLdrLv, pful, lLibPath, initOdin32Path());
    #endif

    #else
    NOREF(pachFilename);
    NOREF(cchFilename);
    NOREF(pLdrLv);
    NOREF(pful);
    NOREF(lLibPath);
    return ERROR_NOT_SUPPORTED;
    #endif
}


/**
 * openPath2 - Worker for openPath which is also used by initOdin32Path.
 *
 * @returns   OS2 return code.
 *            pLdrLv->lv_sfn  is set to filename handle.
 * @param     pachFilename      Pointer to filename. Not zero terminated!
 * @param     cchFilename       Filename length.
 * @param     pLdrLv            Loader local variables? (Struct from KERNEL.SDF)
 * @param     pful              Pointer to flags which are passed on to ldrOpen.
 * @param     fOdin32PathValid  Flag indicating that the pszOdin32Path is valid or not.
 * @sketch
 * This is roughly what the original ldrOpenPath does:
 *      if !CLASS_GLOBAL or miniifs then
 *          ldrOpen(pachModName)
 *      else
 *          loop until no more libpath elements
 *              get next libpath element and add it to the modname.
 *              try open the modname
 *              if successfull then break the loop.
 *          endloop
 *      endif
 * @remark      cchFilename has to be ULONG due to an optimization bug in VA 3.08.
 *              (cchFilename should have been USHORT. But, then the compiler would
 *               treat it as an ULONG.)
 *              TODO: Support quotes.
 */
ULONG  Pe2Lx::openPath2(PCHAR pachFilename, ULONG cchFilename, ldrlv_t *pLdrLv, PULONG pful, ULONG lLibPath, BOOL fOdin32PathValid)
{
    #ifdef RING0

    APIRET  rc;                         /* Returncode. */
    ULONG   cchExt;                     /* Count of chars in additional extention. (0 if extention exists.) */

    /* These defines sets the order the paths and pathlists are examined. */
    #define FINDDLL_EXECUTABLEDIR   1
    #define FINDDLL_CURRENTDIR      2
    #define FINDDLL_SYSTEM32DIR     3
    #define FINDDLL_SYSTEM16DIR     4
    #define FINDDLL_WINDIR          5
    #define FINDDLL_PATH            6
    #define FINDDLL_BEGINLIBPATH    7   /* uses ldrOpenPath */
    #define FINDDLL_LIBPATH         8   /* uses ldrOpenPath */
    #define FINDDLL_ENDLIBPATH      9   /* uses ldrOpenPath */
    #define FINDDLL_FIRST           FINDDLL_EXECUTABLEDIR
    #define FINDDLL_LAST            FINDDLL_ENDLIBPATH

    struct _LocalVars
    {
        char    sz[CCHMAXPATH];
        char    szPath[CCHMAXPATH];
    } *pVars;


    /** @sketch
     * Mark the SFN invalid in the case of error.
     * Allocate memory for local variables.
     * Check for extention.
     */
    pLdrLv->lv_sfn = 0xffff;
    pVars = (struct _LocalVars*)rmalloc(sizeof(struct _LocalVars));
    if (pVars == NULL)
    {
        printErr(("openPath2: rmalloc failed\n"));
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    cchExt = cchFilename - 1;
    while (cchExt != 0 && pachFilename[cchExt] != '.')
        cchExt--;
    cchExt = cchExt != 0 ? 0 : 4;


    /** @sketch
     * Loop thru the paths and pathlists searching them for the filename.
     */
    for (int iPath = FINDDLL_FIRST; iPath <= FINDDLL_LAST; iPath++)
    {
        char  * pszPath;                /* Pointer to the path being examined. */

        /** @sketch
         * Get the path/dir to examin. (This is determined by the value if iPath.)
         */
        switch (iPath)
        {
            case FINDDLL_EXECUTABLEDIR:
                if ((pszPath = ldrGetExePath(pVars->szPath, TRUE)) == NULL)
                {
                    //kprintf(("openPath2: failed to find exe path.\n")); //DEBUG
                    continue;
                }
                break;

            case FINDDLL_CURRENTDIR:
                pszPath = ".";
                break;

            case FINDDLL_SYSTEM32DIR:
                if (!fOdin32PathValid)
                {
                    //kprintf(("openPath2: invalid odin32 paths.\n"));
                    continue;
                }
                pszPath = pVars->szPath;
                strcpy(pszPath, pszOdin32Path);
                strcpy(pszPath + cchOdin32Path, "System32");
                break;

            case FINDDLL_SYSTEM16DIR:
                if (!fOdin32PathValid)
                {
                    //kprintf(("openPath2: invalid odin32 paths.\n"));
                    continue;
                }
                pszPath = pVars->szPath;
                strcpy(pszPath, pszOdin32Path);
                strcpy(pszPath + cchOdin32Path, "System");
                break;

            case FINDDLL_WINDIR:
                if (!fOdin32PathValid)
                {
                    //kprintf(("openPath2: invalid odin32 paths.\n"));
                    continue;
                }
                pszPath = pVars->szPath;
                strcpy(pszPath, pszOdin32Path);
                pszPath[cchOdin32Path - 1] = '\0'; /* remove slash */
                break;

            case FINDDLL_PATH:
            {
                PPTD    pptd = GetTaskData(NULL, FALSE);
                pszPath = NULL;
                if (pptd)
                    pszPath = pptd->pszzOdin32Env;
                if (!pszPath)
                    pszPath = (char*)GetEnv(TRUE);
                if (pszPath == NULL)
                {
                    //kprintf(("openPath2: failed to GetEnv.\n"));
                    continue;
                }
                pszPath = (char*)ScanEnv(pszPath, "PATH");
                break;
            }

            #if 1
            case FINDDLL_BEGINLIBPATH:
                pszPath = NULL;
                if (ptdaGetCur())
                    pszPath = ptdaGet_ptda_pBeginLIBPATH(ptdaGetCur());
                if (pszPath == NULL)
                    continue;
                break;

            case FINDDLL_LIBPATH:
                pszPath = *pLdrLibPath;
                break;

            case FINDDLL_ENDLIBPATH:
                pszPath = NULL;
                if (ptdaGetCur())
                    pszPath = ptdaGet_ptda_pEndLIBPATH(ptdaGetCur());
                if (pszPath == NULL)
                    continue;
                break;
            #endif

            default: /* !internalerror! */
                printIPE(("Pe2Lx::openPath(%.*s,..): iPath is %d which is invalid.\n", cchFilename, pachFilename, iPath));
                rfree(pVars);
                return ERROR_FILE_NOT_FOUND;
        }

        //kprintf(("openPath2: level:%d path=%s\n", iPath, pszPath));

        /** @sketch
         * pszPath is now set to the pathlist to be searched.
         * So we'll loop thru all the paths in the list.
         */
        while (pszPath != NULL && *pszPath != '\0')
        {
            char *  pszNext;            /* Pointer to the next pathlist path */
            int     cch;                /* Length of path (including the slash after the slash is added). */

            /** @sketch
             * Find the end of the path and set pszNext.
             * Uncount any trailing slash.
             * Check that the filename fits within the buffer (and OS/2 filelength limits).
             */
            pszNext = strchr(pszPath, ';');
            if (pszNext != NULL)
            {
                cch = pszNext - pszPath;
                pszNext++;
            }
            else
                cch = strlen(pszPath);

            if (pszPath[cch - 1] == '\\' || pszPath[cch-1] == '/')
                cch--;

            if (cch == 0 || cch + cchFilename + 2 + cchExt > sizeof(pVars->sz)) /* assertion */
            {
                printErr(("Pe2Lx::openPath(%.*s,..): cch (%d) + cchFilename (%d) + 2 + cchExt (%d) > sizeof(pVars->sz) (%d) - path's too long!, iPath=%d",
                          cchFilename, pachFilename, cch, cchExt, cchFilename, sizeof(pVars->sz), iPath));

                pszPath = pszNext;
                continue;
            }


            /** @sketch
             * Copy the path into the pVars->sz buffer.
             * Add a '\\' and the filename (pszFullname) to the path;
             * then we'll have a fullpath.
             */
            memcpy(pVars->sz, pszPath, cch);
            pVars->sz[cch++] = '\\';
            memcpy(&pVars->sz[cch], pachFilename, (size_t)cchFilename);
            if (cchExt != 0)
                memcpy(&pVars->sz[cch + cchFilename], ".DLL", 5);
            else
                pVars->sz[cch + cchFilename] = '\0';


            /** @sketch
             * Try open the file using myLdrOpen.
             * Return if successfully opened or if fatal error.
             */
            rc = myldrOpen(&pLdrLv->lv_sfn, pVars->sz, pful);
            switch (rc)
            {
                /* these errors are ignored (not fatal) */
                case ERROR_FILE_NOT_FOUND:          case ERROR_PATH_NOT_FOUND:          case ERROR_ACCESS_DENIED:           case ERROR_INVALID_ACCESS:
                case ERROR_INVALID_DRIVE:           case ERROR_NOT_DOS_DISK:            case ERROR_REM_NOT_LIST:            case ERROR_BAD_NETPATH:
                case ERROR_NETWORK_BUSY:            case ERROR_DEV_NOT_EXIST:           case ERROR_TOO_MANY_CMDS:           case ERROR_ADAP_HDW_ERR:
                case ERROR_UNEXP_NET_ERR:           case ERROR_BAD_REM_ADAP:            case ERROR_NETNAME_DELETED:         case ERROR_BAD_DEV_TYPE:
                case ERROR_NETWORK_ACCESS_DENIED:   case ERROR_BAD_NET_NAME:            case ERROR_TOO_MANY_SESS:           case ERROR_REQ_NOT_ACCEP:
                case ERROR_INVALID_PASSWORD:        case ERROR_OPEN_FAILED:             case ERROR_INVALID_NAME:            case ERROR_FILENAME_EXCED_RANGE:
                case ERROR_VC_DISCONNECTED:         case ERROR_DRIVE_LOCKED:
                    rc = ERROR_FILE_NOT_FOUND;
                    pszPath = pszNext;
                    break;

                /* all errors and success is let out here */
                case NO_ERROR:
                default:
                    rfree(pVars);
                    return rc;
            }

            /** @sketch
             * Advance to the next path part
             */
            pszPath = pszNext;
        }
    } /* for iPath */


    /*
     * Cleanup: free local variables.
     * Since we haven't found the file yet we'll return thru ldrOpenPath.
     */
    rfree(pVars);
    return ERROR_FILE_NOT_FOUND;//ldrOpenPath(pachFilename, (USHORT)cchFilename, pLdrLv, pful, lLibPath);

    #else
    NOREF(pachFilename);
    NOREF(cchFilename);
    NOREF(pLdrLv);
    NOREF(pful);
    NOREF(lLibPath);
    NOREF(fOdin32PathValid);
    return ERROR_NOT_SUPPORTED;
    #endif
}


#ifndef RING0
/**
 * This method test the applyFixups method.
 * @returns   Last rc from applyFixups.
 * @status
 * @author    knut st. osmundsen
 * @remark    Testing only...
 */
ULONG Pe2Lx::testApplyFixups()
{
    static SMTE smte;
    static MTE  mte;
    static      achPage[PAGESIZE];
    int         i;
    APIRET      rc;

    mte.mte_swapmte = &smte;
    smte.smte_objcnt = cObjects;
    smte.smte_objtab = (POTE)malloc(cObjects * sizeof(OTE));
    makeObjectTable();
    memcpy(smte.smte_objtab, paObjTab, sizeof(OTE) * cObjects);
    smte.smte_objtab[0].ote_base = 0x132d0000;
    for (i = 1; i < cObjects; i++)
        smte.smte_objtab[i].ote_base = ALIGN(smte.smte_objtab[i-1].ote_size + smte.smte_objtab[i-1].ote_base, 0x10000);

    rc = loadBaseRelocations();
    if (rc != NO_ERROR)
    {
        printErr(("loadBaseRelocations failed with rc=%d\n", rc));
        return rc;
    }

    /*
     * Test load and apply all (internal) fixups.
     */
    for (i = 0; i < cObjects; i++)
    {
        ULONG ulAddress = smte.smte_objtab[i].ote_base;
        ULONG ulRVA = paObjects[i].ulRVA;
        LONG  cbObject = paObjects[i].cbVirtual;
        for (i=i; cbObject > 0; cbObject -= PAGESIZE, ulAddress += PAGESIZE, ulRVA += PAGESIZE)
        {
            printInf(("Page at RVA 0x%08x\n", ulRVA));
            rc = readAtRVA(ulRVA, &achPage[0], PAGESIZE);
            if (rc != NO_ERROR)
            {
                printErr(("readAtRVA failed with rc=%d\n"));
                return rc;
            }
            rc = applyFixups(&mte, i, ulRVA >> PAGESHIFT, &achPage[0], ulAddress, NULL);
            if (rc != NO_ERROR)
            {
                printErr(("applyFixups failed with rc=%d\n"));
                return rc;
            }
        }
    }

    return rc;
}



/**
 * Writes the virtual LX file to a file. (Ring 3 only!)
 * @returns   NO_ERROR on success. Error code on error.
 * @param     pszLXFilename  Pointer to name of the LX file.
 * @sketch    Find size of the virtual LX-file.
 *            Open the output file.
 *            LOOP while more to left of file
 *            BEGIN
 *                read into buffer from virtual LX-file.
 *                write to output file.
 *            END
 *            return success or errorcode.
 * @status    compeletely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG Pe2Lx::writeFile(PCSZ pszLXFilename)
{
    static CHAR achReadBuffer[65000];
    APIRET rc;
    ULONG  ulAction = 0;
    ULONG  ulWrote;
    HFILE  hLXFile = NULLHANDLE;
    ULONG  cbLXFile;
    ULONG  offLXFile;

    /* Find size of the virtual LX-file. */
    cbLXFile = querySizeOfLxFile();
    if (cbLXFile == ~0UL)
        return ERROR_BAD_EXE_FORMAT;

    printInf(("\n"));
    printInf(("Creating LX file - %s\n", pszLXFilename));
    printInf(("\n"));
    printInf(("Size of virtual LX-file: %d bytes\n", cbLXFile));

    /* Open the output file. */
    rc = DosOpen(pszLXFilename, &hLXFile, &ulAction, cbLXFile,
                 FILE_NORMAL,
                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                 OPEN_FLAGS_SEQUENTIAL | OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYWRITE | OPEN_ACCESS_WRITEONLY,
                 NULL);
    if (rc == NO_ERROR)
    {
        offLXFile = 0UL;
        while (cbLXFile > 0UL)
        {
            ULONG cbToRead = min(cbLXFile, sizeof(achReadBuffer));
            rc = read(offLXFile, &achReadBuffer[0], 0UL, cbToRead, NULL);
            if (rc != NO_ERROR)
            {
                printErr(("read failed with rc=%d.\n", rc));
                break;
            }
            /* write to output file. */
            rc = DosWrite(hLXFile, &achReadBuffer[0], cbToRead, &ulWrote);
            if (rc != NO_ERROR || ulWrote != cbToRead)
            {
                printErr(("DosWrite failed with rc=%d\n", rc));
                break;
            }

            /* move along */
            offLXFile += cbToRead;
            cbLXFile -= cbToRead;
        }
        DosClose(hLXFile);
    }
    else
        printErr(("Failed to open output file, '%s', for writing. rc = %d\n",
                 pszLXFilename, rc));

    if (rc == NO_ERROR)
        printInf(("File created successfully.\n"));

    return rc;
}
#endif


/**
 * Is this module an executable?
 * @returns   TRUE if executable.
 *            FALSE if not an executable.
 * @sketch
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL    Pe2Lx::isExe()
{
    return ((this->LXHdr.e32_mflags & E32MODMASK) == E32MODEXE);
}


/**
 * Is this module an dynamic link library.
 * @returns   TRUE if dynamic link library.
 *            FALSE if not a dynamic link library.
 * @sketch
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL    Pe2Lx::isDll()
{
    return ((this->LXHdr.e32_mflags & E32MODMASK) == E32MODDLL);
}


/**
 * Invalidates the odin32path.
 * Called by ldrClose when the kernel32 handle is closed.
 * @sketch      Free path
 *              nullify path pointer and kernel32 handle.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
VOID Pe2Lx::invalidateOdin32Path()
{
    if (pszOdin32Path != NULL)
    {
        rfree((void*)pszOdin32Path);
        pszOdin32Path = NULL;
    }
    sfnKernel32 = NULLHANDLE;
}


/**
 * Gets the size of the virtual LX-file.
 * @returns   Size of the virtual LX-file in bytes.
 *            ~0UL on failure.
 * @sketch    Find last object with valid pages.
 *            IF not found THEN return error.
 *            return LX offset + physical size of object; ie. the size of the LX file.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Not called in during init.
 */
ULONG Pe2Lx::querySizeOfLxFile()
{
    LONG iObj;

    #ifdef DEBUG
    /* call-time test. */
    if (fInitTime)
    {
        printIPE(("querySizeOfLXFile should not be called during init!\n"));
        return ~0UL;
    }
    #endif

    /* find last object with valid pages. */
    iObj = cObjects - 1;
    while (iObj >= 0 && paObjects[iObj].cbPhysical == 0UL)
        iObj--;

    /* check for impossible error. */
    if (iObj < 0)
    {
        printIPE(("This could not happen! No objects with valid pages!\n"));
        return ~0UL;
    }

    return paObjects[iObj].offLXFile + paObjects[iObj].cbPhysical;
}


/**
 * Dumps info on the virtual Lx file.
 * Currently it only dumps sizes and offsets.
 * @status    partially implemented.
 * @author    knut st. osmundsen
 */
VOID Pe2Lx::dumpVirtualLxFile()
{
    ULONG ul, ulIndex;
    ULONG off, cb, cbA;
    PCSZ  pszName;

    printInf(("\n"));
    printInf(("----- Pe2Lx::dumpVirtualLxFile() start -----\n"));

    /* Dump sizes */
    printInf(("\n"));
    ul = querySizeOfLxFile();
    printInf(("Size of Virtual LX file: %d (%#x)\n", ul, ul));
    for (ulIndex = 0; ulIndex <= 9; ulIndex++)
    {
        /* ASSUME a given order of tables! (See near bottom of the init() method) */
        switch (ulIndex)
        {
            case 0: /* LXHdr */
                pszName = "LX Header";
                off = 0UL;
                cb = sizeof(LXHdr);
                cbA = 0UL;
                break;
            case 1: /* Object Table */
                pszName = "Object Table";
                off = LXHdr.e32_objtab;
                cb = sizeof(struct o32_obj) * LXHdr.e32_objcnt;
                cbA = _msize(this->paObjTab);
                break;
            case 2: /* Object Page Table */
                pszName = "Object Page Table";
                off = LXHdr.e32_objmap;
                cb = LXHdr.e32_mpages * sizeof(struct o32_map);
                cbA = _msize(paObjPageTab);
                break;
            case 3: /* Resident Name Table */
                pszName = "Resident Name Table";
                off = LXHdr.e32_restab;
                cb = LXHdr.e32_enttab - off;
                cbA = _msize(pachResNameTable);
                break;
            case 4: /* Entry Table */
                pszName = "Entry Table";
                off = LXHdr.e32_enttab;
                cb = LXHdr.e32_fpagetab - off;
                cbA = _msize(pEntryBundles);
                break;
            case 5: /* FixupTable */
                pszName = "Fixup Page Table";
                off = LXHdr.e32_fpagetab;
                cb = LXHdr.e32_frectab - off;
                cbA = _msize(paulFixupPageTable);
                break;
            case 6: /* Fixup Record Table */
                pszName = "Fixup Record Table";
                off = LXHdr.e32_frectab;
                cb = LXHdr.e32_impmod - off;
                cbA = _msize(pFixupRecords);
                break;
            case 7: /* Import Module Name Table */
                pszName = "Import Module Name Table";
                off = LXHdr.e32_impmod;
                cb = LXHdr.e32_impproc - off;
                cbA = _msize(pachImpModuleNames);
                break;
            case 8: /* Import Procedure Name Table */
                pszName = "Import Procedure Name Table";
                off = LXHdr.e32_impproc;
                cb = LXHdr.e32_datapage - off;
                cbA = _msize(pachImpFunctionNames);
                break;
            case 9: /* data pages.*/
                pszName = "Data Pages";
                off = LXHdr.e32_datapage;
                cb = querySizeOfLxFile() - off;
                cbA = 0UL;
                break;
            default: printIPE(("invalid ulIndex. ulIndex = %d\n", ulIndex));
        }
        ul = strlen(pszName);
        printInf(("  %s %*s  off: %.6d (0x%08x)  size: %.6d (0x%08x) allocated: %.6d (0x%08x)\n",
                  pszName, ul > 30UL ? 0 : 30 - ul, "", off, off, cb, cb, cbA, cbA));
    }

    /* Size of Pe2Lx object. (heap size) */
    printInf(("\n"));
    printInf(("Size of Pe2Lx object on heap:\n"));
    for (ulIndex = 0UL, cbA = 0UL; ulIndex <= 13UL; ulIndex ++)
    {
        switch (ulIndex)
        {
            case 0:
                pszName = "The Pe2Lx Object";
                cb = sizeof(Pe2Lx);
                break;
            case 1:
                pszName = "pszFilename";
                cb = _msize(pszFilename);
                break;
            case 2:
                pszName = "paObjects";
                cb = _msize(paObjects);
                break;
            case 3:
                pszName = "paObjTab";
                cb = _msize(paObjTab);
                break;
            case 4:
                pszName = "paObjPageTab";
                cb = _msize(paObjPageTab);
                break;
            case 5:
                pszName = "pachResNameTable";
                cb = _msize(pachResNameTable);
                break;
            case 6:
                pszName = "pEntryBundles";
                cb = _msize(pEntryBundles);
                break;
            case 7:
                pszName = "paulFixupPageTable";
                cb = _msize(paulFixupPageTable);
                break;
            case 8:
                pszName = "pFixupRecords";
                cb = _msize(pFixupRecords);
                break;
            case 9:
                pszName = "pvCrossPageFixup";
                cb = _msize(pvCrossPageFixup);
                break;
            case 10:
                pszName = "pachImpModuleNames";
                cb = _msize(pachImpModuleNames);
                break;
            case 11:
                pszName = "pachImpFunctionNames";
                cb = _msize(pachImpFunctionNames);
                break;
            case 12:
                pszName = "pNtHdrs";
                cb = _msize(pNtHdrs);
                break;
            case 13:
                pszName = "total";
                cb = cbA;
                break;
            default: printIPE(("invalid ulIndex. ulIndex = %d\n", ulIndex));
        }

        ul = strlen(pszName);
        printInf(("  %s %*s size: %.6d (0x%08x)\n", pszName, ul >= 30UL ? 0 : 30 - ul, "", cb, cb));
        cbA += cb;
    }


    printInf(("----- Pe2Lx::dumpVirtualLxFile()  end  -----\n"));
}


/**
 * Adds a stack object.
 * The stack array is sorted ascending on ulRVA.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INITMETHOD_NOT_INITTIME (debug only)
 * @param     ulRVA        Virtual address of this object.
 * @param     cbPhysical   Physical size of the object.
 * @param     cbVirtual    Virtual size of the object.
 * @param     flFlags      LX object flags.
 * @param     offPEFile    Data offset into the PEFile.
 * @precond   We're in init-mode, ie. called from init().
 * @status    Completely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG Pe2Lx::addObject(ULONG ulRVA, ULONG cbPhysical, ULONG cbVirtual, ULONG flFlags, ULONG offPEFile)
{
    int i;
    #ifdef DEBUG
    if (!fInitTime)
    {
        printIPE(("addObject(,,,,) called when not in init mode!\n"));
        return ERROR_INITMETHOD_NOT_INITTIME;
    }
    #endif

    /* Check that there is a free entry in the array for the new object. If not allocate one (or more)! */
    if (cObjectsAllocated == 0UL)
    {
        cObjectsAllocated = (USHORT)(pNtHdrs == NULL ? 2 :
            pNtHdrs->FileHeader.NumberOfSections + (pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL ?  0 : 1));
        paObjects = (PLXOBJECT)malloc(sizeof(LXOBJECT) * cObjectsAllocated);
        if (paObjects == NULL)
        {
            cObjectsAllocated = 0;
            return ERROR_NOT_ENOUGH_MEMORY;
        }
    }
    else if (cObjectsAllocated == cObjects)
    {
        PLXOBJECT paObjTmp = (PLXOBJECT)realloc(paObjects, sizeof(LXOBJECT) * (cObjectsAllocated + 1));
        if (paObjTmp == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;
        paObjects = paObjTmp;
        cObjectsAllocated++;
    }

    /* insert sorted. Move objects after the new object. */
    for (i = cObjects; i > 0 && paObjects[i-1].ulRVA > ulRVA; i--)
        memcpy(&paObjects[i-1], &paObjects[i], sizeof(paObjects[0]));

    paObjects[i].ulRVA              = ulRVA;
    paObjects[i].cbPhysical         = cbPhysical;
    paObjects[i].cbVirtual          = cbVirtual;
    paObjects[i].flFlags            = flFlags;
    paObjects[i].Misc.offTIBFix     = 0UL;
    paObjects[i].Misc.fTIBFixObject = FALSE;
    paObjects[i].Misc.fStackObject  = FALSE;
    paObjects[i].offPEFile          = offPEFile;
    paObjects[i].offLXFile          = 0UL;
    cObjects++;

    return NO_ERROR;
}


/**
 * Adds a TIBFix object.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INITMETHOD_NOT_INITTIME (debug only)
 * @precond   We're in init-mode, ie. called from init().
 * @status    partially implemented.
 * @author    knut st. osmundsen
 * @remark    Possible problem: section alignment! FIXME!
 */
ULONG Pe2Lx::addTIBFixObject()
{
    APIRET rc;

    #ifdef DEBUG
    if (!fInitTime)
    {
        printIPE(("addTIBFixObject(,,,,) called when not in init mode!\n"));
        return ERROR_INITMETHOD_NOT_INITTIME;
    }
    #endif

    rc = addObject(cObjects == 0 ? ulImageBase
                   : ALIGN(paObjects[cObjects-1].ulRVA + paObjects[cObjects-1].cbVirtual, PAGESIZE),
                   SIZEOF_TIBFIX, SIZEOF_TIBFIX, OBJREAD | OBJBIGDEF | OBJEXEC, 0UL);

    if (rc == NO_ERROR)
        paObjects[cObjects-1].Misc.fTIBFixObject = TRUE;

    return rc;
}


/**
 * Adds a stack object.
 * @returns   NO_ERROR
 *            ERROR_INVALID_PARAMETER
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INITMETHOD_NOT_INITTIME (debug only)
 * @param     cbStack  Stack size.
 * @precond   The stack object may not be the first object. (cObjects != 0)
 *            We're in init-mode, ie. called from init().
 * @status    partly implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Possible problem: section alignment! FIXME!
 */
ULONG Pe2Lx::addStackObject(ULONG cbStack)
{
    APIRET rc;
    #ifdef DEBUG
    if (!fInitTime)
    {
        printIPE(("addStackObject(,,,,) called when not in init mode!\n"));
        rc = ERROR_INITMETHOD_NOT_INITTIME;
    }
    #endif

    if (cObjects != 0)
    {
        rc = addObject(ALIGN(paObjects[cObjects-1].ulRVA + paObjects[cObjects-1].cbVirtual, PAGESIZE),
                       0UL, cbStack, OBJREAD |  OBJWRITE | OBJBIGDEF, 0UL);
        if (rc == NO_ERROR)
            paObjects[cObjects-1].Misc.fStackObject = TRUE;
    }
    else
    {
        printIPE(("addStackObject(,,,,) called when cObjects == 0!\n"));
        rc = ERROR_INVALID_PARAMETER;
    }

    return rc;
}


/**
 * Creates the page table according to the current settings of paObjects array and fAllInOneModule.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 * @sketch    IF valid object table pointer THEN return successfully.
 *            (try) allocate memory for the object table.
 *            IF all in one object THEN make one big object which covers the entire range described in paObjects.
 *            ELSE loop tru paObject and create LX objects.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Object Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *  The number of entries in the Object Table is given by the # Objects in Module field in the
 *  linear EXE header.  Entries in the Object Table are numbered starting from one.  Each Object
 *  Table entry has the following format:
 *
 *            ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *        00h ³     VIRTUAL SIZE      ³    RELOC BASE ADDR    ³
 *            ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 *        08h ³     OBJECT FLAGS      ³    PAGE TABLE INDEX   ³
 *            ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 *        10h ³  # PAGE TABLE ENTRIES ³       RESERVED        ³
 *            ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *  Object Table
 *
 *     VIRTUAL SIZE = DD  Virtual memory size. This is the size of the object that will be
 *     allocated when the object is loaded.  The object data length must be less than or equal
 *     to the total size of the pages in the EXE file for the object. This memory size must also
 *     be large enough to contain all of the iterated data and uninitialized data in the EXE file.
 *
 *     RELOC BASE ADDR = DD Relocation Base Address. The relocation base address the object is
 *     currently relocated to. If the internal relocation fixups for the module have been removed,
 *     this is the address the object will be allocated at by the loader.
 *
 *     OBJECT FLAGS = DW  Flag bits for the object. The object flag bits have the following definitions.
 *
 *               0001h = Readable Object.
 *               0002h = Writable Object.
 *               0004h = Executable Object. The readable, writable and executable flags provide support
 *               for all possible protections.  In systems where all of these protections are not
 *               supported,  the loader will be responsible for making the appropriate protection match
 *               for the system.
 *
 *               0008h = Resource Object.
 *               0010h = Discardable Object.
 *               0020h = Object is Shared.
 *               0040h = Object has Preload Pages.
 *               0080h = Object has Invalid Pages.
 *               0100h = Object has Zero Filled Pages.
 *               0200h = Object is Resident (valid for VDDs, PDDs only).
 *               0300h = Object is Resident   Contiguous (VDDs, PDDs only).
 *               0400h = Object is Resident   'long-lockable' (VDDs, PDDs only).
 *               0800h = Reserved for system use.
 *               1000h = 16
 *               2000h = Big/Default Bit Setting (80x86 Specific). The 'big/default' bit , for data
 *               segments, controls the setting of the Big bit in the segment descriptor.  (The Big
 *               bit, or B-bit, determines whether ESP or SP is used as the stack pointer.)  For code
 *               segments, this bit controls the setting of the Default bit in the segment descriptor.
 *                (The Default bit, or D-bit, determines whether the default word size is 32-bits or
 *                16-bits.  It also affects the interpretation of the instruction stream.)
 *
 *               4000h = Object is conforming for code (80x86 Specific).
 *               8000h = Object I/O privilege level (80x86 Specific). Only used for 16
 *
 *     PAGE TABLE INDEX = DD  Object Page Table Index. This specifies the number of the first
 *     object page table entry for this object.  The object page table specifies where in the EXE
 *     file a page can be found for a given object and specifies per-page attributes.  The object
 *     table entries are ordered by logical page in the object table. In other words the object
 *     table entries are sorted based on the object page table index value.
 *
 *     # PAGE TABLE ENTRIES = DD  # of object page table entries for this object. Any logical
 *     pages at the end of an object that do not have an entry in the object page table associated
 *     with them are handled as zero filled or invalid pages by the loader.  When the last logical
 *     pages of an object are not specified with an object page table entry, they are treated as
 *     either zero filled pages or invalid pages based on the last entry in the object page table
 *     for that object. If the last entry was neither a zero filled or invalid page, then the
 *     additional pages are treated as zero filled pages.
 *
 *     RESERVED = DD  Reserved for future use. Must be set to zero.
 *
 * ------------
 *
 *  We have object page table entries for all pages! (we can optimize this later)
 *
 */
ULONG Pe2Lx::makeObjectTable()
{
    /* check if valid object table pointer. */
    if (paObjTab != NULL || cObjects == 0UL)
        return NO_ERROR;

    /* (try) allocate memory for the object table. */
    paObjTab = (struct o32_obj*)malloc(sizeof(struct o32_obj) * (fAllInOneObject ? 1 : cObjects));
    if (paObjTab != NULL)
    {
        if (fAllInOneObject)
        {
            paObjTab[0].o32_size     = paObjects[cObjects-1].ulRVA + paObjects[cObjects-1].cbVirtual;
            paObjTab[0].o32_base     = ulImageBase + paObjects[0].ulRVA;
            paObjTab[0].o32_flags    = OBJREAD | OBJWRITE | OBJBIGDEF;
            paObjTab[0].o32_pagemap  = 1;  /* 1 based */
            paObjTab[0].o32_mapsize  = ALIGN(paObjTab[0].o32_size, PAGESIZE) >> PAGESHIFT;
            paObjTab[0].o32_reserved = 0;
        }
        else
        {
            int    i;
            ULONG  ulPageMap = 1;
            for (i = 0; i < cObjects; i++)
            {
                paObjTab[i].o32_size     = paObjects[i].cbVirtual;
                paObjTab[i].o32_base     = ulImageBase + paObjects[i].ulRVA;
                paObjTab[i].o32_flags    = paObjects[i].flFlags;
                paObjTab[i].o32_pagemap  = ulPageMap;
                paObjTab[i].o32_mapsize  = ALIGN(paObjTab[i].o32_size, PAGESIZE) >> PAGESHIFT;
                paObjTab[i].o32_reserved = 0;
                ulPageMap += paObjTab[i].o32_mapsize;
            }
        }
    }
    else
        return ERROR_NOT_ENOUGH_MEMORY;

    return NO_ERROR;
}


/**
 * Creates the object page table.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 * @sketch    IF valid pointer or no objects THEN return successfully.
 *            Allocate memory.
 *            LOOP thru all pages/objects
 *            BEGIN
 *                IF current object offset within physical size THEN
 *                BEGIN
 *                    make a VALID page entry .
 *                    add the smaller of pagesize and physical size - object offset to offPageData.
 *                END
 *                ELSE make a ZEROED page entry. (remeber fAllInOneObject)
 *                next page/object.
 *            END
 *            return successfully.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Object Page Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *  The Object page table provides information about a logical page in an object. A logical page
 *  may be an enumerated page, a pseudo page or an iterated page. The structure of the object
 *  page table in conjunction with the structure of the object table allows for efficient access
 *  of a page when a page fault occurs, while still allowing the physical page data to be
 *  located in the preload page, demand load page or iterated data page sections in the linear
 *  EXE module. The logical page entries in the Object Page Table are numbered starting from one.
 *  The Object Page Table is parallel to the Fixup Page Table as they are both indexed by the
 *  logical page number.  Each Object Page Table entry has the following format:
 *
 *        63                     32 31       16 15         0
 *         ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *     00h ³    PAGE DATA OFFSET   ³ DATA SIZE ³   FLAGS   ³
 *         ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *  Object Page Table Entry
 *
 *     PAGE DATA OFFSET = DD  Offset to the page data in the EXE file. This field, when bit
 *     shifted left by the PAGE OFFSET SHIFT from the module header, specifies the offset from
 *     the beginning of the Preload Page section of the physical page data in the EXE file that
 *     corresponds to this logical page entry.  The page data may reside in the Preload Pages,
 *     Demand Load Pages or the Iterated Data Pages sections.  A page might not start at the next
 *     available alignment boundary. Extra padding is acceptable between pages as long as each
 *     page starts on an alignment boundary. For example, several alignment boundarys may be
 *     skipped in order to start a frequently accessed page on a sector boundary.  If the FLAGS
 *     field specifies that this is a Zero-Filled page then the PAGE DATA OFFSET field will
 *     contain a 0.  If the logical page is specified as an iterated data page, as indicated by
 *     the FLAGS field, then this field specifies the offset into the Iterated Data Pages section.
 *     The logical page number (Object Page Table index), is used to index the Fixup Page Table to
 *     find any fixups associated with the logical page.
 *
 *     DATA SIZE = DW  Number of bytes of data for this page. This field specifies the actual
 *     number of bytes that represent the page in the file.  If the PAGE SIZE field from the
 *     module header is greater than the value of this field and the FLAGS field indicates a Legal
 *     Physical Page, the remaining bytes are to be filled with zeros.  If the FLAGS field
 *     indicates an Iterated Data Page, the iterated data records will completely fill out the
 *     remainder.
 *
 *     FLAGS = DW  Attributes specifying characteristics of this logical page. The bit definitions
 *     for this word field follow,
 *
 *               00h = Legal Physical Page in the module (Offset from Preload Page Section).
 *               01h = Iterated Data Page (Offset from Iterated Data Pages Section).
 *               02h = Invalid Page (zero).
 *               03h = Zero Filled Page (zero).
 *               04h = Range of Pages.
 *               05h = Compressed Page (Offset from Preload Pages Section).
 *
 * -----------
 *
 * Not space optimized yet!
 *
 */
ULONG Pe2Lx::makeObjectPageTable()
{
    ULONG  cPages;
    ULONG  i;
    ULONG  iObj;            /* Index to Current object. */
    ULONG  offObject;       /* Offset of the current page into the object. */
    ULONG  offPageData;     /* Offset from e32_datapage in virtual LX file.  */

    /* check if valid object page table pointer or no objects */
    if (paObjPageTab != NULL || cObjects == 0)
        return NO_ERROR;

    /* allocate memory */
    cPages = getCountOfPages();
    paObjPageTab = (struct o32_map*)malloc((size_t)(cPages * sizeof(struct o32_map)));
    if (paObjPageTab == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    /* loop */
    iObj = 0UL;
    offObject = 0UL;
    offPageData = 0UL;
    for (i = 0UL; i < cPages; i++)
    {
        paObjPageTab[i].o32_pagedataoffset = offPageData;
        if (offObject < paObjects[iObj].cbPhysical)
        {
            paObjPageTab[i].o32_pagesize  = (USHORT)min(paObjects[iObj].cbPhysical - offObject, PAGESIZE);
            paObjPageTab[i].o32_pageflags = VALID;
            offPageData += min(paObjects[iObj].cbPhysical - offObject, PAGESIZE);
        }
        else
        {
            paObjPageTab[i].o32_pagesize  = (USHORT)(fAllInOneObject && iObj + 1UL < cObjects ?
                PAGESIZE : min(paObjects[iObj].cbVirtual - offObject, PAGESIZE));
            paObjPageTab[i].o32_pageflags = ZEROED;
        }

        /* next */
        if (offObject + PAGESIZE >=
            (fAllInOneObject && iObj + 1UL < cObjects ?
             paObjects[iObj + 1].ulRVA - paObjects[iObj].ulRVA : paObjects[iObj].cbVirtual)
            )
        {   /* object++ */
            iObj++;
            offObject = 0UL;
        }
        else /* page++ */
            offObject += PAGESIZE;
    }

    return NO_ERROR;
}


/**
 * Convert baserelocation and imports to LX fixups. Complex stuff!
 * @returns   NO_ERROR on succes. Errorcode on error.
 * @sketch    If valid pointers to fixups exist then return successfully without processing.
 *            Validate pNtHdrs.
 *            IF forwarders present and exports not made THEN makeExports!
 *            Check and read directories for imports and relocations.
 *            Create necessary Buffered RVA Readers.
 *            Make sure kernel32 is the first imported module.
 *            Initiate the import variables (if any imports):
 *                Loop thru the import descriptiors looking for the lowest FirstThunk RVA. (ulRVAOrgFirstThunk and ulRVAFirstThunk)
 *                When found read the module name and add it. (ulModuleOrdinal)
 *            Initiate base relocations by reading the first Base Relocation.
 *            Initiate iObj to 0UL, ulRVAPage to the RVA for the first object (which is allways 0UL!).
 *
 *            LOOP thru all objects as long as all processing is successful
 *            BEGIN
 *                Add a new page table entry.
 *                IF Page with TIBFix THEN add import fixup for RegisterEXE/DLL call.
 *                IF Import fixups on current page THEN
 *                BEGIN
 *                    LOOP while no processing errors and more imports on this page
 *                    BEGIN
 *                        Read Thunk
 *                        IF not end of thunk array THEN
 *                        BEGIN
 *                            IF ordinal import THEN add ordinal import fixup
 *                            ELSE IF valid RVA THEN
 *                                get name and add name import fixup
 *                            ELSE
 *                                complain and fail.
 *                            Next Thunk array element (ulRVAFirstThunk and ulRVAOrgFirstThunk)
 *                        END
 *                        ELSE
 *                        BEGIN
 *                            LOOP thru ImportDescriptors and find the following FirstThunk array. (ulRVAOrgFirstThunk and ulRVAFirstThunk)
 *                            IF found THEN read the module name and add it. (ulModuleOrdinal)
 *                            ELSE Disable Import Processing.
 *                        END
 *                    END
 *                END
 *                IF BaseRelocation chunk for current page THEN
 *                BEGIN
 *                    LOOP thru all relocation in this chunk.
 *                    BEGIN
 *                        Get relocation type/offset.
 *                        Get target.
 *                        IF BASED_HIGHLOW fixup THEN add it ELSE ignore it.
 *                    END
 *                    Read next relocation chunk header - if any.
 *                END
 *                Next page in object or next object. (ulRVAPage and iObj)
 *            END
 *            IF success THEN add final page table entry.
 *            delete reader objects.
 *            IF success THEN release unused memory in fixup and import structures.
 *            return errorcode.
 *
 * @status    completely
 * @author    knut st. osmundsen
 * @remark    stack usage: 26*4 + 5*4 = 124 bytes
 *            heap  usage: 5 blocks   (4096 + 4*4) bytes = 20110 bytes
 *            See bottom of addForwarderEntry remark!
 *
 *            BTW. This piece of code remindes me about cobol programming - large and clumsy.
 *            (I have not programmed cobol, but have read and debugged it.)
 */
ULONG Pe2Lx::makeFixups()
{
    BOOL                        fImports;           /* fImport is set when a valid import directory is present. */
    ULONG                       ulRVAImportDesc;    /* RVA of the first import descriptor. */
    IMAGE_IMPORT_DESCRIPTOR     ImportDesc;         /* Import descriptor struct used while reading. */
    BufferedRVARead            *pImportReader;      /* Buffered reader import descriptors reads. */
    BufferedRVARead            *pImpNameReader;     /* Buffered reader for names reads; ie. function and module names. */
    BufferedRVARead            *pImpThunkReader;    /* Buffered reader for thunk reads; ie. reading from the OrgiginalFirstThunk array. */
    ULONG                       ulModuleOrdinal;    /* Module ordinal. Valid as long as fImport is set. */
    ULONG                       ulRVAFirstThunk;    /* Current first thunk array RVA. Points at current entry. */
    ULONG                       ulRVAOrgFirstThunk; /* Current original first thunk array RVA. Points at current entry. */
    #ifndef RING0
    BOOL                        fBaseRelocs;        /* fBaseReloc is set when a valid base reloc directory is present. */
    ULONG                       ulRVABaseReloc;     /* RVA of the current base relocation chunk. (Not the first!) */
    LONG                        cbBaseRelocs;       /* Count of bytes left of base relocation. Used to determin eof baserelocs. */
    IMAGE_BASE_RELOCATION       BaseReloc;          /* Base Relocation struct which is used while reading. */
    BufferedRVARead            *pPageReader;        /* Buffered reader for page reads; ie. getting the target address. */
    BufferedRVARead            *pRelocReader;       /* Buffered reader for relocation reads; ie getting the type/offset word. */
    #endif
    ULONG                       ulRVAPage;          /* RVA for the current page. */
    ULONG                       ul;                 /* temporary unsigned long variable. Many uses. */
    PSZ                         psz;                /* temporary string pointer. Used for modulenames and functionnames. */
    ULONG                       iObj;               /* Object iterator. (Index into paObjects) */
    APIRET                      rc;                 /* return code. */

    /* Test if valid fixup data pointers - return if vaild */
    if (pFixupRecords != NULL && paulFixupPageTable != NULL)
        return NO_ERROR;

    /* initiate data members of this object */
    rc = initFixups();
    if (rc != NO_ERROR)
        return rc;

    /* Check that the NtHdr is valid. */
    rc = loadNtHeaders();
    if (rc != NO_ERROR)
        return rc;

    /* if there are forwarder entries present, we'll have to make them first.  */
    if (fForwarders && pEntryBundles == NULL)
    {
        rc = makeExports();
        if (rc != NO_ERROR)
            return rc;
    }

    /* Check if empty import directory. */
    fImports = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size > 0UL
        &&
        (ulRVAImportDesc = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) > 0UL
        &&
        ulRVAImportDesc < pNtHdrs->OptionalHeader.SizeOfImage;

    /* Check if empty base relocation directory. */
    #ifndef RING0
    fBaseRelocs = (cbBaseRelocs = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) > 0UL
        &&
        (ulRVABaseReloc = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) > 0UL
        &&
        ulRVABaseReloc < pNtHdrs->OptionalHeader.SizeOfImage;
    #endif
    printInf(("\n"));
    #ifndef RING0
    printInf(("Make fixups, fBaseReloc=%s, fImports=%s\n",
              fBaseRelocs ? "true" : "false",
              fImports ? "true" : "false"));
    #else
    printInf(("Make fixups, fImports=%s\n", fImports ? "true" : "false"));
    #endif
    printInf(("\n"));

    /* create reader buffers */
    #ifndef RING0
    if (fBaseRelocs)
    {
        pPageReader = new BufferedRVARead(hFile, cObjects, paObjects);
        pRelocReader = new BufferedRVARead(hFile, cObjects, paObjects);
        if (pPageReader == NULL || pRelocReader == NULL)
            rc = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
        pRelocReader = pPageReader = NULL;
    #endif
    if (fImports)
    {
        pImportReader = new BufferedRVARead(hFile, cObjects, paObjects);
        pImpNameReader = new BufferedRVARead(hFile, cObjects, paObjects);
        pImpThunkReader = new BufferedRVARead(hFile, cObjects, paObjects);
        if (pImportReader == NULL || pImpNameReader == NULL || pImpThunkReader == NULL)
            rc = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
        pImpThunkReader = pImpNameReader = pImportReader = NULL;

    /* check for errors */
    if (rc != NO_ERROR)
    {   /* error: clean up and return! */
        #ifndef RING0
        if (pPageReader != NULL)
            delete pPageReader;
        if (pRelocReader != NULL)
            delete pRelocReader;
        #endif
        if (pImportReader != NULL)
            delete pImportReader;
        if (pImpNameReader  != NULL)
            delete pImpNameReader;
        if (pImpThunkReader != NULL)
            delete pImpThunkReader;
        return rc;
    }

    /* Make sure kernel32 is the first imported module */
    if (rc == NO_ERROR)
        rc = addModule("KERNEL32.DLL", (PULONG)SSToDS(&ul));

    /* initiate the import variables */
    if (fImports && rc == NO_ERROR)
    {
        rc = pImportReader->readAtRVA(ulRVAImportDesc, SSToDS(&ImportDesc), sizeof(ImportDesc));
        if (rc == NO_ERROR)
        {
            ul = 0;
            ulRVAFirstThunk = ~0UL;
            while (rc == NO_ERROR && ImportDesc.Name != 0UL && ImportDesc.FirstThunk != 0UL)
            {
                if ((ULONG)ImportDesc.FirstThunk < ulRVAFirstThunk || ulRVAFirstThunk == 0UL) /* 0 test: just in case... */
                {
                    ulRVAFirstThunk = (ULONG)ImportDesc.FirstThunk;
                    ulRVAOrgFirstThunk = (ULONG)ImportDesc.u.OriginalFirstThunk != 0UL ?
                        (ULONG)ImportDesc.u.OriginalFirstThunk : (ULONG)ImportDesc.FirstThunk;
                    ulModuleOrdinal = ImportDesc.Name;
                }

                /* next */
                ul++;
                rc = pImportReader->readAtRVA(ulRVAImportDesc + ul * sizeof(ImportDesc), SSToDS(&ImportDesc), sizeof(ImportDesc));
            }

            if (ulRVAFirstThunk != ~0UL)
            {
                rc = pImpNameReader->dupString(ulModuleOrdinal, (PSZ*)SSToDS(&psz));
                if (rc == NO_ERROR)
                    rc = addModule(psz, (PULONG)SSToDS(&ulModuleOrdinal));
                free(psz);
            }
            else
                fImports = FALSE;
        }
    }


    /* read start of the first basereloc chunk */
    #ifndef RING0
    if (fBaseRelocs && rc == NO_ERROR)
        rc = pRelocReader->readAtRVA(ulRVABaseReloc, SSToDS(&BaseReloc), sizeof(BaseReloc));
    #endif


    /*
     *  The Loop! Iterate thru all pages for all objects.
     */
    iObj = 0UL;
    if (iObj < cObjects)
        ulRVAPage = paObjects[iObj].ulRVA;
    while (iObj < cObjects && rc == NO_ERROR)
    {
        printInfA(("Page at RVA=0x%08x, object no.%d\n", ulRVAPage, iObj));

        /* insert new fixup page and fixup record structs */
        rc = addPageFixupEntry();
        if (rc != NO_ERROR)
            break;

        /* check for TibFix, add import fixup for it */
        if (paObjects[iObj].Misc.fTIBFixObject
            && ((paObjects[iObj].Misc.offTIBFix + paObjects[iObj].ulRVA) & ~(PAGESIZE-1UL)) == ulRVAPage)
        {
            rc = addModule("KERNEL32.DLL", (PULONG)SSToDS(&ul));
            if (rc == NO_ERROR)
            {
                printInfA(("TibFix import fixup\n"));
                rc = add32OrdImportFixup((WORD)((paObjects[iObj].Misc.offTIBFix + paObjects[iObj].ulRVA + TIBFIX_OFF_CALLADDRESS) & (PAGESIZE-1UL)),
                                         ul,
                                         pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL ?
                                            ORD_REGISTERPE2LXDLL : ORD_REGISTERPE2LXEXE);
            }
            if (rc != NO_ERROR)
                break;
        }


        /* check for imports at this page */
        if (fImports && ulRVAFirstThunk < ulRVAPage + PAGESIZE)
        {
            while (rc == NO_ERROR && ulRVAFirstThunk < ulRVAPage + PAGESIZE)
            {
                IMAGE_THUNK_DATA Thunk;
                rc = pImpThunkReader->readAtRVA(ulRVAOrgFirstThunk, SSToDS(&Thunk), sizeof(Thunk));
                if (rc != NO_ERROR)
                    break;
                if (Thunk.u1.Ordinal != 0UL)
                {
                    if (Thunk.u1.Ordinal & (ULONG)IMAGE_ORDINAL_FLAG)
                        rc = add32OrdImportFixup((WORD)(ulRVAFirstThunk & (PAGESIZE-1)),
                                                 ulModuleOrdinal, Thunk.u1.Ordinal & 0xffff);
                    else if (Thunk.u1.Ordinal > 0UL && Thunk.u1.Ordinal < pNtHdrs->OptionalHeader.SizeOfImage)
                    {
                        rc = pImpNameReader->dupString(Thunk.u1.Ordinal + offsetof(IMAGE_IMPORT_BY_NAME, Name),
                                                       (PSZ*)SSToDS(&psz));
                        if (rc != NO_ERROR)
                            break;
                        rc = add32NameImportFixup((WORD)(ulRVAFirstThunk & (PAGESIZE-1)),
                                                  ulModuleOrdinal, psz);
                        free(psz);
                    }
                    else
                    {
                        printErr(("invalid value in thunk array, neither an ordinal value nor an valid RVA. 0x%08x\n", Thunk.u1.Ordinal));
                        rc = ERROR_INVALID_ADDRESS;
                        break;
                    }

                    /* next */
                    ulRVAFirstThunk += sizeof(IMAGE_THUNK_DATA);
                    ulRVAOrgFirstThunk += sizeof(IMAGE_THUNK_DATA);
                }
                else
                {   /* next module */
                    rc = pImportReader->readAtRVA(ulRVAImportDesc, SSToDS(&ImportDesc), sizeof(ImportDesc));
                    if (rc == NO_ERROR)
                    {
                        ULONG ulRVAFirstThunkPrev = ulRVAFirstThunk;
                        ul = 0;
                        ulRVAFirstThunk = ~0UL;
                        while (rc == NO_ERROR && ImportDesc.Name != 0UL && ImportDesc.FirstThunk != 0UL)
                        {
                            if ((ULONG)ImportDesc.FirstThunk < ulRVAFirstThunk
                                && (ULONG)ImportDesc.FirstThunk > ulRVAFirstThunkPrev)
                            {
                                ulRVAFirstThunk = (ULONG)ImportDesc.FirstThunk;
                                ulRVAOrgFirstThunk = (ULONG)ImportDesc.u.OriginalFirstThunk != 0UL ?
                                    (ULONG)ImportDesc.u.OriginalFirstThunk : (ULONG)ImportDesc.FirstThunk;
                                ulModuleOrdinal = ImportDesc.Name;
                            }

                            /* next */
                            ul++;
                            rc = pImportReader->readAtRVA(ulRVAImportDesc + ul * sizeof(ImportDesc), SSToDS(&ImportDesc), sizeof(ImportDesc));
                        }

                        if (ulRVAFirstThunk != ~0UL)
                        {   /* modulename */
                            rc = pImpNameReader->dupString(ulModuleOrdinal, (PSZ*)SSToDS(&psz));
                            if (rc == NO_ERROR)
                            {
                                rc = addModule(psz, (PULONG)SSToDS(&ulModuleOrdinal));
                                free(psz);
                            }
                        }
                        else
                            fImports = FALSE;
                    }
                }
            } /* while */
            if (rc != NO_ERROR)
                break;
        }


        /* check for fixups for this page. ASSUMES that fixups are sorted and that each chunk covers one page, no more no less. */
        #ifndef RING0
        if (fBaseRelocs && BaseReloc.VirtualAddress == ulRVAPage)
        {
            ULONG c = (BaseReloc.SizeOfBlock - sizeof(BaseReloc.SizeOfBlock) - sizeof(BaseReloc.VirtualAddress)) / sizeof(WORD); /* note that sizeof(BaseReloc) is 12 bytes! */
            PWORD pawoffFixup = NULL;

            if (c != 0)
            {
                pawoffFixup = (PWORD)malloc((size_t)(c * sizeof(WORD)));
                if (pawoffFixup != NULL)
                    rc = pRelocReader->readAtRVA(ulRVABaseReloc + offsetof(IMAGE_BASE_RELOCATION, TypeOffset),
                                                 pawoffFixup, c * sizeof(WORD));
                else
                    rc = ERROR_NOT_ENOUGH_MEMORY;

                /* loop thru the baserelocation in this chunk. */
                for (ul = 0; ul < c && rc == NO_ERROR; ul++)
                {
                    WORD  woffFixup;
                    ULONG ulTarget;
                    /* Get relocation type/offset. */
                    if (pawoffFixup != NULL)
                        woffFixup = pawoffFixup[ul];

                    /* Get target. */
                    rc = pPageReader->readAtRVA(BaseReloc.VirtualAddress + (woffFixup & 0x0FFF),
                                                SSToDS(&ulTarget), sizeof(ulTarget));
                    if (rc == NO_ERROR)
                    {
                        switch (woffFixup >> 12)
                        {
                            case IMAGE_REL_BASED_HIGHLOW:
                                rc = add32OffsetFixup((WORD)(woffFixup & 0x0FFF), ulTarget);
                                printInfA(("Fixup: 0x%03x target 0x%08x (rc = %d) %s\n",
                                           (woffFixup & 0x0FFF), ulTarget, rc,
                                           pvCrossPageFixup ? "crosspage" : ""));
                                break;
                            case IMAGE_REL_BASED_ABSOLUTE: /* ignored! */
                                break;
                            default:
                                printWar(("Unknown/unsupported fixup type!, 0x%1x\n", woffFixup >> 12));
                        }
                    }
                }
            }

            /* cleanup */
            if (pawoffFixup != NULL)
                free(pawoffFixup);

            /* break on error */
            if (rc != NO_ERROR)
                break;

            /* read next descriptor if any */
            ulRVABaseReloc += BaseReloc.SizeOfBlock;
            cbBaseRelocs -= BaseReloc.SizeOfBlock;
            if (cbBaseRelocs > 0)
            {
                rc = pRelocReader->readAtRVA(ulRVABaseReloc, SSToDS(&BaseReloc), sizeof(BaseReloc));
                if (rc != NO_ERROR)
                    break;
            }
            else
                fBaseRelocs = FALSE;
        }
        #endif /*ifndef RING */

        /**  next  **/
        if (ulRVAPage + PAGESIZE >=
            (fAllInOneObject && iObj + 1 < cObjects ?
             paObjects[iObj + 1].ulRVA : paObjects[iObj].ulRVA + paObjects[iObj].cbVirtual)
            )
        {   /* object++ */
            iObj++;
            if (iObj < cObjects)
                ulRVAPage = paObjects[iObj].ulRVA;
        }
        else /* page++ */
            ulRVAPage += PAGESIZE;
        Yield();
    } /* The Loop! */


    /* insert final fixup page struct */
    if (rc == NO_ERROR)
        rc = addPageFixupEntry(TRUE);

    /* finished! - cleanup! */
    #ifndef RING0
    if (pPageReader != NULL)
        delete pPageReader;
    if (pRelocReader != NULL)
        delete pRelocReader;
    #endif
    if (pImportReader != NULL)
        delete pImportReader;
    if (pImpNameReader  != NULL)
        delete pImpNameReader;
    if (pImpThunkReader != NULL)
        delete pImpThunkReader;

    /* Release unused memory in fixup and import structures. */
    if (rc == NO_ERROR)
    {
        finalizeImportNames();
        finalizeFixups();
    }
    #ifdef RING0
    /* load base relocations... */
    if (rc == NO_ERROR)
        return loadBaseRelocations();
    #endif
    return rc;
}


/**
 * Convert exports to LX entries and resident name table.
 * It also creates the modulename.
 * @returns   Return code. (NO_ERROR)
 * @sketch    IF valid pointers THEN - nothing to do - return successfully.
 *            Check that the NtHdr is valid.
 *            Init datastructues.
 *            Add modulename to resident name table with ordinal 0.
 *            IF empty export directory THEN finalizeExports and return successfully.
 *            Create buffered readers.
 *            Read Export Directory.
 *            Convert Address of Functions to LX entry points:
 *            LOOP thru all entries in the AddressOfFunctions array
 *            BEGIN
 *                Read entry. (ulRVA)
 *                IF forwarder THEN add forwarder.
 *                IF not forwarder THEN add entry.
 *            END
 *
 *            Convert function names to resident names (if any names of course).
 *            LOOP thru all entries in the two parallell arrays AddressOfNames and AddressOfNameOrdinals
 *            BEGIN
 *                Read entries from both tables.
 *                Read Name.
 *                Add resident name.
 *            END
 *
 *            Add last entry.
 *            Delete readers.
 *            Finalize Exports.
 *            return return code.
 *
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG Pe2Lx::makeExports()
{
    IMAGE_EXPORT_DIRECTORY  ExpDir;         /* Export directory struct used when reading the export directory. */
    BufferedRVARead        *pFATFOTReader;  /* Buffered reader for function address table and function ordinal table reads. */
    BufferedRVARead        *pFNTReader;     /* Buffered reader for function name table reads. */
    BufferedRVARead        *pNameReader;    /* Buffered reader for function name and forwarder 'dll.function' reads. */
    ULONG                   ulRVAExportDir;
    APIRET                  rc;
    PSZ                     psz;


    /* check if valid pointers - nothing to do - return successfully.*/
    if (pEntryBundles != NULL && pachResNameTable != NULL)
        return NO_ERROR;

    /* Check that the NtHdr is valid. */
    rc = loadNtHeaders();
    if (rc != NO_ERROR)
        return rc;

    /* Init datastructues. */
    rc = initEntry();
    if (rc != NO_ERROR)
        return rc;

    /* Add modulename to resident name table with ordinal 0. */
    rc = addResName(0UL, pszModuleName, ~0UL);
    if (rc != NO_ERROR)
        return rc;

    /* Check if empty export directory. */
    if (!(pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size > 0UL
        &&
        (ulRVAExportDir = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress) > 0UL
        &&
        ulRVAExportDir < pNtHdrs->OptionalHeader.SizeOfImage))
    {   /* no exports */
        finalizeExports();
        return rc;
    }

    printInf(("\n"));
    printInf(("Make exports\n"));
    printInf(("\n"));

    /* Create buffered readers. */
    pFATFOTReader = new BufferedRVARead(hFile, cObjects, paObjects);
    pFNTReader    = new BufferedRVARead(hFile, cObjects, paObjects);
    pNameReader   = new BufferedRVARead(hFile, cObjects, paObjects);
    if (pFATFOTReader != NULL && pFNTReader != NULL && pNameReader != NULL)
    {
        /* Read export directory. */
        rc = pFATFOTReader->readAtRVA(ulRVAExportDir, SSToDS(&ExpDir), sizeof(ExpDir));
        if (rc == NO_ERROR)
        {
            ULONG ulRVA;
            ULONG ul;

            /* (try) optimize diskreads. */
            if ((ULONG)ExpDir.AddressOfFunctions < ulRVAExportDir + BUFFEREDRVAREADER_BUFFERSIZE)
                *pFNTReader = *pFATFOTReader;
            *pNameReader = *pFATFOTReader;

            /* Convert Address of Functions to LX entry points. */
            for (ul = 0; ul < ExpDir.NumberOfFunctions && rc == NO_ERROR; ul++)
            {
                rc = pFATFOTReader->readAtRVA((ULONG)ExpDir.AddressOfFunctions + sizeof(ULONG)*ul,
                                              SSToDS(&ulRVA), sizeof(ulRVA));
                /* empty? */
                if (ulRVA != 0UL)
                {
                    BOOL fForwarder = FALSE;

                    /* forwarder? ulRVA within export directory. */
                    if (ulRVA > pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
                        && ulRVA < pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
                                   + pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size
                        )
                    {   /* forwarder!(?) */
                        PSZ  pszDll;
                        PSZ  pszFn;
                        rc = pNameReader->dupString(ulRVA, (PSZ*)SSToDS(&pszDll));
                        if (rc != NO_ERROR)
                            break;
                        pszFn = strchr(pszDll, '.');
                        if (pszFn != NULL && strlen(pszDll) != 0 && strlen(pszFn) != 0)
                        {
                            *pszFn++ = '\0';
                            rc = addForwarderEntry(ul + ExpDir.Base, pszDll, pszFn);
                            fForwarder = TRUE;
                        }
                        free(pszDll);
                    }

                    /* non-forwarder export? */
                    if (!fForwarder)
                        rc = addEntry(ul + ExpDir.Base, ulRVA);
                }
            } /* for loop - export --> entry */
            if (rc != NO_ERROR)
                printErr(("export --> entry loop failed! ul = %d rc = %d\n", ul, rc));
            Yield();

            /* Convert function names to resident names. */
            if (rc == NO_ERROR && ExpDir.NumberOfNames > 0UL)
            {
                if ((ULONG)ExpDir.AddressOfNameOrdinals != 0UL && (ULONG)ExpDir.AddressOfNames != 0UL
                    && (ULONG)ExpDir.AddressOfNameOrdinals < pNtHdrs->OptionalHeader.SizeOfImage
                    && (ULONG)ExpDir.AddressOfNames < pNtHdrs->OptionalHeader.SizeOfImage
                    )
                {
                    WORD usOrdinal;

                    for (ul = 0; ul < ExpDir.NumberOfNames && rc == NO_ERROR; ul++)
                    {
                        rc = pFNTReader->readAtRVA((ULONG)ExpDir.AddressOfNames + ul * sizeof(ULONG),
                                                   SSToDS(&ulRVA), sizeof(ulRVA));
                        if (rc != NO_ERROR)
                            break;
                        rc = pFATFOTReader->readAtRVA((ULONG)ExpDir.AddressOfNameOrdinals + ul * sizeof(WORD),
                                                      SSToDS(&usOrdinal), sizeof(usOrdinal));
                        if (rc != NO_ERROR)
                            break;
                        usOrdinal += ExpDir.Base;
                        rc = pNameReader->dupString(ulRVA, (PSZ*)SSToDS(&psz));
                        if (rc != NO_ERROR)
                            break;
                        rc = addResName(usOrdinal, psz, ~0UL);
                        free(psz);
                        Yield();
                    }
                    if (rc != NO_ERROR)
                        printErr(("FnNames --> ResNames loop failed! ul = %d rc = %d\n", ul, rc));
                }
                else
                {
                    printErr(("NumberOfNames = %d but AddressOfNames = 0x%08x and AddressOfNameOrdinals = 0x%08x, invalid RVA(s)!\n",
                             ExpDir.AddressOfNames, ExpDir.AddressOfNameOrdinals));
                    rc = ERROR_BAD_EXE_FORMAT;
                } /* endifelse: 'Names' integrity check */

                /* Add last entry. */
                if (rc == NO_ERROR)
                    rc = addLastEntry();

            } /* endif: name */
        }
    }
    else
        rc = ERROR_NOT_ENOUGH_MEMORY;

    /* delete readers */
    if (pFATFOTReader != NULL)
        delete pFATFOTReader;
    if (pFNTReader != NULL)
        delete pFNTReader;
    if (pNameReader != NULL)
        delete pNameReader;

    /* Release unused memory in export structures */
    if (rc == NO_ERROR)
        finalizeExports();

    return rc;
}


/**
 * Load the NT headers from disk if they're not present.
 * Call this function to make sure that pNtHdrs is valid.
 * @returns   Error code. (NO_ERROR == success)
 * @status    Competely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Minor error: Don't read more datadirectory entries than defined in the header files.
 *                         This is not a problem since we'll only use some of the first ones.
 */
ULONG Pe2Lx::loadNtHeaders()
{
    APIRET rc;
    if (pNtHdrs != NULL)
        return NO_ERROR;
    pNtHdrs = (PIMAGE_NT_HEADERS)malloc(sizeof(IMAGE_NT_HEADERS));
    if (pNtHdrs == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    rc = ReadAt(hFile, offNtHeaders, pNtHdrs, sizeof(IMAGE_NT_HEADERS));
    if (rc != NO_ERROR)
    {
        free(pNtHdrs);
        pNtHdrs = NULL;
    }
    return rc;
}


/**
 * Releases memory used by the pNtHdrs pointer.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
VOID Pe2Lx::releaseNtHeaders()
{
    if (pNtHdrs != NULL)
    {
        free(pNtHdrs);
        pNtHdrs = NULL;
    }
}


/**
 * Loads the baserelocations from the PE-file.
 * @returns   NO_ERROR on success, appropriate error message.
 * @sketch
 * @status    completely implemented; untestd.
 * @author    knut st. osmundsen
 */
ULONG Pe2Lx::loadBaseRelocations()
{
    APIRET rc;
    ULONG ulRVA;

    /* ? */
    rc = loadNtHeaders();
    if (rc != NO_ERROR)
        return rc;
    /* end ? */

    ulRVA  = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    cbBaseRelocs = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

    if (ulRVA > 0UL && cbBaseRelocs > 0UL && ulRVA < ~0UL && cbBaseRelocs < ~0UL)
    {
        pBaseRelocs = (PIMAGE_BASE_RELOCATION)smalloc((size_t)cbBaseRelocs);
        if (pBaseRelocs != NULL)
        {
            rc = readAtRVA(ulRVA, pBaseRelocs, cbBaseRelocs);
            if (rc != NO_ERROR)
            {
                printErr(("readAtRVA(0x%08x, 0x%08x, 0x%08x) failed with rc = %d\n",
                          ulRVA, pBaseRelocs, cbBaseRelocs, rc));
                sfree(pBaseRelocs);
                pBaseRelocs = NULL;
            }
            #ifdef DEBUG
            else
            {
                PIMAGE_BASE_RELOCATION pbrCur = pBaseRelocs;
                while ((void*)pbrCur < (void*)((unsigned)pBaseRelocs + cbBaseRelocs)
                       && pbrCur->SizeOfBlock >= 8)
                {
                    if ((unsigned)pbrCur->SizeOfBlock + (unsigned)pbrCur > (unsigned)pBaseRelocs + cbBaseRelocs)
                        printErr(("Debug-check - Chunk is larger than the base relocation directory. "
                                  "SizeOfBlock=0x%08x, VirtualAddress=0x%08x\n",
                                  pbrCur->SizeOfBlock, pbrCur->VirtualAddress));
                    pbrCur = (PIMAGE_BASE_RELOCATION)((unsigned)pbrCur + pbrCur->SizeOfBlock);
                }
            }
            #endif
            return rc;
        }
        else
        {
            printErr(("rmalloc failed when allocating memory for pBaseReloc, cbSize=0x%x(%d)\n",
                      cbBaseRelocs, cbBaseRelocs));
            rc = ERROR_NOT_ENOUGH_MEMORY;
        }
    }
    else
    {
        pBaseRelocs = NULL;
        cbBaseRelocs = 0;
    }

    return NO_ERROR;
}


/**
 * Initiates the fixup data members of this object.
 * Called from makeFixup and initEntry().
 * Checks for forwarder stuff. If forwarders present, the import module and procdure name tables are not
 * freed - initEntry have done that.
 * @returns   NO_ERROR.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG  Pe2Lx::initFixups()
{
    if (paulFixupPageTable != NULL)
    {
        free(paulFixupPageTable);
        paulFixupPageTable = NULL;
    }
    cFixupPTEntries = cFPTEAllocated = 0UL;
    if (pFixupRecords != NULL)
    {
        free(pFixupRecords);
        pFixupRecords = NULL;
    }
    offCurFixupRec = cbFRAllocated = 0UL;
    if (pvCrossPageFixup != NULL)
    {
        free(pvCrossPageFixup);
        pvCrossPageFixup = NULL;
    }
    cbCrossPageFixup = 0UL;

    /* if there aren't forwarders we may safely free the Import Module/Procedure name tables */
    if (!fForwarders)
    {
        if (pachImpModuleNames != NULL)
        {
            free(pachImpModuleNames);
            pachImpModuleNames = NULL;
        }
        offCurImpModuleName = cchIMNAllocated = 0UL;
        if (pachImpFunctionNames != NULL)
        {
            free(pachImpFunctionNames);
            pachImpFunctionNames = NULL;
        }
        offCurImpFunctionName = cchIFNAllocated = 0UL;
    }
    return NO_ERROR;
}


/**
 * Adds a page fixup entry to the fixup page table array (paFixupPageTable).
 * @returns   NO_ERROR on success. Errorcode on error.
 * @param     fLast  TRUE:  last entry
 *                   FALSE: not last (default)
 * @sketch    IF not enough memory THEN
 *            BEGIN
 *                IF no memory allocated THEN count pages and allocate memory for all pages (+1).
 *                ELSE increase amount of memory according to fLast.
 *            END
 *
 *            Set offset of current entry to current Fixup Record Array offset.
 *            IF last entry (fLast) THEN
 *                IF Cross Page Fixup THEN error!
 *            ELSE
 *                IF Cross Page Fixup THEN check if enough memory and add it.
 *            return successfully.
 *
 * @status    Completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Fixup Page Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *   The Fixup Page Table provides a simple mapping of a logical page number to an offset
 *   into the Fixup Record Table for that page.  This table is parallel to the Object Page
 *   Table, except that there is one additional entry in this table to indicate the end of
 *   the Fixup Record Table.
 *   The format of each entry is:
 *
 *                     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *    Logical Page #1  ³  OFFSET FOR PAGE #1   ³
 *                     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 *    Logical Page #2  ³  OFFSET FOR PAGE #2   ³
 *                     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 *                               . . .
 *                     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *    Logical Page #n  ³  OFFSET FOR PAGE #n   ³
 *                     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 *                     ³OFF TO END OF FIXUP REC³   This is equal to:
 *                     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ   Offset for page #n + Size
 *                                                 of fixups for page #n
 *   Fixup Page Table
 *
 *     OFFSET FOR PAGE # = DD  Offset for fixup record for this page. This field specifies
 *     the offset, from the beginning of the fixup record table, to the first fixup record
 *     for this page.
 *
 *     OFF TO END OF FIXUP REC = DD  Offset to the end of the fixup records. This field
 *     specifies the offset following the last fixup record in the fixup record table. This
 *     is the last entry in the fixup page table.  The fixup records are kept in order by
 *     logical page in the fixup record table.  This allows the end of each page's fixup
 *     records is defined by the offset for the next logical page's fixup records. This last
 *     entry provides support of this mechanism for the last page in the fixup page table.
 *
 */
ULONG  Pe2Lx::addPageFixupEntry(BOOL fLast/* = FALSE*/)
{
    /* enough memory? */
    if (cFixupPTEntries >= cFPTEAllocated)
    {
        if (cFPTEAllocated == 0UL)
        {   /* first call */
            ULONG cPages = getCountOfPages();
            if (cPages == 0UL && !fLast)
                return ERROR_INTERNAL_PROCESSING_ERROR;

            paulFixupPageTable = (PULONG)malloc((size_t)(cPages + 1UL) * sizeof(ULONG));
            if (paulFixupPageTable != NULL)
                cFPTEAllocated = cPages + 1UL;
            else
                return ERROR_NOT_ENOUGH_MEMORY;
        }
        else
        {   /* hmm - algorithm for determin array size incorrect? */
            printErr(("More fixup pages than calculated!\n"));
            return ERROR_BAD_EXE_FORMAT;
        }
    }

    /* add new entry */
    paulFixupPageTable[cFixupPTEntries++] = offCurFixupRec;
    if (fLast)
    {   /* final entry */
        /* Error check */
        if (pvCrossPageFixup != NULL)
        {
            printErr(("Cross page fixup when fLast is set!\n"));
            return ERROR_INTERNAL_PROCESSING_ERROR;
        }
    }
    else
    {   /* just another entry */
        /* cross page fixup? */
        if (pvCrossPageFixup != NULL)
        {
            AllocateMoreMemory(offCurFixupRec + cbCrossPageFixup > cbFRAllocated,
                               pFixupRecords, PVOID, cbFRAllocated, PAGESIZE, PAGESIZE)

            memcpy(&((PCHAR)pFixupRecords)[offCurFixupRec],
                   pvCrossPageFixup, cbCrossPageFixup);
            offCurFixupRec += cbCrossPageFixup;
            free(pvCrossPageFixup);
            pvCrossPageFixup = NULL;
        }
    }
    return NO_ERROR;
}


/**
 * Add 32-bit offset fixup.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_BAD_EXE_FORMAT
 * @param     offSource  Source offset into the current page.
 * @param     ulTarget   Target Pointer.
 * @sketch    IF not enough memory THEN allocate some more memory.
 *            Find target object and offset into target object for the given target address (ulTarget).
 *            Fill in fixup record.
 *            Increment the size of the fixup records array (offCurFixupRec).
 *            IF cross page fixup THEN
 *            BEGIN
 *                Allocate memory for the cross page fixup record.
 *                Copy the fixup record we just created into the memory of the cross page fixup record..
 *                Substract the size of a Page from the source offset (r32_soff) in the cross page fixup record.
 *            END
 *            End successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Internal Fixup Record format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *           ÚÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *       00h ³ SRC ³FLAGS³SRCOFF/CNT*³
 *           ÃÄÄÄÄÄÁÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄ¿
 *   03h/04h ³  OBJECT * ³        TRGOFF * @     ³
 *           ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ´
 *           ³ SRCOFF1 @ ³   . . .   ³ SRCOFFn @ ³
 *           ÀÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *           * These fields are variable size.
 *           @ These fields are optional.
 *
 *   Internal Fixup Record
 *
 *       OBJECT = D[B|W]  Target object number. This field is an index into the current
 *       module's Object Table to specify the target Object. It is a Byte value when the
 *       '16-bit Object Number/Module Ordinal Flag' bit in the target flags field is
 *       clear and a Word value when the bit is set.
 *
 *       TRGOFF = D[W|D]  Target offset. This field is an offset into the specified target
 *       Object. It is not present when the Source Type specifies a 16-bit Selector fixup.
 *       It is a Word value when the '32-bit Target Offset Flag' bit in the target flags
 *       field is clear and a Dword value when the bit is set.
 * ---------------------
 * This code got a bit dirty while trying to optimize memory usage.
 */
ULONG Pe2Lx::add32OffsetFixup(WORD offSource, ULONG ulTarget)
{
    struct r32_rlc *prlc;
    ULONG           iObj;            /* target object. */
    ULONG           cbFixup;         /* size of the fixup record. */

    /* enough memory? */
    AllocateMoreMemory(offCurFixupRec + RINTSIZE32 > cbFRAllocated,
                       pFixupRecords, PVOID, cbFRAllocated, PAGESIZE, PAGESIZE)

    /* target object and offset */
    if (ulTarget >= ulImageBase)
    {
        ulTarget -= ulImageBase; /* ulTarget is now an RVA */
        iObj = 0UL;
        while (iObj < cObjects
               && ulTarget >= (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                              : ALIGN(paObjects[iObj].cbVirtual, PAGESIZE) + paObjects[iObj].ulRVA)
               )
            iObj++;
        if (iObj < cObjects)
        {
            if (!fAllInOneObject)
            {
                ulTarget -= paObjects[iObj].ulRVA;
                iObj++; /* one based object number. */
            }
            else
                iObj = 1UL;
        }
        else
        {
            printErr(("Invalid target RVA, 0x%08x.\n", ulTarget));
            return ERROR_BAD_EXE_FORMAT;
        }
    }
    else
    {
        printErr(("Invalid target address, 0x%08x.\n", ulTarget));
        return ERROR_BAD_EXE_FORMAT;
    }
    /* ulTarget is now an offset into the target object. */

    /* fill in fixup record */
    cbFixup = 7UL;
    prlc = (struct r32_rlc *)((ULONG)pFixupRecords + offCurFixupRec);
    prlc->nr_stype = NROFF32;
    prlc->nr_flags = NRRINT;
    if (iObj > 255UL)
    {
        prlc->nr_flags |= NR16OBJMOD;
        cbFixup++;
    }
    if (ulTarget > 65535UL)
    {
        prlc->nr_flags |= NR32BITOFF;
        cbFixup += 2;
    }

    prlc->r32_soff = (USHORT)offSource;
    prlc->r32_objmod = (USHORT)iObj;
    if (prlc->nr_flags & NR16OBJMOD)
        prlc->r32_target.intref.offset32 = ulTarget;
    else
        ((struct r32_rlc*)((ULONG)prlc - 1))->r32_target.intref.offset32 = ulTarget;

    /* commit fixup */
    offCurFixupRec += cbFixup;

    /* cross page fixup? */
    if (offSource > PAGESIZE - 4UL)
    {   /* cross page fixup! */
        if (pvCrossPageFixup != NULL)
        {
            printWar(("A cross page fixup allready exists!\n"));
            free(pvCrossPageFixup);
        }
        pvCrossPageFixup = malloc((size_t)cbFixup);
        if (pvCrossPageFixup != NULL)
        {
            memcpy(pvCrossPageFixup, prlc, (size_t)cbFixup);
            cbCrossPageFixup = cbFixup;
            ((struct r32_rlc *)pvCrossPageFixup)->r32_soff -= (USHORT)PAGESIZE;
        }
        else
            return ERROR_NOT_ENOUGH_MEMORY;
    }

    return NO_ERROR;
}


/**
 * Add 32-bit ordinal import fixup.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 * @param     offSource          Offset of the fixup reltaive to the start of the page.
 * @param     ulModuleOrdinal    Module ordinal. Ordinal into the import module name table. (1 based!)
 * @param     ulFunctionOrdinal  Function ordinal. Number of the export which is to be imported from
 *                               the module given by ulModuleOrdinal.
 * @sketch    IF not enough memory for the fixup THEN (try) allocate more memory
 *            Fill in fixup record.
 *            Increment the size of the fixup records array (offCurFixupRec).
 *            IF cross page fixup THEN
 *            BEGIN
 *                Allocate memory for the cross page fixup record.
 *                Copy the fixup record we just created into the memory of the cross page fixup record..
 *                Substract the size of a Page from the source offset (r32_soff) in the cross page fixup record.
 *            END
 *            End successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Internal Fixup Record format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *           ÚÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *       00h ³ SRC ³FLAGS³SRCOFF/CNT*³
 *           ÃÄÄÄÄÄÁÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *   03h/04h ³ MOD ORD# *³IMPORT ORD*³     ADDITIVE * @      ³
 *           ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÙ
 *           ³ SRCOFF1 @ ³   . . .   ³ SRCOFFn @ ³
 *           ÀÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *          * These fields are variable size.
 *          @ These fields are optional.
 *
 *   Import by Ordinal Fixup Record
 *
 *       MOD ORD # = D[B|W]  Ordinal index into the Import Module Name Table. This value
 *       is an ordered index in to the Import Module Name Table for the module containing
 *       the procedure entry point. It is a Byte value when the '16-bit Object Number/Module
 *       Ordinal' Flag bit in the target flags field is clear and a Word value when the bit
 *       is set. The loader creates a table of pointers with each pointer in the table
 *       corresponds to the modules named in the Import Module Name Table. This value is used
 *       by the loader to index into this table created by the loader to locate the referenced module.
 *
 *       IMPORT ORD = D[B|W|D]  Imported ordinal number. This is the imported procedure's
 *       ordinal number. It is a Byte value when the '8-bit Ordinal' bit in the target flags
 *       field is set. Otherwise it is a Word value when the '32-bit Target Offset Flag' bit
 *       in the target flags field is clear and a Dword value when the bit is set.
 *
 *       ADDITIVE = D[W|D]  Additive fixup value. This field exists in the fixup record only
 *       when the 'Additive Fixup Flag' bit in the target flags field is set.  When the
 *       'Additive Fixup Flag' is clear the fixup record does not contain this field and
 *       is immediately followed by the next fixup record (or by the source offset list
 *       for this fixup record).  This value is added to the address derived from the target
 *       entry point. This field is a Word value when the '32-bit Additive Flag' bit in the
 *       target flags field is clear and a Dword value when the bit is set.
 *
 * ---------------------
 * This code got a bit dirty while trying to optimize memory usage.
 *
 */
ULONG  Pe2Lx::add32OrdImportFixup(WORD offSource, ULONG ulModuleOrdinal, ULONG ulFunctionOrdinal)
{
    struct r32_rlc *prlc;
    ULONG           cbFixup;         /* size of the fixup record. */

    /* enough memory? */
    AllocateMoreMemory(offCurFixupRec + 10 > cbFRAllocated /* 10 is max size */,
                       pFixupRecords, PVOID, cbFRAllocated, PAGESIZE, PAGESIZE)

    /* fill in fixup record */
    cbFixup = 7UL;
    prlc = (struct r32_rlc *)((ULONG)pFixupRecords + offCurFixupRec);
    prlc->nr_stype = NROFF32;
    prlc->nr_flags = NRRORD;
    if (ulModuleOrdinal > 255UL)
    {
        prlc->nr_flags |= NR16OBJMOD;
        cbFixup++;
    }
    #if 0 /* lxdump.exe (from Hacker's View release 5.66) don't support this. */
    if (ulFunctionOrdinal < 256UL)
    {
        prlc->nr_flags |= NR8BITORD;
        cbFixup--;
    }
    #endif
    else if (ulFunctionOrdinal > 65535UL)
    {
        prlc->nr_flags |= NR32BITOFF;
        cbFixup += 2;
    }
    prlc->r32_soff = (USHORT)offSource;
    prlc->r32_objmod = (USHORT)ulModuleOrdinal;
    if (prlc->nr_flags & NR16OBJMOD)
        prlc->r32_target.intref.offset32 = ulFunctionOrdinal;
    else
        ((struct r32_rlc*)((ULONG)prlc - 1))->r32_target.intref.offset32 = ulFunctionOrdinal;

    /* commit fixup */
    offCurFixupRec += cbFixup;

    /* cross page fixup? */
    if (offSource > PAGESIZE - 4UL)
    {   /* cross page fixup! */
        if (pvCrossPageFixup != NULL)
        {
            printWar(("A cross page fixup allready exists!\n"));
            free(pvCrossPageFixup);
        }
        pvCrossPageFixup = malloc((size_t)cbFixup);
        if (pvCrossPageFixup != NULL)
        {
            memcpy(pvCrossPageFixup, prlc, (size_t)cbFixup);
            cbCrossPageFixup = cbFixup;
            ((struct r32_rlc *)pvCrossPageFixup)->r32_soff -= (USHORT)PAGESIZE;
        }
        else
            return ERROR_NOT_ENOUGH_MEMORY;
    }

    printInfA(("offset=0x%03x modordinal=%d fnord=%4d cbFixup=%d %s\n",
               offSource, ulModuleOrdinal, ulFunctionOrdinal, cbFixup, pvCrossPageFixup ? "crosspage" : ""));

    return NO_ERROR;
}


/**
 *
 * @returns   NO_ERROR on success. Errorcode on error.
 * @param     offSource        Fixup offset relative to page.
 * @param     ulModuleOrdinal  Module ordinal in the import module name table (1 based!)
 * @param     pszFnName        Pointer to a readonly function name for the imported function.
 * @sketch    IF not enough memory for the fixup THEN (try) allocate more memory
 *            Add function name to the import procedure name table.
 *            Fill in fixup record.
 *            Increment the size of the fixup records array (offCurFixupRec).
 *            IF cross page fixup THEN
 *            BEGIN
 *                Allocate memory for the cross page fixup record.
 *                Copy the fixup record we just created into the memory of the cross page fixup record..
 *                Substract the size of a Page from the source offset (r32_soff) in the cross page fixup record.
 *            END
 *            End successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Internal Fixup Record format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *           ÚÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *       00h ³ SRC ³FLAGS³SRCOFF/CNT*³
 *           ÃÄÄÄÄÄÁÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *   03h/04h ³ MOD ORD# *³ PROCEDURE NAME OFFSET*³     ADDITIVE * @      ³
 *           ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 *           ³ SRCOFF1 @ ³   . . .   ³ SRCOFFn @ ³
 *           ÀÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *         * These fields are variable size.
 *         @ These fields are optional.
 *
 *   Import by Name Fixup Record
 *
 *       MOD ORD # = D[B|W]  Ordinal index into the Import Module Name Table. This value is
 *       an ordered index in to the Import Module Name Table for the module containing the
 *       procedure entry point. It is a Byte value when the '16-bit Object Number/Module Ordinal'
 *       Flag bit in the target flags field is clear and a Word value when the bit is set. The
 *       loader creates a table of pointers with each pointer in the table corresponds to the
 *       modules named in the Import Module Name Table. This value is used by the loader to
 *       index into this table created by the loader to locate the referenced module.
 *
 *       PROCEDURE NAME OFFSET = D[W|D]  Offset into the Import Procedure Name Table. This
 *       field is an offset into the Import Procedure Name Table.  It is a Word value when
 *       the '32-bit Target Offset Flag' bit in the target flags field is clear and a Dword
 *       value when the bit is set.
 *
 *       ADDITIVE = D[W|D]  Additive fixup value. This field exists in the fixup record only
 *       when the 'Additive Fixup Flag' bit in the target flags field is set.  When the
 *       'Additive Fixup Flag' is clear the fixup record does not contain this field and is
 *       immediately followed by the next fixup record (or by the source offset list for this
 *       fixup record).  This value is added to the address derived from the target entry point.
 *       This field is a Word value when the '32-bit Additive Flag' bit in the target flags
 *       field is clear and a Dword value when the bit is set.
 *
 * ---------------------
 * This code got a bit dirty while trying to optimize memory usage.
 *
 */
ULONG  Pe2Lx::add32NameImportFixup(WORD offSource, ULONG ulModuleOrdinal, PCSZ pszFnName)
{
    APIRET          rc;
    struct r32_rlc *prlc;
    ULONG           cbFixup;         /* size of the fixup record. */
    ULONG           offFnName;

    /* enough memory? */
    AllocateMoreMemory(offCurFixupRec + 10 > cbFRAllocated /* 10 is max size */,
                       pFixupRecords, PVOID, cbFRAllocated, PAGESIZE, PAGESIZE)

    rc = addImportFunctionName(pszFnName, (PULONG)SSToDS(&offFnName));
    if (rc != NO_ERROR)
        return rc;

    /* fill in fixup record */
    cbFixup = 7UL;
    prlc = (struct r32_rlc *)((ULONG)pFixupRecords + offCurFixupRec);
    prlc->nr_stype = NROFF32;
    prlc->nr_flags = NRRNAM;
    if (ulModuleOrdinal > 255UL)
    {
        prlc->nr_flags |= NR16OBJMOD;
        cbFixup++;
    }
    #if 0 /* lxdump.exe (from Hacker's View release 5.66) don't support this. */
    if (offFnName < 256UL)
    {
        prlc->nr_flags |= NR8BITORD;
        cbFixup--;
    }
    #endif
    else if (offFnName > 65535UL)
    {
        prlc->nr_flags |= NR32BITOFF;
        cbFixup += 2;
    }
    prlc->r32_soff = (USHORT)offSource;
    prlc->r32_objmod = (USHORT)ulModuleOrdinal;
    if (prlc->nr_flags & NR16OBJMOD)
        prlc->r32_target.intref.offset32 = offFnName;
    else
        ((struct r32_rlc*)((ULONG)prlc - 1))->r32_target.intref.offset32 = offFnName;

    /* commit fixup */
    offCurFixupRec += cbFixup;

    /* cross page fixup? */
    if (offSource > PAGESIZE - 4UL)
    {   /* cross page fixup! */
        if (pvCrossPageFixup != NULL)
        {
            printWar(("A cross page fixup allready exists!\n"));
            free(pvCrossPageFixup);
        }
        pvCrossPageFixup = malloc((size_t)cbFixup);
        if (pvCrossPageFixup != NULL)
        {
            memcpy(pvCrossPageFixup, prlc, (size_t)cbFixup);
            cbCrossPageFixup = cbFixup;
            ((struct r32_rlc *)pvCrossPageFixup)->r32_soff -= (USHORT)PAGESIZE;
        }
        else
            return ERROR_NOT_ENOUGH_MEMORY;
    }

    printInfA(("offset=0x%03x modordinal=%d fnname=%s cbFixup=%d %s\n",
               offSource, ulModuleOrdinal, pszFnName, cbFixup, pvCrossPageFixup ? "crosspage" : ""));
    return NO_ERROR;
}


/**
 * Adds a modulename and returns the module ordinal.
 * If the module allready exists in the import module table then return the module ordinal only.
 * @returns   NO_ERROR
 *            ERROR_INVALID_PARAMETER
 *            ERROR_NOT_ENOUGH_MEMORY
 * @param     pszModuleName     Pointer to readonly string containing the modulename which is to be added.
 * @param     pulModuleOrdinal  Pointer to an ULONG which will hold the module ordinal value upon successfull return.
 * @sketch    validate input (pulModuleOrdinal)
 *            get/make Odin32 modulename (lie-list)
 *            IF modulename length > 127 THEN truncate it.
 *            IF module allready exists in the import module table THEN return successfully.
 *            IF not enough memory for modulename THEN (try) allocate some more.
 *            Set length and copy modulename.
 *            Update offCurImpModuleName.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Module table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *   ÚÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ     ÄÄÄÄÄÄ¿
 *   ³ LEN ³    ASCII STRING  . . .      ³
 *   ÀÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ     ÄÄÄÄÄÄÙ
 *
 *   LEN = DB  String Length. This defines the length of the string in bytes.
 *             The length of each ascii name string is limited to 255 characters.
 *   ASCII STRING = DB  ASCII String. This is a variable length string with it's
 *             length defined in bytes by the LEN field.  The string is case
 *             sensitive and is not null terminated.
 *
 *   The end of the import module name table is not terminated by a special character,
 *   it is followed directly by the import procedure name table.
 */
ULONG  Pe2Lx::addModule(PCSZ pszModuleName, PULONG pulModuleOrdinal)
{
    ULONG cchModuleName;
    ULONG offModule;

    #ifdef DEBUG
    /* validate input */
    if (pulModuleOrdinal < (PULONG)0x10000UL)
    {
        printErr(("Invalid (stack?) pointer passed in, 0x%08x\n", pulModuleOrdinal));
        return ERROR_INVALID_PARAMETER;
    }
    #endif

    /* check lie-list - get Odin32 modulename */
    pszModuleName = queryOdin32ModuleName(pszModuleName);

    /* length check */
    cchModuleName = strlen(pszModuleName);
    if (cchModuleName > 255UL) /* we've only got a byte to store the length in... */
    {
        printWar(("Modulename truncated! %s\n", pszModuleName));
        cchModuleName = 255;
    }

    /* check if module allready exists in the list and find module ordinal*/
    *pulModuleOrdinal = 1UL;
    offModule = 0UL;
    while (offModule < offCurImpModuleName)
    {
        if (strnicmp(&pachImpModuleNames[offModule + 1], pszModuleName,
                     (int)pachImpModuleNames[offModule]) == 0) /* case insensitive search - correct? */
        {
            return NO_ERROR;
        }
        offModule += 1 + pachImpModuleNames[offModule];
        (*pulModuleOrdinal)++;
    }

    printInf(("Modulename: %s  Ordinal: %d\n", pszModuleName, *pulModuleOrdinal));

    /* check if there is enough memory */
    AllocateMoreMemory(offCurImpModuleName + 1 + cchModuleName > cchIMNAllocated,
                       pachImpModuleNames, PCHAR, cchIMNAllocated,
                       1 + cchModuleName, 1 + cchModuleName)

    /* copy the modulename */
    pachImpModuleNames[offCurImpModuleName] = (CHAR)cchModuleName;
    memcpy(&pachImpModuleNames[offCurImpModuleName + 1], pszModuleName, (size_t)cchModuleName);

    offCurImpModuleName += cchModuleName + 1;

    return NO_ERROR;
}


/**
 * Adds a procedure name to the Import Procedure Table.
 * @returns   NO_ERROR
 *            ERROR_INVALID_PARAMETER
 *            ERROR_NOT_ENOUGH_MEMORY
 * @param     pszFnName   Pointer to readonly procname string.
 * @param     poffFnName  Pointer to variable which will hold the proc name offset on return.
 * @sketch    Validate pointer parameter poffFnName.
 *            Determin function name length. (max length 127)
 *            IF not enough memory for the function name THEN (try) allocate more memory
 *            Add function name to the import procedure name table.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Resident Name Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *   The import procedure name table defines the procedure name strings imported by this module
 *   through dynamic link references.  These strings are referenced through the imported
 *   relocation fixups.  To determine the length of the import procedure name table add the
 *   fixup section size to the fixup page table offset, this computes the offset to the end
 *   of the fixup section, then subtract the import procedure name table offset. These values
 *   are located in the linear EXE header. The import procedure name table is followed by the
 *   data pages section. Since the data pages section is aligned on a 'page size' boundary,
 *   padded space may exist between the last import name string and the first page in the
 *   data pages section.  If this padded space exists it will be zero filled.  The strings
 *   are CASE SENSITIVE and NOT NULL TERMINATED.  Each name table entry has the following
 *   format:
 *
 *          ÚÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ     ÄÄÄÄÄÄ¿
 *      00h ³ LEN ³    ASCII STRING  . . .      ³
 *          ÀÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ     ÄÄÄÄÄÄÙ
 *
 *   Import Procedure Name Table
 *
 *      LEN = DB  String Length. This defines the length of the string in bytes. The length
 *      of each ascii name string is limited to 255 characters. The high bit in the LEN field
 *      (bit 7) is defined as an Overload bit. This bit signifies that additional information
 *      is contained in the linear EXE module and will be used in the future for parameter
 *      type checking.
 *
 *      ASCII STRING = DB  ASCII String. This is a variable length string with it's length
 *      defined in bytes by the LEN field.  The string is case sensitive and is not null terminated.
 *
 *   Note: The first entry in the import procedure name table must be a null entry.  That is,
 *         the LEN field should be zero followed an empty ASCII STRING (no bytes).
 *
 * ----
 *
 * 255 bytes long when bit 7 is Overload bit?. We'll continue truncating to 127 bytes.
 *
 */
ULONG Pe2Lx::addImportFunctionName(PCSZ pszFnName, PULONG poffFnName)
{
    #ifdef DEBUG
    /* validate parameters */
    if (poffFnName < (PULONG)0x10000UL)
    {
        printErr(("poffFnName is invalid!, 0x%08x\n", poffFnName));
        return ERROR_INVALID_PARAMETER;
    }
    #endif

    ULONG   cchFnName = strlen(pszFnName);

    /* max function name length is 127 bytes */
    if (cchFnName > 127)
    {
        printWar(("function name truncated, %s", pszFnName));
        cchFnName = 127;
    }

    AllocateMoreMemory(offCurImpFunctionName + cchFnName + 1 > cchIFNAllocated,
                       pachImpFunctionNames, PCHAR, cchIFNAllocated, PAGESIZE, PAGESIZE)

    /* check if first entry */
    if (offCurImpFunctionName == 0UL)
    {   /* add null entry */
        pachImpFunctionNames[offCurImpFunctionName++] = '\0';
    }

    /* add function name */
    pachImpFunctionNames[offCurImpFunctionName] = (CHAR)cchFnName;
    memcpy(&pachImpFunctionNames[offCurImpFunctionName+1], pszFnName, (size_t)cchFnName);
    *poffFnName = offCurImpFunctionName;
    offCurImpFunctionName += 1 + cchFnName;

    return NO_ERROR;
}


/**
 * Releases unsused memory from the Fixup data structures.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
VOID Pe2Lx::finalizeFixups()
{
    if (paulFixupPageTable != NULL && cFixupPTEntries < cFPTEAllocated)
    {
        PVOID pv = realloc(paulFixupPageTable, (size_t)cFixupPTEntries * sizeof(ULONG));
        if (pv != NULL)
        {
            paulFixupPageTable = (PULONG)pv;
            cFPTEAllocated = cFixupPTEntries;
        }
    }

    if (pFixupRecords != NULL && offCurFixupRec < cbFRAllocated)
    {
        PVOID pv = realloc(pFixupRecords, offCurFixupRec);
        if (pv != NULL)
        {
            pFixupRecords = pv;
            cbFRAllocated = offCurFixupRec;
        }
    }
}


/**
 * Releases unsused memory from the Import data structures.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
VOID Pe2Lx::finalizeImportNames()
{
    if (pachImpModuleNames != NULL && offCurImpModuleName < cchIMNAllocated)
    {
        PVOID pv = realloc(pachImpModuleNames, offCurImpModuleName);
        if (pv != NULL)
        {
            pachImpModuleNames = (PCHAR)pv;
            cchIMNAllocated = offCurImpModuleName;
        }
    }

    if (pachImpFunctionNames != NULL && offCurImpFunctionName < cchIFNAllocated)
    {
        PVOID pv = realloc(pachImpFunctionNames, offCurImpFunctionName);
        if (pv != NULL)
        {
            pachImpFunctionNames = (PCHAR)pv;
            cchIFNAllocated = offCurImpFunctionName;
        }
    }
}


/**
 * Initiates the entry (export) related data members of this object.
 * Forwarders are concidered.
 * @returns   NO_ERROR
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG  Pe2Lx::initEntry()
{
    APIRET rc;

    if (pEntryBundles != NULL)
    {
        free(pEntryBundles);
        pEntryBundles = NULL;
    }
    offCurEntryBundle  = 0UL;
    offLastEntryBundle = 0UL;
    ulLastOrdinal = 0UL;
    cbEBAllocated = 0UL;

    if (pachResNameTable)
    {
        free(pachResNameTable);
        pachResNameTable = NULL;
    }
    offCurResName   = 0UL;
    cchRNTAllocated = 0UL;

    /* If this is a second call to makeExports, the fForwarders flag may be set. */
    /* When it's set we'll have to clean up the fixup structures too. */
    if (fForwarders)
    {
        fForwarders = FALSE;
        rc = initFixups();
        fForwarders = TRUE;
    }
    else
        rc = NO_ERROR;

    return  rc;
}


/**
 * Adds a name/ordinal to the resident name table.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 * @param     ulOrdinal  Ordinal number for this name.
 * @param     pszName    Pointer to read only name string. (Don't have to be zero-termitated)
 * @param     cchName    Numbers of chars to copy or ~0UL to copy until zero-termintaion.
 * @sketch    IF cchName == ~0L THEN get name length.
 *            IF name length > 127 THEN truncate name.
 *            IF not enough memory THEN (try) allocate more memory.
 *            Add name:
 *              Set length.
 *              Copy name
 *              Set ordinal
 *              Update offCurResName to point the next (unused) name entry.
 *            return successfully.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Resident Name Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *   The resident and non-resident name tables define the ASCII names and ordinal numbers for
 *   exported entries in the module. In addition the first entry in the resident name table
 *   contains the module name. These tables are used to translate a procedure name string into
 *   an ordinal number by searching for a matching name string. The ordinal number is used to
 *   locate the entry point information in the entry table.  The resident name table is kept
 *   resident in system memory while the module is loaded.  It is intended to contain the
 *   exported entry point names that are frequently dynamicaly linked to by name.  Non-resident
 *   names are not kept in memory and are read from the EXE file when a dynamic link reference
 *   is made.  Exported entry point names that are infrequently dynamicaly linked to by name or
 *   are commonly referenced by ordinal number should be placed in the non-resident name table.
 *   The trade off made for references by name is performance vs memory usage. Import references
 *   by name require these tables to be searched to obtain the entry point ordinal number.
 *   Import references by ordinal number provide the fastest lookup since the search of these
 *   tables is not required.
 *
 *   Installable File Systems, Physical Device Drivers, and Virtual Device Drivers are closed
 *   after the file is loaded.  Any refeference to the non-resident name table after this time
 *   will fail. The strings are CASE SENSITIVE and are NOT NULL TERMINATED. Each name table entry
 *   has the following format:
 *
 *
 *       ÚÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ     ÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *   00h ³ LEN ³    ASCII STRING  . . .      ³ ORDINAL # ³
 *       ÀÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ     ÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *   Resident or Non-resident Name Table Entry
 *
 *     LEN = DB  String Length. This defines the length of the string in bytes. A zero length
 *     indicates there are no more entries in table. The length of each ascii name string is
 *     limited to 255 characters.  The high bit in the LEN field (bit 7) is defined as an Overload
 *     bit. This bit signifies that additional information is contained in the linear EXE module
 *     and will be used in the future for parameter type checking.
 *
 *     ASCII STRING = DB  ASCII String. This is a variable length string with it's length defined
 *     in bytes by the LEN field.  The string is case case sensitive and is not null terminated.
 *
 *     ORDINAL # = DW  Ordinal number. The ordinal number in an ordered index into the entry table
 *     for this entry point.
 *
 */
ULONG  Pe2Lx::addResName(ULONG ulOrdinal, PCSZ pszName, ULONG cchName)
{
    /* IF cchName == ~0L THEN get name length. */
    if (cchName == ~0UL)
        cchName = strlen(pszName);

    /* IF name length > 127 THEN truncate name. */
    if (cchName > 127)
    {
        printWar(("Resident name truncated! %s\n", pszName));
        cchName = 127;
    }

    /* IF not enough memory THEN (try) allocate more memory. */
    AllocateMoreMemory(cchName + 1 + 2 + offCurResName > cchRNTAllocated,
                       pachResNameTable, PCHAR, cchRNTAllocated, 2048, 1024)

    /* Add name */
    pachResNameTable[offCurResName] = (CHAR)cchName;
    memcpy(&pachResNameTable[offCurResName + 1], pszName, (size_t)cchName);
    *(PUSHORT)&pachResNameTable[offCurResName + 1 + cchName] = (USHORT)ulOrdinal;

    /* Update offCurResName to point the next (unused) name entry. */
    offCurResName += 1 + cchName + 2;

    printInf(("Resident Name: %.*s ordinal:%d\n", cchName, pszName, ulOrdinal));

    return NO_ERROR;
}


/**
 * Adds an entry to the entry table.
 * The way this is implemented, it is REQUIRED that the entries are added in strict
 * ascending order by ordinal.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INVALID_PARAMETER
 * @param     ulOrdinal  Ordinal number of the entry point.
 * @param     ulRVA      The RVA of the entry point.
 * @sketch    Validate input. (ulOrdinal)
 *            Find object and offObject corresponding to the entrypoint RVA.
 *            IF no enough memory THEN (try) allocate more.
 *            LOOP WHILE last ordinal + 1 != new ordinal
 *            BEGIN
 *                Add unused entry which skips to the new ordinal - 1.
 *                Update offCurEntryBundle
 *                Set offLastEntryBundle to offLastEntryBundle.
 *                IF no enough memory THEN (try) allocate more.
 *            END
 *            IF offCurEntryBundle == offLastEntryBundle OR last bundle type != 32-bit entry
 *               OR last bundle object != this object OR bundle is full THEN
 *            BEGIN
 *                Add an empty 32-bit bundle.
 *                Set offLastEntryBundle to start of new 32-bit bundle.
 *                Set offCurEntryBundle to end of of the new 32-bit bundle.
 *            END
 *            Add Flags (01h) and 32-bit offset.
 *            Update offCurEntryBundle.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Entry Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *   The entry table contains object and offset information that is used to resolve fixup
 *   references to the entry points within this module. Not all entry points in the entry
 *   table will be exported, some entry points will only be used within the module.  An ordinal
 *   number is used to index into the entry table.  The entry table entries are numbered
 *   starting from one.  The list of entries are compressed into 'bundles', where possible. The
 *   entries within each bundle are all the same size. A bundle starts with a count field which
 *   indicates the number of entries in the bundle.  The count is followed by a type field which
 *   identifies the bundle format.  This provides both a means for saving space as well as a
 *   mechanism for extending the bundle types.  The type field allows the definition of 256
 *   bundle types.  The following bundle types will initially be defined:
 *      Unused Entry.
 *      16-bit Entry.
 *      286 Call Gate Entry.
 *      32-bit Entry.
 *      Forwarder Entry.
 *
 *   The bundled entry table has the following format:
 *          ÚÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *      00h ³ CNT ³TYPE ³ BUNDLE INFO . . ³
 *          ÀÄÄÄÄÄÁÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 *
 *   Entry Table
 *
 *      CNT = DB  Number of entries. This is the number of entries in this bundle.  A zero
 *      value for the number of entries identifies the end of the entry table.  There is no
 *      further bundle information when the number of entries is zero.  In other words the
 *      entry table is terminated by a single zero byte.
 *
 *      TYPE = DB  Bundle type. This defines the  bundle type  which determines the contents
 *      of the BUNDLE INFO.  The follow types are defined:
 *                     00h = Unused Entry.
 *                     01h = 16-bit Entry.
 *                     02h = 286 Call Gate Entry.
 *                     03h = 32-bit Entry.
 *                     04h = Forwarder Entry.
 *                     80h = Parameter Typing Information Present. This bit signifies that
 *                           additional information is contained in the linear EXE module
 *                           and will be used in the future for parameter type checking.
 *
 *     The following is the format for each bundle type:
 *
 *     Unused Entry:
 *     ------------
 *                ÚÄÄÄÄÄÂÄÄÄÄÄ¿
 *            00h ³ CNT ³TYPE ³
 *                ÀÄÄÄÄÄÁÄÄÄÄÄÙ
 *
 *           CNT = DB  Number of entries. This is the number of unused entries to skip.
 *
 *           TYPE = DB  0 (Unused Entry)
 *
 *     32-bit Entry:
 *     ------------
 *                ÚÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *            00h ³ CNT ³TYPE ³   OBJECT  ³
 *                ÃÄÄÄÄÄÅÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄ¿
 *            04h ³FLAGS³        OFFSET         ³
 *                ÃÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 *            09h ³ ... ³         . . .         ³
 *
 *
 *           CNT = DB  Number of entries. This is the number of 32-bit entries in this bundle.
 *           The flags and offset value are repeated this number of times.
 *
 *           TYPE = DB  3 (32-bit Entry) The 32-bit Entry type will only be defined by the
 *           linker when the offset in the object can not be specified by a 16-bit offset.
 *
 *           OBJECT = DW  Object number. This is the object number for the entries in this bundle.
 *
 *           FLAGS = DB  Entry flags. These are the flags for this entry point.  They  have the
 *           following definition.
 *                     01h = Exported entry flag.
 *                     F8h = Parameter dword count mask.
 *
 *           OFFSET = DD  Offset in object. This is the offset in the object for the entry point
 *           defined at this ordinal number.
 *
 */
ULONG  Pe2Lx::addEntry(ULONG ulOrdinal, ULONG ulRVA)
{
    APIRET              rc;
    ULONG               iObj;
    ULONG               offObject;
    struct b32_bundle  *pBundle;
    struct e32_entry   *pEntry;

    #ifdef DEBUG
    /* Validate input. (ulOrdinal) */
    if (ulOrdinal < ulLastOrdinal + 1UL || ulOrdinal > 65535UL)
    {
        printErr(("Invalid ordinal. ulOrdinal %d, ulLastOrdinal %d.\n", ulOrdinal, ulLastOrdinal));
        return ERROR_INVALID_PARAMETER;
    }
    #endif

    /* Find object and offObject corresponding to the entrypoint RVA. */
    rc = queryObjectAndOffset(ulRVA, (PULONG)SSToDS(&iObj), (PULONG)SSToDS(&offObject));
    if (rc != NO_ERROR)
        return rc;

    /* IF no enough memory THEN (try) allocate more. */
    AllocateMoreMemory(offCurEntryBundle + 4 + 2 + 5  > cbEBAllocated /* max memory uasage! Should detect more exact memory usage! */,
                       pEntryBundles, struct b32_bundle *, cbEBAllocated , 512, 256)

    /* Add unused entry to skip ordinals? */
    while (ulOrdinal > ulLastOrdinal + 1)
    {
        /* Add unused entry which skips to the new ordinal - 1.*/
        pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offCurEntryBundle);
        pBundle->b32_cnt = (ulOrdinal - ulLastOrdinal - 1) < 0x100 ?
            (UCHAR)(ulOrdinal - ulLastOrdinal - 1) : (UCHAR)0xff;
        pBundle->b32_type = EMPTY;
        ulLastOrdinal += pBundle->b32_cnt;

        /* Update offCurEntryBundle and offLastEntryBundle */
        offLastEntryBundle = offCurEntryBundle += 2UL;

        /* IF no enough memory THEN (try) allocate more. */
        AllocateMoreMemory(offCurEntryBundle + 4 + 2 + 5  > cbEBAllocated /* max memory uasage! Should detect more exact memory usage! */,
                           pEntryBundles, struct b32_bundle *, cbEBAllocated , 512, 256)
    }

    /* new entry32 bundle? */
    pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offLastEntryBundle);
    if (offCurEntryBundle == offLastEntryBundle || pBundle->b32_type != ENTRY32
        || pBundle->b32_obj != iObj || pBundle->b32_cnt == 255)
    {
        /* Add an empty 32-bit bundle. */
        pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offCurEntryBundle);
        pBundle->b32_cnt  = 0;
        pBundle->b32_type = ENTRY32;
        pBundle->b32_obj  = (USHORT)iObj;
        /* Set offLastEntryBundle to start of new 32-bit bundle. */
        offLastEntryBundle = offCurEntryBundle;
        /* Set offCurEntryBundle to end of of the new 32-bit bundle. */
        offCurEntryBundle += sizeof(struct b32_bundle);
    }

    /* Add Flags (01h) and 32-bit offset. */
    pEntry = (struct e32_entry *)((ULONG)pEntryBundles + offCurEntryBundle);
    pEntry->e32_flags = E32EXPORT;
    pEntry->e32_variant.e32_offset.offset32 = offObject;
    pBundle->b32_cnt++;

    /* Update offCurEntryBundle. */
    offCurEntryBundle += FIXENT32;
    ulLastOrdinal = ulOrdinal;

    printInfA(("Export entry: ulOrdinal=%d  ulRVA=%#5x  iObj=%2d  offObject=%#5x\n",
               ulOrdinal, ulRVA, iObj, offObject));

    return NO_ERROR;
}


/**
 * Adds a forwarder entry.
 * The way this is implemented, it is REQUIRED that the entries are added in strict
 * ascending order by ordinal.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INVALID_PARAMETER
 * @param     ulOrdinal       Entry ordinal number.
 * @param     pszDllName      Pointer to string containing the dllname.
 * @param     pszFnNameOrOrd  Pointer to string containing either a function or an ordinal.
 *                            Note. Have currently not found any example of an ordinal...
 *                            That is just an assumption!
 * @sketch    Set forwarder flag.
 *            Validate input. (ulOrdinal and pointers)
 *            IF no enough memory THEN (try) allocate more.
 *            IF Function ordinal THEN convert it to an ordinal number.
 *            Make sure kernel32 is the first imported module.
 *            Add module name.
 *            IF not forwarder to ordinal THEN Add name to imported procedure table.
 *            IF last ordinal + 1 != new ordinal THEN
 *            BEGIN
 *                Add unused entry which skips to the new ordinal - 1.
 *                Update offCurEntryBundle
 *                Set offLastEntryBundle to offLastEntryBundle.
 *            END
 *            IF offCurEntryBundle == offLastEntryBundle OR last bundle type != forwarder entry
 *               OR bundle is full THEN
 *            BEGIN
 *                Add an empty forwarder bundle.
 *                Set offLastEntryBundle to start of the new forwarder bundle.
 *                Set offCurEntryBundle to end of of the new forwarder bundle.
 *            END
 *            Add Flags, module ordinal and name offset/ordinal number.
 *            Update offCurEntryBundle.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Entry Table format (from Linear eXecutable Module Format,lxspec.inf, Rev.8):
 *
 *     Forwarder Entry:
 *     ---------------
 *           ÚÄÄÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
 *       00h ³ CNT ³TYPE ³ RESERVED  ³
 *           ÃÄÄÄÄÄÅÄÄÄÄÄÁÄÄÄÄÄÂÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 *       04h ³FLAGS³ MOD ORD#  ³ OFFSET / ORDNUM       ³
 *           ÃÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 *       09h ³ ... ³    ...    ³          ...          ³
 *
 *
 *      CNT = DB  Number of entries. This is the number of forwarder entries in this bundle.
 *      The FLAGS, MOD ORD#, and OFFSET/ORDNUM values are repeated this number of times.
 *
 *      TYPE = DB  4 (Forwarder Entry)
 *
 *      RESERVED = DW 0 This field is reserved for future use.
 *
 *      FLAGS = DB  Forwarder flags. These are the flags for this entry point. They have the
 *      following definition.
 *                01h = Import by ordinal.
 *                F7h = Reserved for future use; should be zero.
 *
 *      MOD ORD# = DW Module Ordinal Number This is the index into the Import Module Name Table
 *      for this forwarder.
 *
 *      OFFSET / ORDNUM = DD Procedure Name Offset or Import Ordinal Number If the FLAGS field
 *      indicates import by ordinal, then this field is the ordinal number into the Entry Table
 *      of the target module, otherwise this field is the offset into the Procedure Names Table
 *      of the (?)target(?) module.
 *
 *   A Forwarder entry (type = 4) is an entry point whose value is an imported reference.  When a
 *   load time fixup occurs whose target is a forwarder, the loader obtains the address imported
 *   by the forwarder and uses that imported address to resolve the fixup.
 *
 *   A forwarder may refer to an entry point in another module which is itself a forwarder, so there
 *   can be a chain of forwarders.  The loader will traverse the chain until it finds a non-forwarded
 *   entry point which terminates the chain , and use this to resolve the original fixup. Circular
 *   chains are detected by the loader and result in a load time error.  A maximum of 1024
 *   forwarders is allowed in a chain; more than this results in a load time error.
 *
 *   Forwarders are useful for merging and recombining API calls into different sets of libraries,
 *   while maintaining compatibility with applications. For example, if one wanted to combine
 *   MONCALLS, MOUCALLS, and VIOCALLS into a single libraries, one could provide entry points
 *   for the three libraries that are forwarders pointing to the common implementation.
 *
 * ---------------
 *
 *  Forwarder makes some minor difficulties concerning function names.
 *  We then have to
 *    1) Allways convert entries before imports (fixups).
 *    2) When forwarders are present makeFixups can't be called without calling makeExports first.
 *    3) initEntries will clean up import variables too if fForwarders is set.
 */
ULONG  Pe2Lx::addForwarderEntry(ULONG ulOrdinal, PCSZ pszDllName, PCSZ pszFnNameOrOrd)
{
    APIRET              rc;
    ULONG               ulFnOrdinal;     /* function ordinal or function offset into import procdure table.
                                          * The high bit is set when it is an ordinal value. (remember to mask it off!)
                                          * The high bit is clear when it is an offset. */
    ULONG               ulModuleOrdinal; /* Module ordinal */
    struct b32_bundle  *pBundle;         /* pointer to current (offLast...) entry bundle. */
    struct e32_entry   *pEntry;          /* pointer to new entry (new entry in the array imediately following the bundle) . */
    PCSZ                psz;             /* temporary string pointer. */

    /* Set forwarder flag. */
    fForwarders = TRUE;

    /* Validate input. (ulOrdinal and pointers) */
    if (ulOrdinal < ulLastOrdinal + 1UL || ulOrdinal > 65535UL)
    {
        printErr(("Invalid ordinal. ulOrdinal %d, ulLastOrdinal %d.\n", ulOrdinal, ulLastOrdinal));
        return ERROR_INVALID_PARAMETER;
    }
    #ifdef DEBUG
    if (pszDllName < (PCSZ)0x10000UL || pszFnNameOrOrd < (PCSZ)0x10000UL)
        return ERROR_INVALID_PARAMETER;
    #endif

    /* IF no enough memory THEN (try) allocate more. */
    AllocateMoreMemory(offCurEntryBundle + 4 + 2 + 7  > cbEBAllocated /* max memory uasage! Should detect more exact memory usage! */,
                       pEntryBundles, struct b32_bundle *, cbEBAllocated , 512, 256)

    /* IF Function ordinal THEN convert it to an ordinal number. */
    ulFnOrdinal = 0UL;
    psz = pszFnNameOrOrd;
    while (*psz != '\0' && *psz >= '0' && *psz <= '9')
        ulFnOrdinal = (ulFnOrdinal*10) + *psz - '0';
    if (*psz == '\0')
        ulFnOrdinal |= 0x80000000UL; /* ordinal flag */
    else
        ulFnOrdinal = 0; /* not ordinal! */

    /* Make sure kernel32 is the first imported module */
    if (offCurImpModuleName == 0)
    {
        rc = addModule("KERNEL32.DLL", (PULONG)SSToDS(&ulModuleOrdinal));
        if (rc != NO_ERROR)
            return rc;
    }

    /* Add module name. */
    rc = addModule(pszDllName, (PULONG)SSToDS(&ulModuleOrdinal));
    if (rc != NO_ERROR)
        return rc;

    /* IF not forwarder to ordinal THEN Add name to imported procedure table. */
    if (!(ulFnOrdinal & 0x80000000UL))
    {
        rc = addImportFunctionName(pszFnNameOrOrd, (PULONG)SSToDS(&ulFnOrdinal));
        if (rc != NO_ERROR)
            return rc;
    }

    /* Add unused entry to skip ordinals? */
    while (ulOrdinal > ulLastOrdinal + 1)
    {
        /* Add unused entry which skips to the new ordinal - 1.*/
        pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offCurEntryBundle);
        pBundle->b32_cnt = (ulOrdinal - ulLastOrdinal - 1) < 0x100 ?
            (UCHAR)(ulOrdinal - ulLastOrdinal - 1) : (UCHAR)0xff;
        pBundle->b32_type = EMPTY;
        ulLastOrdinal += pBundle->b32_cnt;

        /* Update offCurEntryBundle and offLastEntryBundle */
        offLastEntryBundle = offCurEntryBundle += 2UL;

        /* IF no enough memory THEN (try) allocate more. */
        AllocateMoreMemory(offCurEntryBundle + 4 + 2 + 7  > cbEBAllocated /* max memory uasage! Should detect more exact memory usage! */,
                           pEntryBundles, struct b32_bundle *, cbEBAllocated , 512, 256)
    }

    /* new forwarder bundle? */
    pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offLastEntryBundle);
    if (offCurEntryBundle == offLastEntryBundle || pBundle->b32_type != ENTRYFWD
        || pBundle->b32_cnt == 255)
    {
        /* Add an empty 32-bit bundle. */
        pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offCurEntryBundle);
        pBundle->b32_cnt  = 0;
        pBundle->b32_type = ENTRYFWD;
        pBundle->b32_obj  = 0;
        /* Set offLastEntryBundle to start of the new forwarder bundle. */
        offLastEntryBundle = offCurEntryBundle;
        /* Set offCurEntryBundle to end of of the new forwarder bundle. */
        offCurEntryBundle += sizeof(struct b32_bundle);
    }

    /* Add Flags, module ordinal and name offset/ordinal number. */
    pEntry = (struct e32_entry *)((ULONG)pEntryBundles + offCurEntryBundle);
    pEntry->e32_flags =  (UCHAR)(ulFnOrdinal & 0x80000000UL ? FWD_ORDINAL : 0);
    pEntry->e32_variant.e32_fwd.modord = (USHORT)ulModuleOrdinal;
    pEntry->e32_variant.e32_fwd.value  = ulFnOrdinal & 0x7fffffffUL;
    pBundle->b32_cnt++;

    /* Update offCurEntryBundle. */
    offCurEntryBundle += FWDENT;
    ulLastOrdinal = ulOrdinal;

    printInfA(("Forwarder Export entry: ulOrdinal=%d   pszDllName=%s  pszFnNameOrOrd=%s\n",
               ulOrdinal, pszDllName, pszFnNameOrOrd));

    return NO_ERROR;
}


/**
 * Adds the closing entry bundle.
 * @returns   NO_ERROR
 *            ERROR_NOT_ENOUGH_MEMORY
 * @sketch    IF no enough memory THEN (try) allocate more.
 *            fill in final bundle. (cnt=0 only)
 *            Update off*.
 *            return successfully.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    Should only be called to close a sequence of addEntry and addForwarderEntry calls.
 */
ULONG  Pe2Lx::addLastEntry()
{
    struct b32_bundle  *pBundle;         /* pointer to current (offLast...) entry bundle. */

    /* IF no enough memory THEN (try) allocate more. */
        /* table is terminated by a single '\0' byte according to the docs. */
    AllocateMoreMemory(offCurEntryBundle + 1  > cbEBAllocated,
                       pEntryBundles, struct b32_bundle *, cbEBAllocated, 1, 1)

    /* fill in final bundle (cnt = 0!) */
    pBundle = (struct b32_bundle *)((ULONG)pEntryBundles + offCurEntryBundle);
    pBundle->b32_cnt  = 0;

    /* Update offLastEntryBundle and offLastEntryBundle. */
    offLastEntryBundle = offCurEntryBundle += 1;

    return NO_ERROR;
}


/**
 * Releases unsused memory from the Export data structures.
 * @status    completely implemented; tested
 * @author    knut st. osmundsen
 */
VOID  Pe2Lx::finalizeExports()
{
    if (pEntryBundles != NULL && offCurEntryBundle < cbEBAllocated)
    {
        PVOID pv = realloc(pEntryBundles, offCurEntryBundle);
        if (pv != NULL)
        {
            pEntryBundles = (struct b32_bundle*)pv;
            cbEBAllocated = offCurEntryBundle;
        }
    }

    if (pachResNameTable != NULL && offCurResName < cchRNTAllocated)
    {
        PVOID pv = realloc(pachResNameTable, offCurResName);
        if (pv != NULL)
        {
            pachResNameTable = (PCHAR)pv;
            cchRNTAllocated = offCurEntryBundle;
        }
    }
}


/**
 * Gets the number of pages in the virtual lx file.
 * @returns   Number of pages. (0UL is a valid return!)
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    fAllInOneObject should be established before this function is called!
 */
ULONG Pe2Lx::getCountOfPages()
{
    ULONG cPages = 0UL;
    ULONG iObj;

    for (iObj = 0; iObj < cObjects; iObj++)
    {
        if (fAllInOneObject && iObj + 1 < cObjects)
            cPages += ALIGN(paObjects[iObj+1].ulRVA - paObjects[iObj].ulRVA, PAGESIZE) >> PAGESHIFT;
        else
            cPages += ALIGN(paObjects[iObj].cbVirtual, PAGESIZE) >> PAGESHIFT;
    }

    return cPages;
}


/**
 * Gets the object number (1 based!) and offset into that object.
 * @returns   NO_ERROR
 *            ERROR_INVALID_PARAMETER
 * @param     ulRVA
 * @param     pulObject   Pointer to variable which will hold the object number upon return. (1 based)
 * @param     poffObject  Pointer to variabel which will hold the object offset upon return.
 * @sketch    Validate input pointers.
 *            find object. (be aware of fAllInOneObject!)
 *            IF object not found THEN return error invalid parameter.
 *            Set output parameters.
 *            return successfully.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    ulRVA points within a valid (LX term) page area.
 */
ULONG  Pe2Lx::queryObjectAndOffset(ULONG ulRVA, PULONG pulObject, PULONG poffObject)
{
    ULONG iObj;

    #ifdef DEBUG
    /* validate passed in pointers. */
    if (pulObject < (PULONG)0x10000UL)
    {
        printErr(("Invalid parameter pulObject.\n"));
        return ERROR_INVALID_PARAMETER;
    }
    if (poffObject < (PULONG)0x10000UL)
    {
        printErr(("Invalid parameter poffObject.\n"));
        return ERROR_INVALID_PARAMETER;
    }
    #endif

    /* find object */
    if (!fAllInOneObject)
    {
        iObj = 0;
        while (iObj < cObjects && paObjects[iObj].ulRVA + paObjects[iObj].cbVirtual <= ulRVA)
            iObj++;

        if (iObj >= cObjects || paObjects[iObj].ulRVA > ulRVA)
            return ERROR_INVALID_PARAMETER;
    }
    else
    {   /* one large object! */
        if (cObjects != 0 && ulRVA >= paObjects[0].ulRVA
            && ulRVA <= paObjects[cObjects-1].ulRVA + paObjects[cObjects-1].cbVirtual)
            iObj = 0;
        else
            return ERROR_INVALID_PARAMETER;
    }

    /* set output */
    *poffObject = ulRVA - paObjects[iObj].ulRVA;
    *pulObject = iObj + 1;

    return NO_ERROR;
}


/**
 * Reads a chunk of data at the spcified RVA.
 * @returns   NO_ERROR on success.
 *            ERROR_INVALID_PARAMETER
 *            <Whatever rc ReadAt returns>
 * @param     ulRVA     RVA to read from. Within the filesize.
 * @param     pvBuffer  Pointer to output buffer. pvBuffer > 64KB
 * @param     cbBuffer  Number of bytes to read. 0 < cbBuffer > 256MB
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG Pe2Lx::readAtRVA(ULONG ulRVA, PVOID pvBuffer, ULONG cbBuffer)
{
    #ifdef DEBUG
    if (ulRVA == ~0UL || (ULONG)pvBuffer < 0x10000UL || cbBuffer == 0UL || cbBuffer >= 0x10000000UL)
        return ERROR_INVALID_PARAMETER;
    #endif

    ULONG iObj = 0;
    while (cbBuffer != 0UL)
    {
        while (iObj < cObjects
               && ulRVA >= (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                            : paObjects[iObj].ulRVA + ALIGN(paObjects[iObj].cbVirtual, PAGESIZE))
               )
            iObj++;
        if (iObj >= cObjects)
            return ERROR_INVALID_PARAMETER;

        /* ulRVA points at physical or virtual data? */
        if (ulRVA < paObjects[iObj].ulRVA + paObjects[iObj].cbPhysical)
        {   /* physical data - read from file */
            APIRET rc;
            ULONG  cbToRead = min(paObjects[iObj].ulRVA + paObjects[iObj].cbPhysical - ulRVA, cbBuffer);
            rc = ReadAt(hFile,
                        ulRVA - paObjects[iObj].ulRVA + paObjects[iObj].offPEFile,
                        pvBuffer,
                        cbToRead
                        );
            if (rc != NO_ERROR)
                return rc;
            ulRVA += cbToRead;
            cbBuffer -= cbToRead;
            pvBuffer = (void*)((unsigned)pvBuffer + cbToRead);
        }
        else
        {   /* virtual data - memset(,0,) */
            ULONG cbToSet = (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                              : paObjects[iObj].ulRVA + ALIGN(paObjects[iObj].cbVirtual, PAGESIZE))
                            - ulRVA; /* calcs size of virtual data left in this object */
            cbToSet = min(cbToSet, cbBuffer);

            memset(pvBuffer, 0, (size_t)cbToSet);
            ulRVA += cbToSet;
            cbBuffer -= cbToSet;
            pvBuffer = (void*)((unsigned)pvBuffer + cbToSet);
        }
    } /* while */

    return NO_ERROR;
}



/**
 * Check if the modulename exists in the lielist. If not the passed in modulename is returned.
 * @returns   Pointer (readonly) to Odin32 modulename.
 * @param     pszWin32ModuleName  Win32 modulename.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    static method.
 */
PCSZ Pe2Lx::queryOdin32ModuleName(PCSZ pszWin32ModuleName)
{
    int i = 0;

    while (paLieList[i].pszWin32Name != NULL)
    {
        if (stricmp(paLieList[i].pszWin32Name, pszWin32ModuleName) == 0)
            return paLieList[i].pszOdin32Name;
        i++;
    }

    return pszWin32ModuleName;
}



/**
 * Initiates the odin32path.
 * @returns     Success indicator.
 * @sketch      If allready inited ok Then do nothing return TRUE.
 *
 *              Check if KERNEL32 is loaded using ldrFindModule.
 *              If loaded then set path according to the smte_path and return.
 *
 *              If the path is set to something then return TRUE. (ie. the following method is allready applied.)
 *
 *              Use odinPath2 to locate the KERNEL32 module in the LIBPATHs. The
 *                win32k loaders are temporarily disabled. Path is returned in
 *                the ldrpFileNameBuf buffer.
 *              If found the Then set path according to ldrpFileNameBuf and return
 *
 *              Fail returning FALSE.
 * @status
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
BOOL Pe2Lx::initOdin32Path()
{
    #ifdef RING0
    APIRET rc;
    PMTE   pMTE;


    if (sfnKernel32 != NULLHANDLE)
        return TRUE;

    /*
     * Try find it using ldrFindModule.
     */
    pMTE = NULL;
    rc = ldrFindModule("KERNEL32", 8, CLASS_GLOBAL, (PPMTE)SSToDS(&pMTE));
    if (rc == NO_ERROR && pMTE != NULL && pMTE->mte_swapmte != NULL)
    {
        /*
         * We now take the smte_path. Start at the end and skip the filename,
         * and one directory up. We assume a fully qualified path is found in
         * smte_path.
         */
        if (pMTE->mte_swapmte->smte_path != NULL)//paranoia
        {
            sfnKernel32 = pMTE->mte_sfn;
            return setOdin32Path(pMTE->mte_swapmte->smte_path);
        }
    }


    /*
     * KERNEL32 isn't loaded. We'll only search the paths if
     */
    if (pszOdin32Path != NULL)
        return TRUE;


    /*
     * Try find it searching the LIBPATHs.
     *
     * For the time being:
     *  We'll use odinPath2 to do this, but we'll have to
     *  disable the win32k.sys overloading temporarily.
     */
    ldrlv_t lv = {0};
    ULONG   ful = 0;
    ULONG   ul;

    ul = options.fNoLoader;
    options.fNoLoader = TRUE;
    lv.lv_class = CLASS_GLOBAL;
    rc = openPath2("KERNEL32", 8, (ldrlv_t*)SSToDS(&lv), (PULONG)SSToDS(&ful), 3, FALSE);
    options.fNoLoader = ul;
    if (rc == NO_ERROR)
    {
        /*
         * Set the odin32path according to the kernel32 path we've found.
         * (ldrOpen sets ldrpFileNameBuf to the fully qualified path of
         *  the last opended filed, which in this case is kernel32.dll.)
         * We'll close the file handle first of course.
         */
        rc = setOdin32Path(ldrpFileNameBuf);
        ldrClose(lv.lv_sfn);
        return rc;
    }

    #endif
    return FALSE;
}



/**
 * Sets the Odin32Path to the given fully qualified filename of kernel32.
 * @returns     Success indicator.
 * @param       psz     Fully qualified filename of kernel32 with path.
 * @sketch
 * @status
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
BOOL Pe2Lx::setOdin32Path(const char *psz)
{
    const char * psz2;

    /*
     * We now take the psz. Start at the end and skip the filename,
     * and one directory up. We assume a fully qualified path.
     */
    psz2 = psz + strlen(psz) - 1;
    while (psz2 > psz && *psz2 != '\\' && *psz2 != '/' && *psz2 != ':')
        psz2--;
    psz2--;
    while (psz2 > psz && *psz2 != '\\' && *psz2 != '/' && *psz2 != ':')
        psz2--;
    if (psz2 > psz)
    {
        char *pszPath;
        /*
         * Free old path (if any) and allocate space for a new path.
         * Copy the path including the slash.
         * Remember the kernel32 filehandle (to be able to invalidate the path).
         */
        if (pszOdin32Path)
            rfree((void*)pszOdin32Path);
        if (*psz2 == ':') //in case someone installed odin in a root directory.
            psz2++;
        psz2++;       //include the slash
        cchOdin32Path = psz2 - psz;
        pszPath = (char*)rmalloc((size_t)cchOdin32Path);
        if (pszPath == NULL) return FALSE;
        memcpy(pszPath, psz, (size_t)cchOdin32Path);
        pszPath[cchOdin32Path] = '\0';
        pszOdin32Path = pszPath;

        return TRUE;
    }

    return FALSE;
}




/**
 * Static method which dumps a set of nt headers.
 * @param     pNtHdrs  Pointer to nt headers.
 * @status    Completed
 * @author    knut st. osmundsen
 */
VOID Pe2Lx::dumpNtHeaders(PIMAGE_NT_HEADERS pNtHdrs)
{
    if (pNtHdrs >= (PIMAGE_NT_HEADERS)0x10000)
    {
        int   i;
        WORD  w;

        printInf(("\nPE-headers - File header\n"));
        printInf(("Signature               %.2s\n", &pNtHdrs->Signature));
        printInf(("Machine                 0x%08x\n",  pNtHdrs->FileHeader.Machine));
        switch (pNtHdrs->FileHeader.Machine)
        {
            case IMAGE_FILE_MACHINE_UNKNOWN:    printInf(("    IMAGE_FILE_MACHINE_UNKNOWN\n")); break;
            case IMAGE_FILE_MACHINE_I386:       printInf(("    IMAGE_FILE_MACHINE_I386\n")); break;
            case IMAGE_FILE_MACHINE_R3000:      printInf(("    IMAGE_FILE_MACHINE_R3000\n")); break;
            case IMAGE_FILE_MACHINE_R4000:      printInf(("    IMAGE_FILE_MACHINE_R4000\n")); break;
            case IMAGE_FILE_MACHINE_R10000:     printInf(("    IMAGE_FILE_MACHINE_R10000\n")); break;
            case IMAGE_FILE_MACHINE_ALPHA:      printInf(("    IMAGE_FILE_MACHINE_ALPHA\n")); break;
            case IMAGE_FILE_MACHINE_POWERPC:    printInf(("    IMAGE_FILE_MACHINE_POWERPC\n")); break;
            default:
                printInf(("    *unknown*\n"));
        }
        printInf(("NumberOfSections        %d\n",      pNtHdrs->FileHeader.NumberOfSections));
        printInf(("TimeDataStamp           0x%08x\n",  pNtHdrs->FileHeader.TimeDateStamp));
        printInf(("PointerToSymbolTable    0x%08x\n",  pNtHdrs->FileHeader.PointerToSymbolTable));
        printInf(("NumberOfSymbols         %d\n",      pNtHdrs->FileHeader.NumberOfSymbols));
        printInf(("SizeOfOptionalHeader    %d\n",      pNtHdrs->FileHeader.SizeOfOptionalHeader));
        printInf(("Characteristics         0x%04x\n",  pNtHdrs->FileHeader.Characteristics));
        w = pNtHdrs->FileHeader.Characteristics;
        if ((w & IMAGE_FILE_RELOCS_STRIPPED) == IMAGE_FILE_RELOCS_STRIPPED)
            printInf(("    IMAGE_FILE_RELOCS_STRIPPED\n"));
        if ((w & IMAGE_FILE_EXECUTABLE_IMAGE) == IMAGE_FILE_EXECUTABLE_IMAGE)
            printInf(("    IMAGE_FILE_EXECUTABLE_IMAGE\n"));
        if ((w & IMAGE_FILE_LINE_NUMS_STRIPPED) == IMAGE_FILE_LINE_NUMS_STRIPPED)
            printInf(("    IMAGE_FILE_LINE_NUMS_STRIPPED\n"));
        if ((w & IMAGE_FILE_LOCAL_SYMS_STRIPPED) == IMAGE_FILE_LOCAL_SYMS_STRIPPED)
            printInf(("    IMAGE_FILE_LOCAL_SYMS_STRIPPED\n"));
        if ((w & 0x0010) == 0x0010)
            printInf(("    IMAGE_FILE_AGGRESIVE_WS_TRIM\n"));
        if ((w & IMAGE_FILE_BYTES_REVERSED_LO) == IMAGE_FILE_BYTES_REVERSED_LO)
            printInf(("    IMAGE_FILE_BYTES_REVERSED_LO\n"));
        if ((w & IMAGE_FILE_32BIT_MACHINE) == IMAGE_FILE_32BIT_MACHINE)
            printInf(("    IMAGE_FILE_32BIT_MACHINE\n"));
        if ((w & IMAGE_FILE_DEBUG_STRIPPED) == IMAGE_FILE_DEBUG_STRIPPED)
            printInf(("    IMAGE_FILE_DEBUG_STRIPPED\n"));
        if ((w & 0x0400) == 0x0400)
            printInf(("    IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP\n"));
        if ((w & 0x0800) == 0x0800)
            printInf(("    IMAGE_FILE_NET_RUN_FROM_SWAP\n"));
        if ((w & IMAGE_FILE_SYSTEM) == IMAGE_FILE_SYSTEM)
            printInf(("    IMAGE_FILE_SYSTEM\n"));
        if ((w & IMAGE_FILE_DLL) == IMAGE_FILE_DLL)
            printInf(("    IMAGE_FILE_DLL\n"));
        if ((w & 0x4000) == 0x4000)
            printInf(("    IMAGE_FILE_UP_SYSTEM_ONLY\n"));
        if ((w & IMAGE_FILE_BYTES_REVERSED_HI) == IMAGE_FILE_BYTES_REVERSED_HI)
            printInf(("    IMAGE_FILE_BYTES_REVERSED_HI\n"));

        printInf(("\nPE-headers - Optional header\n"));
        printInf(("Magic                       0x%04x\n", pNtHdrs->OptionalHeader.Magic));
        printInf(("MajorLinkerVersion          %d\n",     pNtHdrs->OptionalHeader.MajorLinkerVersion));
        printInf(("MinorLinkerVersion          %d\n",     pNtHdrs->OptionalHeader.MajorLinkerVersion));
        printInf(("SizeOfCode                  0x%08x\n", pNtHdrs->OptionalHeader.SizeOfCode));
        printInf(("SizeOfInitializedData       0x%08x\n", pNtHdrs->OptionalHeader.SizeOfInitializedData));
        printInf(("SizeOfUninitializedData     0x%08x\n", pNtHdrs->OptionalHeader.SizeOfUninitializedData));
        printInf(("AddressOfEntryPoint         0x%08x\n", pNtHdrs->OptionalHeader.AddressOfEntryPoint));
        printInf(("BaseOfCode                  0x%08x\n", pNtHdrs->OptionalHeader.BaseOfCode));
        printInf(("BaseOfData                  0x%08x\n", pNtHdrs->OptionalHeader.BaseOfData));
        printInf(("ImageBase                   0x%08x\n", pNtHdrs->OptionalHeader.ImageBase));
        printInf(("SectionAlignment            0x%08x\n", pNtHdrs->OptionalHeader.SectionAlignment));
        printInf(("FileAlignment               0x%08x\n", pNtHdrs->OptionalHeader.FileAlignment));
        printInf(("MajorOperatingSystemVersion %d\n",     pNtHdrs->OptionalHeader.MajorOperatingSystemVersion));
        printInf(("MinorOperatingSystemVersion %d\n",     pNtHdrs->OptionalHeader.MinorOperatingSystemVersion));
        printInf(("MajorImageVersion           %d\n",     pNtHdrs->OptionalHeader.MajorImageVersion));
        printInf(("MinorImageVersion           %d\n",     pNtHdrs->OptionalHeader.MinorImageVersion));
        printInf(("MajorSubsystemVersion       %d\n",     pNtHdrs->OptionalHeader.MajorSubsystemVersion));
        printInf(("MinorSubsystemVersion       %d\n",     pNtHdrs->OptionalHeader.MinorSubsystemVersion));
        printInf(("Win32VersionValue           0x%08x\n", pNtHdrs->OptionalHeader.Reserved1));
        printInf(("SizeOfImage                 0x%08x\n", pNtHdrs->OptionalHeader.SizeOfImage));
        printInf(("SizeOfHeaders               0x%08x\n", pNtHdrs->OptionalHeader.SizeOfHeaders));
        printInf(("CheckSum                    0x%08x\n", pNtHdrs->OptionalHeader.CheckSum));
        printInf(("Subsystem                   0x%04x\n", pNtHdrs->OptionalHeader.Subsystem));

        switch(pNtHdrs->OptionalHeader.Subsystem)
        {
            case IMAGE_SUBSYSTEM_UNKNOWN:       printInf(("    IMAGE_SUBSYSTEM_UNKNOWN\n")); break;
            case IMAGE_SUBSYSTEM_NATIVE:        printInf(("    IMAGE_SUBSYSTEM_NATIVE\n")); break;
            case IMAGE_SUBSYSTEM_WINDOWS_GUI:   printInf(("    IMAGE_SUBSYSTEM_WINDOWS_GUI\n")); break;
            case IMAGE_SUBSYSTEM_WINDOWS_CUI:   printInf(("    IMAGE_SUBSYSTEM_WINDOWS_CUI\n")); break;
            case IMAGE_SUBSYSTEM_OS2_CUI:       printInf(("    IMAGE_SUBSYSTEM_OS2_CUI\n")); break;
            case IMAGE_SUBSYSTEM_POSIX_CUI:     printInf(("    IMAGE_SUBSYSTEM_POSIX_CUI\n")); break;
            case 8:                             printInf(("    IMAGE_SUBSYSTEM_NATIVE_WINDOWS\n")); break;
            case 9:                             printInf(("    IMAGE_SUBSYSTEM_WINDOWS_CE_GUI\n")); break;
            default:
                printInf(("    *unknown*"));
        }
        printInf(("DllCharacteristics          0x%04x\n", pNtHdrs->OptionalHeader.DllCharacteristics));
        printInf(("SizeOfStackReserve          0x%08x\n", pNtHdrs->OptionalHeader.SizeOfStackReserve));
        printInf(("SizeOfStackCommit           0x%08x\n", pNtHdrs->OptionalHeader.SizeOfStackCommit));
        printInf(("SizeOfHeapReserve           0x%08x\n", pNtHdrs->OptionalHeader.SizeOfHeapReserve));
        printInf(("SizeOfHeapCommit            0x%08x\n", pNtHdrs->OptionalHeader.SizeOfHeapCommit));
        printInf(("LoaderFlags                 0x%08x\n", pNtHdrs->OptionalHeader.LoaderFlags));
        printInf(("NumberOfRvaAndSizes         0x%08x\n", pNtHdrs->OptionalHeader.NumberOfRvaAndSizes));

        printInf(("\nPE-Headers - DataDirectory\n"));
        for (i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
        {
            char *pszName;

            switch (i)
            {
                case IMAGE_DIRECTORY_ENTRY_EXPORT:      pszName = "Export Directory (IMAGE_DIRECTORY_ENTRY_EXPORT)"; break;
                case IMAGE_DIRECTORY_ENTRY_IMPORT:      pszName = "Import Directory (IMAGE_DIRECTORY_ENTRY_IMPORT)"; break;
                case IMAGE_DIRECTORY_ENTRY_RESOURCE:    pszName = "Resource Directory (IMAGE_DIRECTORY_ENTRY_RESOURCE)"; break;
                case IMAGE_DIRECTORY_ENTRY_EXCEPTION:   pszName = "Exception Directory (IMAGE_DIRECTORY_ENTRY_EXCEPTION)"; break;
                case IMAGE_DIRECTORY_ENTRY_SECURITY:    pszName = "Security Directory (IMAGE_DIRECTORY_ENTRY_SECURITY)"; break;
                case IMAGE_DIRECTORY_ENTRY_BASERELOC:   pszName = "Base Relocation Table (IMAGE_DIRECTORY_ENTRY_BASERELOC)"; break;
                case IMAGE_DIRECTORY_ENTRY_DEBUG:       pszName = "Debug Directory (IMAGE_DIRECTORY_ENTRY_DEBUG)"; break;
                case IMAGE_DIRECTORY_ENTRY_COPYRIGHT:   pszName = "Description String (IMAGE_DIRECTORY_ENTRY_COPYRIGHT)"; break;
                case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:   pszName = "Machine Value (MIPS GP) (IMAGE_DIRECTORY_ENTRY_GLOBALPTR)"; break;
                case IMAGE_DIRECTORY_ENTRY_TLS:         pszName = "TLS Directory (IMAGE_DIRECTORY_ENTRY_TLS)"; break;
                case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG: pszName = "Load Configuration Directory (IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG)"; break;
                case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:pszName = "Bound Import Directory in headers (IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)"; break;
                case IMAGE_DIRECTORY_ENTRY_IAT:         pszName = "Import Address Table (IMAGE_DIRECTORY_ENTRY_IAT)"; break;
                default:
                    pszName = "unknown";
            }

            printInf(("%2d %s\n", i, pszName));
            printInf(("    Address    0x%08x\n", pNtHdrs->OptionalHeader.DataDirectory[i].VirtualAddress));
            printInf(("    Size       0x%08x\n", pNtHdrs->OptionalHeader.DataDirectory[i].Size));
        }
    }
}


/**
 * Static method which dumps a section header.
 * @param     pSection  Pointer to a section header.
 * @status    paritally implemented.
 * @author    knut st. osmundsen
 * @remark    Missing flags symbols!
 */
VOID Pe2Lx::dumpSectionHeader(PIMAGE_SECTION_HEADER pSection)
{
    if (pSection >= (PIMAGE_SECTION_HEADER)0x10000)
    {
        printInf(("\nSection Name:        %.8s\n", pSection->Name));
        printInf(("Raw data size:       0x%08x\n", pSection->SizeOfRawData));
        printInf(("Virtual Address:     0x%08x\n", pSection->VirtualAddress));
        printInf(("Virtual Size:        0x%08x\n", pSection->Misc.VirtualSize));
        printInf(("Pointer to raw data: 0x%08x\n", pSection->PointerToRawData));
        printInf(("Section flags:       0x%08x\n", pSection->Characteristics));
        /* FIXME! display flags! */
    }
    else
        printInf(("dumpSectionHeader - invalid pointer specified! pSection=%#8x\n", pSection));
}


/**
 * Constructor.
 * @param     hFile      Filehandle.
 * @param     cObjects   Count of objects in the object array pointed to by paObjects.
 * @param     paObjects  Read-only pointer to object array.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
BufferedRVARead::BufferedRVARead(SFN hFile, ULONG cObjects, PCLXOBJECT paObjects)
    : hFile(hFile), cObjects(cObjects), paObjects(paObjects), ulRVA(~0UL)
{
}

#if 0 /* made inline! */
/**
 * Reads a chunk of data at the spcified RVA.
 * @returns   NO_ERROR on success.
 *            ERROR_INVALID_PARAMETER
 *            <Whatever rc ReadAt returns>
 * @param     ulRVA     RVA to read from. Within the filesize.
 * @param     pvBuffer  Pointer to output buffer. pvBuffer > 64KB
 * @param     cbBuffer  Number of bytes to read. 0 < cbBuffer > 256MB
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG BufferedRVARead::readAtRVA(ULONG ulRVA, PVOID pvBuffer, ULONG cbBuffer)
{
    /*
     * five cases:
     *  1) entire area is within the buffer.
     *  2) start of area is within the buffer.
     *  3) end of area is within the buffer.
     *  4) the area is larger than the buffer, covering it.
     *  5) the area is outside the buffer.
     *
     * these are optimal: 1, 2, and 5.
     * The request is allways process from start to end. This will make case 3 and 4 less effecient.
     */
    #ifdef DEBUG
    if (ulRVA == ~0UL || (ULONG)pvBuffer < 0x10000UL || cbBuffer == 0UL || cbBuffer >= 0x10000000UL)
        return ERROR_INVALID_PARAMETER;
    #endif

    do
    {
        if (ulRVA >= this->ulRVA && ulRVA < this->ulRVA + sizeof(achBuffer))
        {   /* in buffer */
            register ULONG cbRead = sizeof(achBuffer) - (ulRVA - this->ulRVA);
            cbRead = min(cbRead, cbBuffer);
            memcpy(pvBuffer, &achBuffer[ulRVA - this->ulRVA], (size_t)cbRead);
            if (cbBuffer == cbRead)
                return NO_ERROR;
            cbBuffer -= cbRead;
            pvBuffer = (PVOID)((ULONG)pvBuffer + cbRead);
            ulRVA += cbRead;
        }
        else
        {   /* not in buffer, then read it into the buffer! */
            APIRET rc = readToBuffer(ulRVA);
            if (rc != NO_ERROR)
                return rc;
        }
    } while (cbBuffer != 0UL);

    return NO_ERROR;
}
#endif


/**
 * Reads a zero string into a heap block and returns it to the caller (thru ppsz).
 * @returns   NO_ERROR
 *            ERROR_INVALID_PARAMETER
 *            ERROR_NOT_ENOUGH_MEMORY
 *            Return code from ReadAt.
 * @param     ulRVA  RVA which the string is to be read from.
 * @param     ppsz   Pointer to a stringpointer. Output: This will hold pointer to a heapblock with the string.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG BufferedRVARead::dupString(ULONG ulRVA, PSZ *ppsz)
{
    #ifdef DEBUG
    if (ulRVA == ~0UL || ppsz == NULL)
        return ERROR_INVALID_PARAMETER;
    #endif

    if (ppsz < (PSZ*)0x10000UL)
    {
        printErr(("Call to dupString with a unconverted stack pointer!.\n"));
        ppsz = (PSZ*)SSToDS(ppsz);
    }

    *ppsz = NULL;
    ULONG cchAllocated = 0UL;
    ULONG offCurrent = 0UL;
    while (TRUE)
    {
        if (ulRVA >= this->ulRVA && ulRVA < this->ulRVA + sizeof(achBuffer))
        {   /* in buffer */
            ULONG cchAlloc;
            PCHAR pch = (PCHAR)memchr(&achBuffer[ulRVA - this->ulRVA], '\0',
                                      sizeof(achBuffer) - (size_t)(ulRVA - this->ulRVA));

            /* amout of memory to allocate */
            if (pch != NULL)
                cchAlloc = (ULONG)pch - (ULONG)&achBuffer[ulRVA - this->ulRVA] + 1;
            else
                cchAlloc = sizeof(achBuffer) - (ulRVA - this->ulRVA) + 42; /* 42 - think of a number... */

            /* allocate (more) memory */
            if (*ppsz == NULL)
                *ppsz = (PSZ)malloc((size_t)cchAlloc);
            else
            {
                PVOID pv = realloc(*ppsz, (size_t)(cchAlloc + cchAllocated));
                if (pv == NULL)
                {
                    free(*ppsz);
                    *ppsz = NULL;
                    return ERROR_NOT_ENOUGH_MEMORY;
                }
                *ppsz = (PSZ)pv;
            }
            cchAllocated += cchAlloc;

            /* copy string data */
            if (pch != NULL)
            {   /* final part of the string. */
                strcpy(&(*ppsz)[offCurrent], &achBuffer[ulRVA - this->ulRVA]);
                break;
            }
            /* more to come */
            memcpy(&(*ppsz)[offCurrent], &achBuffer[ulRVA - this->ulRVA], sizeof(achBuffer) - (size_t)(ulRVA - this->ulRVA));
            offCurrent += sizeof(achBuffer) - (ulRVA - this->ulRVA);
            ulRVA = ALIGN(ulRVA+1, sizeof(achBuffer));
        }
        else
        {   /* not in buffer, then read it into the buffer! */
            APIRET rc = readToBuffer(ulRVA);
            if (rc != NO_ERROR)
                return rc;
        }
    }

    return NO_ERROR;
}


/**
 * Assignment operator.
 * @returns   Reference to this BufferedRVARead object.
 * @param     SrcObj  The object on the right side of the operator.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BufferedRVARead & BufferedRVARead::operator =(BufferedRVARead &SrcObj)
{
    hFile      = SrcObj.hFile;
    cObjects   = SrcObj.cObjects;
    paObjects  = SrcObj.paObjects;
    ulRVA      = SrcObj.ulRVA;
    if (ulRVA != ~0UL)
        memcpy(&achBuffer[0], &SrcObj.achBuffer[0], sizeof(achBuffer));
    return *this;
}


/**
 * Reads data to the buffer for a given RVA.
 * @returns   NO_ERROR
 *            ERROR_INVALID_PARAMETER
 *            Return code from ReadAt.
 * @param     ulRVA  RVA to read at/near.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
ULONG BufferedRVARead::readToBuffer(ULONG ulRVA)
{
    ULONG ulRVARead;

    /* where to read? */
    if (ulRVA != this->ulRVA + sizeof(achBuffer) || this->ulRVA == ~0UL)
        ulRVARead = this->ulRVA = ulRVA & ~(PAGESIZE-1UL); /* align on page boundrary - ASSUMES: buffersize >= PAGESIZE! */
    else
        ulRVARead = this->ulRVA += sizeof(achBuffer); /* immediately after current buffer */

    ULONG cbLeftToRead = sizeof(achBuffer);
    ULONG iObj = 0;
    while (cbLeftToRead != 0UL)
    {
        while (iObj < cObjects
               && ulRVARead >= (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                                : paObjects[iObj].ulRVA + ALIGN(paObjects[iObj].cbVirtual, PAGESIZE))
              )
            iObj++;
        if (iObj >= cObjects)
        {
            this->ulRVA = ~0UL;
            return ERROR_INVALID_PARAMETER;
        }

        /* ulRVARead points at physical or virtual data? */
        if (ulRVARead < paObjects[iObj].ulRVA + paObjects[iObj].cbPhysical)
        {   /* physical data - read from file */
            APIRET rc;
            ULONG  cbToRead = min(paObjects[iObj].ulRVA + paObjects[iObj].cbPhysical - ulRVARead, cbLeftToRead);
            rc = ReadAt(hFile,
                        ulRVARead - paObjects[iObj].ulRVA + paObjects[iObj].offPEFile,
                        &achBuffer[sizeof(achBuffer)-cbLeftToRead],
                        cbToRead
                        );
            if (rc != NO_ERROR)
            {
                this->ulRVA = ~0UL;
                return rc;
            }
            cbLeftToRead -= cbToRead;
            ulRVARead += cbToRead;
        }
        else
        {   /* virtual data - memset(,0,) */
            ULONG cbToSet = (iObj + 1 < cObjects ? paObjects[iObj+1].ulRVA
                              : paObjects[iObj].ulRVA + ALIGN(paObjects[iObj].cbVirtual, PAGESIZE))
                            - ulRVARead; /* calcs size of virtual data left in this object */
            cbToSet = min(cbToSet, cbLeftToRead);

            memset(&achBuffer[sizeof(achBuffer)-cbLeftToRead], 0, (size_t)cbToSet);
            cbLeftToRead -= cbToSet;
            ulRVARead += cbToSet;
        }
    }

    return NO_ERROR;
}


/* end of file */
