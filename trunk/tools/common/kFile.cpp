/* $Id: kFile.cpp,v 1.9 2001-10-17 14:21:10 bird Exp $
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
#include <stdlib.h>

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
    else
        rc = NO_ERROR;

    return TRUE;
}


/**
 * Reads data from the file into the buffer from the given file offset.
 * @returns Success indicator.
 * @param   offFile     File offset to read from.
 *                      (If readonly and beyond end of file the offset used
 *                       may be addjusted.)
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL kFile::bufferRead(ULONG offFile) throw (int)
{
    ULONG   cbRead;

    /* refresh file status (cbFile) */
    if (!refreshFileStatus())
        return FALSE;

    /* check that the request is valid */
    if (offFile > filestatus.cbFile)
        return FALSE;

    /* commit dirty buffer */
    if (fBufferDirty)
    {
        if (!bufferCommit())
            return TRUE;

        /* refresh file status (cbFile) */
        if (!refreshFileStatus())
            return FALSE;
    }

    /* If readonly file optimize end of file */
    if (fReadOnly && cbBuffer + offFile > filestatus.cbFile)
        offFile = filestatus.cbFile > cbBuffer ? filestatus.cbFile - cbBuffer : 0UL;

    /* need to change file ptr? */
    if (offFile != offReal)
    {
        ULONG ul;
        rc = DosSetFilePtr(hFile, offFile, FILE_BEGIN, &ul);
        if (rc != NO_ERROR)
        {
            if (fThrowErrors)
                throw ((int)rc);
            return FALSE;
        }
        offReal = offFile;
    }

    /* read from the file */
    cbRead = min(filestatus.cbFile - offFile, cbBuffer);
    rc = DosRead(hFile, pachBuffer, cbRead, &cbRead);
    if (rc == NO_ERROR)
    {
        cbBufferValid   = cbRead;
        offBuffer       = offFile;
        offReal         = offFile + cbRead;
        fBufferDirty    = FALSE;
    }
    else
    {
        cbBufferValid   = 0;
        offBuffer       = ~0UL;
        fBufferDirty    = FALSE;
        if (fThrowErrors)
            throw ((int)rc);
        return FALSE;
    }

    return TRUE;
}


/**
 * Commits the data in the buffer.
 * @returns Success indicator.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL kFile::bufferCommit(void) throw (int)
{
    ULONG   cbWrote;
    ULONG   ul;

    /* don't commit clean buffers! */
    if (!fBufferDirty)
        return TRUE;

    /* need to change file ptr? */
    if (offBuffer != offReal)
    {
        rc = DosSetFilePtr(hFile, offBuffer, FILE_BEGIN, &ul);
        if (rc != NO_ERROR)
        {
            if (fThrowErrors)
                throw ((int)rc);
            return FALSE;
        }
        offReal = offBuffer;
    }

    /* write to the file */
    rc = DosWrite(hFile, pachBuffer, cbBufferValid, &cbWrote);
    fStatusClean = FALSE;
    if (rc == NO_ERROR)
    {
        fBufferDirty = FALSE;
        offReal += cbWrote;
    }
    else
    {
        DosSetFilePtr(hFile, offReal, FILE_BEGIN, &ul);
        if (fThrowErrors)
            throw ((int)rc);
        return FALSE;
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
    fStdDev(TRUE),
    pachBuffer(NULL),
    cbBufferValid(0UL),
    offBuffer(~0UL),
    fBufferDirty(FALSE)
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
    fStdDev(FALSE),
    pachBuffer(NULL),
    cbBufferValid(0UL),
    offBuffer(~0UL),
    fBufferDirty(FALSE)
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

    /* Buffering */
    cbBuffer   = (fReadOnly && filestatus.cbFile < 32768) ? filestatus.cbFile : 8192;
    pachBuffer = new char[cbBuffer];
    if (pachBuffer == NULL)
        throw (ERROR_NOT_ENOUGH_MEMORY);
    if (fReadOnly && filestatus.cbFile < 32768)
    {
        if (!bufferRead(0))
            throw ((int)rc);
    }
}


