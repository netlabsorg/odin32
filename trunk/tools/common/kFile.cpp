/* $Id: kFile.cpp,v 1.5 2000-10-02 04:01:39 bird Exp $
 *
 * kFile - Simple (for the time being) file class.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "kFile.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
kFile kFile::StdIn((HFILE)0, TRUE);
kFile kFile::StdOut((HFILE)1, FALSE);
kFile kFile::StdErr((HFILE)2, FALSE);


/**
 * Updates the internal filestatus struct.
 * @returns     Success indicator.
 *              On success filestatus is refreshed.
 * @remark
 */
BOOL    kFile::refreshFileStatus()
{
    if (fStdDev)
        return fStatusClean = TRUE;

    if (!fStatusClean)
    {
        rc = DosQueryFileInfo(hFile, FIL_QUERYEASIZE, &filestatus, sizeof(filestatus));
        fStatusClean = (rc == NO_ERROR);
        if (!fStatusClean && fThrowErrors)
            throw ((int)rc);
    }
    else
        rc = NO_ERROR;

    return fStatusClean;
}


/**
 * Changes the real file position to match the virtual file position.
 * @returns     Success indicator.
 */
BOOL    kFile::position()
{
    /*
     * If virtual file offset is different from the real,
     * issue a set file pointer call.
     */
    if (offVirtual != offReal)
    {
        ULONG   off;
        rc = DosSetFilePtr(hFile, offVirtual, FILE_BEGIN, &off);
        if (rc != NO_ERROR || off != offVirtual)
        {
            if (fThrowErrors)
                throw ((int)rc);
            return FALSE;
        }
        offReal = offVirtual;
    }

    return TRUE;
}

/**
 * Creates a kFile object for a file that is opened allready.
 *  Intended use for the three standard handles only.
 *
 * @returns     <object> with state updated.
 * @param       pszFilename     Filename.
 * @param       fReadOnly       TRUE:  Open the file readonly.
 *                              FALSE: Open the file readwrite appending
 *                                     existing files.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
kFile::kFile(HFILE hFile, BOOL fReadOnly)
:   fReadOnly(fReadOnly),
    fStatusClean(FALSE),
    fThrowErrors(FALSE),
    offVirtual(0),
    offReal(0),
    pszFilename(NULL),
    hFile(hFile),
    fStdDev(TRUE)
{
    if (!refreshFileStatus())
        throw ((int)rc);
    this->pszFilename = strdup("");
}


/**
 * Opens a file for binary reading or readwrite.
 * @returns     <object> with state updated.
 *              Throws OS/2 error on error.
 * @param       pszFilename     Filename.
 * @param       fReadOnly       TRUE:  Open the file readonly. (default)
 *                              FALSE: Open the file readwrite appending
 *                                     existing files.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
kFile::kFile(const char *pszFilename, BOOL fReadOnly/*=TRUE*/)
:   fReadOnly(fReadOnly),
    fStatusClean(FALSE),
    fThrowErrors(FALSE),
    offVirtual(0),
    offReal(0),
    pszFilename(NULL),
    fStdDev(FALSE)
{
    ULONG   fulOpenFlags;
    ULONG   fulOpenMode;
    ULONG   ulAction;

    /*
     * Determin open flags according to fReadOnly.
     */
    if (fReadOnly)
    {
        fulOpenFlags = OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        fulOpenMode  = OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY;
    }
    else
    {
        fulOpenFlags = OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        fulOpenMode  = OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READWRITE;
    }

    rc = DosOpen((PCSZ)pszFilename, &hFile, &ulAction, 0, FILE_NORMAL,
                 fulOpenFlags, fulOpenMode, NULL);
    if (rc != NO_ERROR)
        throw ((int)rc);

    if (!refreshFileStatus())
        throw ((int)rc);

    char szFullName[CCHMAXPATH];
    if (DosQueryPathInfo(pszFilename, FIL_QUERYFULLNAME, szFullName, sizeof(szFullName)))
        strcpy(szFullName, pszFilename);
    this->pszFilename = strdup(szFullName);
    if (this->pszFilename == NULL)
        throw (ERROR_NOT_ENOUGH_MEMORY);
}


