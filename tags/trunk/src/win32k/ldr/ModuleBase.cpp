/* $Id: ModuleBase.cpp,v 1.1 1999-11-10 01:45:35 bird Exp $
 *
 * ModuleBase - Implementetation.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS                      /* DOS Error codes. */
#ifdef RING0
    #define INCL_NOAPI                      /* RING0: No apis. */
#else /*RING3*/
    #define INCL_DOSFILEMGR                 /* RING3: DOS File api. */
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                            /* OS/2 header file. */

#include "malloc.h"                         /* win32k malloc. Not C library! */

#include <string.h>                         /* C library string.h. */
#include <stdarg.h>                         /* C library stdarg.h. */

#include "vprintf.h"                        /* win32k printf and vprintf. Not C library! */
#include "dev32.h"                          /* 32-Bit part of the device driver. (SSToDS) */
#include "OS2Krnl.h"                        /* kernel structs.  (SFN) */
#include "modulebase.h"                     /* ModuleBase class definitions, ++. */


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



