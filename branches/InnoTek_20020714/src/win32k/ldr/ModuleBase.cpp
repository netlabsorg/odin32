/* $Id: ModuleBase.cpp,v 1.7 2001-02-10 11:11:44 bird Exp $
 *
 * ModuleBase - Implementetation.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS                  /* DOS Error codes. */
#ifdef RING0
    #define INCL_NOAPI                  /* RING0: No apis. */
#else /*RING3*/
    #define INCL_DOSFILEMGR             /* RING3: DOS File api. */
#endif
#define INCL_OS2KRNL_LDR


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "malloc.h"                     /* Win32k malloc. Not C library! */

#include <string.h>                     /* C library string.h. */
#include <stdarg.h>                     /* C library stdarg.h. */

#include "vprintf.h"                    /* win32k printf and vprintf. Not C library! */
#include "dev32.h"                      /* 32-Bit part of the device driver. (SSToDS) */
#include "OS2Krnl.h"                    /* kernel structs.  (SFN) */
#include "modulebase.h"                 /* ModuleBase class definitions, ++. */


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/**
 * Current output message detail level; default: ModuleBase::Info, -W3.
 */
ULONG ModuleBase::ulInfoLevel = ModuleBase::Info;



/**
 * Constructor - Initializes the object to init state and sets the filehandle.
 * @param     hFile   System filenumber of the exectuable file.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ModuleBase::ModuleBase(SFN hFile) : hFile(hFile), pszFilename(NULL), pszModuleName(NULL)
{
    #ifdef DEBUG
    fInitTime = TRUE;
    #endif
}


/**
 * Destructor frees all memory allocated by this object.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ModuleBase::~ModuleBase()
{
    if (pszFilename != NULL)
        free(pszFilename);
    if (pszModuleName != NULL)
        free(pszModuleName);
    #ifdef DEBUG
        fInitTime = (BOOL)-1;
        pszFilename = (PSZ)0xFFFFBEEF;
        pszModuleName = (PSZ)0xFFFFBEEF;
    #endif
}



/**
 * This functions sets the filename and modulename for the ModuleBase object.
 *
 * The children of this class should override this method and upon return of it
 * the object should be a fully initialized virtual LX file. The object will
 * then not be in init mode any longer (fInitTime is FALSE).
 *
 * @returns   NO_ERROR on success.
 *            ERROR_NOT_ENOUGH_MEMORY
 *            ERROR_INTERNAL_PROCESSING_ERROR
 * @param     pszFilename  Module filename.
 * @precond   Called in init-mode.
 * @sketch    DEBUG: Verify initmode.
 *            Duplicate filename.
 *            Derive the modulename from the filename.
 *            return successfully.
 * @status    Completely implemented; tested.
 * @author    knut st. osmundsen
 * @remark    The object is still in initmode when returing.
 */