/**
 * Closes the file.
 */
kFile::~kFile()
{
    DosClose(hFile);
}


/**
 * Reads <cbBuffer> bytes from the current file posistion into the buffer.
 * @returns     success indicator. (TRUE/FALSE)
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to read.
 */
BOOL            kFile::read(void *pvBuffer, long cbBuffer)
{
    if (position())
    {
        ULONG   cbRead;
        rc = DosRead(hFile, pvBuffer, cbBuffer, &cbRead);
        if (rc == NO_ERROR)
        {
            offVirtual = offReal += cbRead;
            return TRUE;
        }
    }

    if (fThrowErrors)
        throw ((int)rc);
    return FALSE;
}


/**
 * Reads <cbBuffer> bytes at file offset <off>.
 * @returns     success indicator. (TRUE/FALSE)
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to read.
 * @param       off         Absolute file offset.
 */
BOOL            kFile::readAt(void *pvBuffer, long cbBuffer, long off)
{
    return set(off) && read(pvBuffer, cbBuffer);
}


/**
 * Reads the entire file into a single memory block.
 * (The memory block has a '\0' at the end just in case you
 *  are using it as a long string.)
 * @returns     Pointer to file in memory.
 */
void *          kFile::readFile() throw(int)
{
    void *pv;

    /* allocate memory for the file */
    pv = calloc((size_t)this->getSize() + 1, 1);
    if (pv == NULL)
    {
        if (fThrowErrors)
            throw(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    /* go the start of the file and read it. */
    if (start() && read(pv, this->getSize()))
        return pv; // successfull exit!

    /* we failed, cleanup and return NULL */
    free(pv);
    return NULL;
}


/**
 * Writes <cbBuffer> bytes to the file at the current file position.
 * @returns     success indicator. (TRUE/FALSE)
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to write.
 */
BOOL            kFile::write(void *pvBuffer, long cbBuffer)
{
    if (fReadOnly)
        rc = ERROR_ACCESS_DENIED;
    else
    {
        if (position())
        {
            ULONG   cbWrote;

            rc = DosWrite(hFile, pvBuffer, cbBuffer, &cbWrote);
            if (rc == NO_ERROR)
            {
                fStatusClean = FALSE;
                offVirtual = offReal += cbWrote;
                return TRUE;
            }
        }
    }

    if (fThrowErrors)
        throw ((int)rc);
    return FALSE;
}


/**
 * Write <cbBuffer> bytes at file offset <off> from <pvBuffer>.
 * @returns     success indicator. (TRUE/FALSE)
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to write.
 * @param       off         Absolute file offset.
 */
BOOL            kFile::writeAt(void *pvBuffer, long cbBuffer, long off)
{
    return set(off) && write(pvBuffer, cbBuffer);
}


/**
 * printf - formatted write.
 *
 * Lonely '\n's are prefixed with a '\r' to make output conform with
 * PC line ending.
 *
 * @returns     Number of bytes written.
 * @param       pszFormat   Format string.
 * @param       ...         Ellipcis.
 * @remark      Currently limited to 64KB of result data.
 */
int             kFile::printf(const char *pszFormat, ...) throw (int)
{
    long        offStart = getPos();
    va_list     arg;

    /* !QUICK AND DIRTY! */
    char *  psz, * pszEnd;
    char *  pszBuffer = (char*)malloc(1024*64); //64KB should normally be enough...

    va_start(arg, pszFormat);
    pszEnd = vsprintf(pszBuffer, pszFormat, arg) + pszBuffer;
    va_end(arg);

    psz = pszEnd;
    while (psz > pszBuffer)
    {
        if (*psz-- == '\n' && *psz != '\r')
        {
            memmove(psz+2, psz+1, pszEnd - psz + 1);
            psz[1] = '\r';
            pszEnd++;
        }
    }

    write(pszBuffer, pszEnd - pszBuffer);
    free(pszBuffer);

    return (int)(getPos() - offStart);
}


/**
 * Sets the filesize.
 * @returns     Success indicator.
 * @param       cbFile      New filesize.
 *                          Defaults to 0xffffffff, which results in
 *                          cutting the file at the current position.
 */
BOOL            kFile::setSize(unsigned long cbFile/*= ~0UL*/)
{
    if (cbFile == ~0UL)
        cbFile = offVirtual;
    rc = DosSetFileSize(hFile, cbFile);
    if (rc != NO_ERROR && fThrowErrors)
        throw ((int)rc);

    return rc == NO_ERROR;
}



/**
 * Seek relative to the current position.
 * @returns     Success indicator.
 * @param       off     Relative reposition.
 */
BOOL            kFile::move(long off)
{
    if ((off + offVirtual) & 0x80000000UL) /* above 2GB or negative */
        rc = ERROR_NEGATIVE_SEEK;
    else
    {
        if (off + offVirtual > filestatus.cbFile && fReadOnly) /* can't expand readonly file. */
            rc = ERROR_HANDLE_EOF;
        else
        {
            offVirtual += off;
            return TRUE;
        }
    }

    if (fThrowErrors)
        throw ((int)rc);
    return FALSE;
}


/**
 * Seek to an absolute position in the file (off).
 * @returns     Success indicator.
 * @param       off     New file position.
 */
BOOL            kFile::set(long off)
{
    if (off < 0)
        rc = ERROR_NEGATIVE_SEEK;
    else
    {
        if ((unsigned long)off > filestatus.cbFile && fReadOnly)
            rc = ERROR_HANDLE_EOF;
        else
        {
            offVirtual = off;
            rc = NO_ERROR;
            return TRUE;
        }
    }
    if (fThrowErrors)
        throw ((int)rc);
    return FALSE;
}


/**
 * Seek to the end of the file.
 * @returns     Success indicator. TRUE / FALSE.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
BOOL            kFile::end()
{
    if (!refreshFileStatus())
        return FALSE;
    offVirtual = filestatus.cbFile; //?? or +1
    rc = NO_ERROR;
    return TRUE;
}


/**
 * Seek to the start of the file.
 * @returns     TRUE.
 * @remark      Will never throw errors.
 */
BOOL            kFile::start()
{
    offVirtual = 0;
    rc = NO_ERROR;
    return TRUE;
}


/**
 * Get the size of the file.
 * @returns     Returns file size on success.
 *              -1 on error.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
long            kFile::getSize()
{
    if (!refreshFileStatus())
        return -1;

    return filestatus.cbFile;
}


/**
 * Get current position.
 * @returns     The current file position.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
long            kFile::getPos() const
{
    return offVirtual;
}


/**
 * Checks if we have reached the file end.
 * @returns     TRUE if end-of-file is reached.
 *              FALSE is not end-of-file.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
BOOL            kFile::isEOF()
{
    #if 0
    throw(ERROR_NOT_SUPPORTED); //this method don't currently work! Need to use flag!
    #else
    if (!refreshFileStatus())
        return (BOOL)-1;

    return filestatus.cbFile >= offReal; //???
    #endif
}


/**
 * Set error behaviour to fail by throwing the OS/2 return code when an
 * error occures.
 * @returns     TRUE;
 * @remark      Will never throw errors.
 */
BOOL            kFile::setThrowOnErrors()
{
    fThrowErrors = TRUE;
    rc = NO_ERROR;
    return TRUE;
}


/**
 * Set error behaviour to fail by return FALSE when an error has occures.
 * @returns     TRUE;
 * @remark      Will never throw errors.
 */
BOOL            kFile::setFailOnErrors()
{
    fThrowErrors = FALSE;
    rc = NO_ERROR;
    return TRUE;
}


/**
 * Gets the last error code.
 * @returns     OS/2 error code for the last operation.
 * @remark      Will never throw errors.
 */
int            kFile::getLastError() const
{
    return rc;
}