/**
 * Closes the file.
 */
kFile::~kFile()
{
    if (fBufferDirty)
        bufferCommit();
    if (pachBuffer)
        delete pachBuffer;
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
    ULONG   cbRead;

    /* Validate parameters */
    if (cbBuffer == 0)
        return TRUE;
    if (cbBuffer < 0)
    {
        rc = ERROR_INVALID_PARAMETER;
        if (fThrowErrors)
            throw ((int)rc);
        return FALSE;
    }

    /* refresh file status (cbFile) */
    if (!refreshFileStatus())
        return FALSE;

    /* check if valid request */
    if (    offVirtual > filestatus.cbFile
        ||  offVirtual + cbBuffer > filestatus.cbFile
        )
    {   /* invalid request */
        rc = ERROR_NO_DATA;
    }
    else if (this->cbBufferValid == filestatus.cbFile && offBuffer == 0)
    {
        /*
         * The entire file is buffered
         *      Complete the request.
         */
        memcpy(pvBuffer, &pachBuffer[offVirtual], (size_t)cbBuffer);
        offVirtual += cbBuffer;
        rc = NO_ERROR;
    }
    else if (pachBuffer && cbBuffer <= this->cbBuffer)
    {   /*
         * Buffered read. (request not bigger than the buffer!)
         *      Update status (filesize).
         *      Check if the request is with the bounds of the file.
         *      Not in buffer?
         *          Then read more data into buffer.
         *      In buffer?
         *          Then complete the request.
         *
         */
        while (cbBuffer > 0)
        {
            /* Part or all in buffer? */
            if (pachBuffer != NULL &&
                offVirtual >= offBuffer &&
                offVirtual < offBuffer + cbBufferValid
                )
            {   /* copy data from buffer */
                cbRead = cbBufferValid - offVirtual + offBuffer;
                cbRead = min(cbRead, cbBuffer);
                memcpy(pvBuffer, &pachBuffer[offVirtual - offBuffer], (size_t)cbRead);
                offVirtual += cbRead;
                pvBuffer = (char*)pvBuffer + cbRead;
                cbBuffer -= cbRead;
            }
            else
            {
                /* read into buffer */
                if (!bufferRead(offVirtual))
                    return FALSE;
            }
        }

        rc = NO_ERROR;
    }
    else if (position())
    {   /*
         * unbuffered read.
         */
        rc = DosRead(hFile, pvBuffer, cbBuffer, &cbRead);
        if (rc == NO_ERROR)
            offVirtual = offReal += cbRead;
    }

    /* check for error and return accordingly */
    if (rc)
    {
        if (fThrowErrors)
            throw ((int)rc);
        return FALSE;
    }
    return TRUE;
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
 * Reads a single line from the file into the given buffer.
 * Newline is stripped!
 * @returns Success indicator.
 * @param   pszBuffer   Pointer to string buffer.
 *                      Will hold a zero-string upon successful return.
 * @param   cchBuffer   Buffer size.
 * @sketch
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL            kFile::readln(char *pszBuffer, long cchBuffer) throw (int)
{
    long    cbRead;

    /* refresh file status (cbFile) */
    if (!refreshFileStatus())
        return FALSE;

    /*
     * Buffered read.
     *      Calc max read size.
     *      Loop buffer by buffer looking for a newline.
     */
    cbRead = min(max((long)filestatus.cbFile - (long)offVirtual, 0), cchBuffer-1);
    if (cbRead == 0)
        return FALSE;

    while (cbRead > 0)
    {
        char *pszNewLine;
        char *pszReturn;
        char *pszEnd;
        unsigned long cch;

        /* read more buffer ? */
        if (offVirtual >= offBuffer + cbBufferValid || offVirtual < offBuffer)
            if (!bufferRead(offVirtual))
                return FALSE;

        /* Scan buffer for new line */
        pszNewLine = (char*)memchr(&pachBuffer[offVirtual - offBuffer], '\r', (size_t)(cbBufferValid - offVirtual + offBuffer));
        pszReturn  = (char*)memchr(&pachBuffer[offVirtual - offBuffer], '\n', (size_t)(cbBufferValid - offVirtual + offBuffer));
        if (pszNewLine != NULL || pszReturn != NULL)
        {
            pszEnd = pszReturn != NULL && (pszNewLine == NULL || pszReturn < pszNewLine) ?
                    pszReturn : pszNewLine;
            cch = pszEnd - &pachBuffer[offVirtual - offBuffer];
        }
        else
        {
            pszEnd = NULL;
            cch = cbBufferValid - offVirtual + offBuffer;
        }

        /* copy into result buffer */
        memcpy(pszBuffer, &pachBuffer[offVirtual - offBuffer], (size_t)cch);
        cbRead -= cch;
        offVirtual += cch;
        pszBuffer += cch;
        *pszBuffer = '\0';

        /* check for completion */
        if (pszEnd != NULL)
        {   /* skip newline */
            if (pszEnd[0] == '\r')
            {
                if (cch + 1 > cbBufferValid)
                {
                    if (bufferRead(offBuffer + cbBufferValid))
                        pszEnd = pachBuffer;
                }
                else
                    pszEnd++;
                offVirtual += pszEnd != NULL && *pszEnd == '\n'  ? 2 : 1;
            }
            else
                offVirtual++;
            return TRUE;
        }
    }

    return TRUE;
}


/**
 * Writes <cbBuffer> bytes to the file at the current file position.
 * @returns     success indicator. (TRUE/FALSE)
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to write.
 */
BOOL            kFile::write(const void *pv, long cb)
{
    if (fReadOnly)
        rc = ERROR_ACCESS_DENIED;
    else
    {
        /* buffered writes? */
        if (pachBuffer != NULL)
        {
            ULONG cbWrite;
            ULONG cbAddPost = 0;

            /*
             * New buffer algorithm.
             *  Init buffer if it's invalid.
             *  Loop until no more to write
             *      If All fits into current buffer Then
             *          Insert it. COMPLETED.
             *      If Start fits into the current buffer Then
             *          Insert it.
             *      Else If End fits into the current buffer Then
             *          Insert it.
             *      Else //nothing fit's into the buffer
             *          Commit the buffer.
             *          Initiate the buffer to the current offset.
             *          Insert as much as possible.
             *      Endif
             *  EndLoop
             */
            if (offBuffer == ~0UL)
            {   /* Empty buffer at current virtual offset */
                fBufferDirty = 0;
                cbBufferValid = 0;
                offBuffer = offVirtual;
            }

            while (cb > 0)
            {
                if (    offBuffer <= offVirtual
                    &&  offBuffer + cbBufferValid >= offVirtual
                    &&  offBuffer + cbBuffer >= offVirtual + cb
                    )
                {   /* everything fits into the buffer */
                    memcpy(&pachBuffer[offVirtual - offBuffer], pv, cb);
                    if (cbBufferValid < cb + offVirtual - offBuffer)
                        cbBufferValid = cb + offVirtual - offBuffer;
                    offVirtual += cb + cbAddPost;
                    fBufferDirty = TRUE;
                    return TRUE;
                }

                if (    offBuffer <= offVirtual
                    &&  offBuffer + cbBufferValid >= offVirtual
                    &&  offBuffer + cbBuffer < offVirtual
                    )
                {   /* start fits into the buffer */
                    cbWrite = cbBuffer - (offVirtual - offBuffer);
                    memcpy(&pachBuffer[offVirtual - offBuffer], pv, cbWrite);
                    cbBufferValid = cbBuffer;
                    offVirtual += cbWrite;
                    cb -= cbWrite;
                    pv = (char*)pv + cbWrite;
                }
                else if (   offBuffer > offVirtual
                         && offBuffer <= offVirtual + cb
                         && offBuffer + cbBuffer >= offVirtual + cb
                         )
                {   /* end fits into the buffer */
                    cbWrite = offVirtual + cb - offBuffer;
                    memcpy(pachBuffer, pv, cbWrite);
                    if (cbBufferValid <  cbWrite)
                        cbBufferValid = cbWrite;
                    cbAddPost += cbWrite;
                    cb -= cbWrite;
                }
                else
                {   /* don't fit anywhere... */
                    if (!bufferCommit())
                        return FALSE;
                    offBuffer = offVirtual;
                    cbWrite = cbBufferValid = cb > cbBuffer ? cbBuffer : cb;
                    memcpy(pachBuffer, pv, cbWrite);
                    cb -= cbWrite;
                    pv = (char*)pv + cbWrite;
                    offVirtual += cbWrite;
                }
                fBufferDirty = TRUE;


            }   /* loop */
            offVirtual += cbAddPost;

            return TRUE;
        }
        else if (position())
        {   /* non-buffered write! */
            ULONG   cbWrote;

            rc = DosWrite(hFile, (PVOID)pv, cb, &cbWrote);
            if (rc == NO_ERROR)
            {
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
    static char    szBuffer[1024*64];   //64KB should normally be enough for anyone...

    va_start(arg, pszFormat);
    pszEnd = vsprintf(szBuffer, pszFormat, arg) + szBuffer;
    va_end(arg);

    psz = pszEnd;
    while (psz > szBuffer)
    {
        if (*psz-- == '\n' && *psz != '\r')
        {
            memmove(psz+2, psz+1, pszEnd - psz + 1);
            psz[1] = '\r';
            pszEnd++;
        }
    }

    write(szBuffer, pszEnd - szBuffer);

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
 * Appends the AppendFile to this file.
 * @returns Reference to this file.
 * @param   AppendFile  Reference to the file we're to append.
 */
kFile &         kFile::operator+=(kFile &AppendFile)
{
    long    cb;
    char *  pachBuffer  = new char[1024*256];
    long    pos         = AppendFile.getPos();
    BOOL    fAppend     = AppendFile.fThrowErrors;
    BOOL    fThis       = fThrowErrors;

    setThrowOnErrors();
    AppendFile.setThrowOnErrors();

    end();
    AppendFile.start();
    AppendFile.refreshFileStatus();

    cb = min(1024*256, AppendFile.filestatus.cbFile);
    while (cb > 0)
    {
        AppendFile.read(pachBuffer, cb);
        write(pachBuffer, cb);
        cb = min(1024*256, (long)AppendFile.filestatus.cbFile - (long)AppendFile.offVirtual);
    }

    delete pachBuffer;
    AppendFile.set(pos);
    AppendFile.fThrowErrors = fAppend;
    fThrowErrors            = fThis;

    return *this;
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

    if (!fReadOnly && pachBuffer && offBuffer != ~0UL && offBuffer + cbBufferValid > filestatus.cbFile)
        /* a writable file with buffer might have uncommited data in the buffer. */
        offVirtual = offBuffer + cbBufferValid;
    else
        offVirtual = filestatus.cbFile;

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
    if (!fReadOnly && !refreshFileStatus())
        return (BOOL)-1;

    return filestatus.cbFile <= offVirtual; //??? - !!!
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



/**
 * Reads the specified file in to a memory block and returns it.
 * @returns Pointer to memory mapping on success.
 *          NULL on error.
 * @param   pszFilename     Name of the file.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  May throw errors.
 */
void *kFile::readFile(const char *pszFilename)
{
    kFile file(pszFilename);
    return file.readFile();
}