ULONG ModuleBase::init(PCSZ pszFilename)
{
    PCSZ psz;
    int   i;

    #ifdef DEBUG
        /* check that were called in initmode. */
        if (!fInitTime)
            return ERROR_INTERNAL_PROCESSING_ERROR;
    #endif

    /* Duplicate filename. */
    this->pszFilename = (char*)malloc(strlen(pszFilename) + 1);
    if (this->pszFilename == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    strcpy(this->pszFilename, pszFilename);

    /* Derive the modulename. */
    if ((psz = strrchr(pszFilename, '\\') + 1) == (PCHAR)1)
            if ((psz = strrchr(pszFilename, '/') + 1) == (PCHAR)1)
                psz = pszFilename;
    pszModuleName = strchr(psz, '.');
    i = pszModuleName != NULL ? pszModuleName - psz : strlen(psz);
    pszModuleName = (PSZ)malloc(i + 1);
    if (pszModuleName == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    strncpy(pszModuleName, psz, i);
    pszModuleName[i] = '\0';

    /* return successfully */
    return NO_ERROR;
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
 * @remark     Stub.
 */
ULONG  ModuleBase::applyFixups(PMTE pMTE, ULONG iObject, ULONG iPageTable, PVOID pvPage,
                               ULONG ulPageAddress, PVOID pvPTDA)
{
    NOREF(pMTE);
    NOREF(iObject);
    NOREF(iPageTable);
    NOREF(pvPage);
    NOREF(ulPageAddress);
    NOREF(pvPTDA);
    return NO_ERROR;
}


/**
 * openPath - opens file eventually searching loader specific paths.
 *
 * This base implementation simply calls ldrOpenPath.
 * This method is only called for DLLs. DosLoadModule and Imports.
 *
 * @returns   OS2 return code.
 *            pLdrLv->lv_sfn  is set to filename handle.
 * @param     pachFilename  Pointer to filename. Not zero terminated!
 * @param     cchFilename   Filename length.
 * @param     pLdrLv        Loader local variables? (Struct from KERNEL.SDF)
 * @param     pful          Pointer to flags which are passed on to ldrOpen.
 * @param     lLibPath      New parameter in build 14053(/8266?)
 *                          ldrGetMte calls with 1
 *                          ldrOpenNewExe calls with 3
 *                          This is compared to the initial libpath index.
 *                              The libpath index is:
 *                                  BEGINLIBPATH    1
 *                                  LIBPATH         2
 *                                  ENDLIBPATH      3
 *                              The initial libpath index is either 1 or 2.
 *                          Currently we'll ignore it. (I don't know why ldrGetMte calls ldrOpenPath...)
 * @sketch
 * This is roughly what the original ldrOpenPath does:
 *      Save pTCBCur->TCBFailErr.
 *      if !CLASS_GLOBAL or miniifs then
 *          ldrOpen(pachFilename)
 *      else
 *          if beglibpath != NULL then path = 1 else path = 2
 *          if (lLibPath < path)
 *              return ERROR_FILE_NOT_FOUND; (2)
 *          Allocate buffer.
 *          loop until no more libpath elements
 *              get next libpath element and add it to the modname.
 *              try open the modname
 *              if successfull then break the loop.
 *          endloop
 *          Free buffer.
 *      endif
 *      Restore pTCBCur->TCBFailErr.
 */
ULONG  ModuleBase::openPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *pLdrLv, PULONG pful, ULONG lLibPath) /* (ldrOpenPath) */
{
    #ifdef RING0
    printf("ModuleBase::openPath:\n");
    return ldrOpenPath(pachFilename, cchFilename, pLdrLv, pful, lLibPath);
    #else
    NOREF(pachFilename);
    NOREF(cchFilename);
    NOREF(pLdrLv);
    NOREF(pful);
    return ERROR_NOT_SUPPORTED;
    #endif
}


#ifndef RING0

/**
 * Optional method objects when in Ring-3: Write the virtual LX file to disk.
 *
 * A child my override this method, as this is simply a dummy stub.
 *
 * @returns   ERROR_NOT_SUPPORTED.
 *            Children: OS/2 styled return codes with the errorcodes specified in ModuleBase.h.
 * @param     pszFilename  Name of output file. This file should be overwritten if exists and created
 *                         if it don't exists.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG  ModuleBase::writeFile(PCSZ pszFilename)
{
    pszFilename = pszFilename;
    return ERROR_NOT_SUPPORTED;
}

#endif


/**
 * Compare a given filename/modulename with the name of this module.
 * @returns   TRUE:  Matching.
 *            FALSE: Non-matching.
 * @param     pszFilename  Filename/modulename to match with this module.
 * @sketch    IF filename without path THEN
 *            BEGIN
 *                IF no extention THEN
 *                    compare directly with modulename
 *                ELSE
 *                    compare input filename with the object filename without path.
 *            END
 *            ELSE
 *                compare input filename with the object filename
 *            return TRUE if equal : FALSE if not.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BOOL  ModuleBase::queryIsModuleName(PCSZ pszFilename)
{
    #ifdef DEBUG
    if (!fInitTime)
    {
        printIPE(("queryIsModuleName should not be called during init!\n"));
        return FALSE;
    }
    #endif
    if (strchr(pszFilename, '\\') == NULL && strchr(pszFilename,'/') == NULL)
    {   /* use module name - no extention */
        if (strchr(pszFilename, '.') == NULL)
            return stricmp(pszFilename, pszModuleName) == 0;
        else
        {   /* extention */
            PCSZ psz = strchr(this->pszFilename, '\\');
            if ((psz = strchr(this->pszFilename, '/')) == NULL)
                 psz = this->pszFilename;
            else
                psz++; /* skip '\\' or '/' */
            return stricmp(pszFilename, psz) == 0;
        }
    }

    /* TODO: relative name vs fullname. */
    return stricmp(pszFilename, this->pszFilename) == 0;
}


/**
 * Gets the fullpath filename.
 * @returns     Const ("Readonly") pointer to the filename.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
PCSZ ModuleBase::getFilename()
{
    return pszFilename;
}


/**
 * Gets the modulename.
 * @returns     Const ("Readonly") pointer to the module name.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      Modulename is filename without path and extention.
 */
PCSZ ModuleBase::getModuleName()
{
    return pszModuleName;
}


/**
 * Output function for Modules.
 * @param     pszFormat    Pointer to format string.
 * @status    completely implemented; tested.
 * @author    knut st. osmundsen
 */
VOID ModuleBase::printf(PCSZ pszFormat, ...)
{
    va_list arguments;

    if (ulInfoLevel <= ModuleBase::ulInfoLevel)
    {
        va_start(arguments, pszFormat);
        vprintf2(pszFormat, arguments);
        va_end(arguments);
    }
}


#ifndef RING0
/**
 *
 * @returns   OS/2 return code. (NO_ERROR on success...)
 * @param     hFile     Handle to file. (RING0: Handle to SystemFileNumber (SFN).)
 * @param     ulOffset  Offset in the file to start reading at.
 * @param     pvBuffer  Pointer to output buffer.
 * @param     cbToRead  Count of bytes to read.
 * @sketch    Set Change filepointer to ulOffset.
 *            Read cbToRead into pvBufer.
 * @status    completely tested.
 * @author    knut st. osmundsen
 * @remark
 */
APIRET ReadAt(SFN hFile, ULONG ulOffset, PVOID pvBuffer, ULONG cbToRead)
{
    ULONG cbRead, ulMoved;
    APIRET rc;

    rc = DosSetFilePtr(hFile, ulOffset, FILE_BEGIN, &ulMoved);
    if (rc == NO_ERROR)
        rc = DosRead(hFile, pvBuffer, cbToRead, &cbRead);
    else
        printErr(("DosSetFilePtr(hfile, %#8x(%d),..) failed with rc = %d.\n",
                  ulOffset, ulOffset, rc));

    return rc;
}
#endif

