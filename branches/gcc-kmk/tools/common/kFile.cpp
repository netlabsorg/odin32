/* $Id: kFile.cpp,v 1.10 2002-02-24 02:47:24 bird Exp $
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
#ifdef __EMX__
#define __OS2DEF__
#define OS2EMX_PLAIN_CHAR
#endif
#include <os2.h>

#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "kTypes.h"
#include "kError.h"
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
KBOOL   kFile::refreshFileStatus() throw (kError)
{
    if (fStdDev)
        return fStatusClean = TRUE;

    if (!fStatusClean)
    {
        rc = DosQueryFileInfo(OSData.os2.hFile, FIL_QUERYEASIZE, &OSData.os2.filestatus, sizeof(OSData.os2.filestatus));
        fStatusClean = (rc == NO_ERROR);
        if (!fStatusClean && fThrowErrors)
            throw (kError(rc));
    }
    else
        rc = NO_ERROR;

    return fStatusClean;
}


/**
 * Changes the real file position to match the virtual file position.
 * @returns     Success indicator.
 */
KBOOL   kFile::position() throw (kError)
{
    /*
     * If virtual file offset is different from the real,
     * issue a set file pointer call.
     */
    if (offVirtual != offReal)
    {
        ULONG   off;
        rc = DosSetFilePtr(OSData.os2.hFile, offVirtual, FILE_BEGIN, &off);
        if (rc != NO_ERROR || off != offVirtual)
        {
            if (fThrowErrors)
                throw (kError(rc));
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
KBOOL kFile::bufferRead(unsigned long offFile) throw(kError)
{
    ULONG   cbRead;

    /* refresh file status (cbFile) */
    if (!refreshFileStatus())
        return FALSE;

    /* check that the request is valid */
    if (offFile > OSData.os2.filestatus.cbFile)
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
    if (fReadOnly && cbBuffer + offFile > OSData.os2.filestatus.cbFile)
        offFile = OSData.os2.filestatus.cbFile > cbBuffer ? OSData.os2.filestatus.cbFile - cbBuffer : 0UL;

    /* need to change file ptr? */
    if (offFile != offReal)
    {
        ULONG ul;
        rc = DosSetFilePtr(OSData.os2.hFile, offFile, FILE_BEGIN, &ul);
        if (rc != NO_ERROR)
        {
            if (fThrowErrors)
                throw (kError(rc));
            return FALSE;
        }
        offReal = offFile;
    }

    /* read from the file */
    cbRead = KMIN(OSData.os2.filestatus.cbFile - offFile, cbBuffer);
    rc = DosRead(OSData.os2.hFile, pachBuffer, cbRead, &cbRead);
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
            throw (kError(rc));
        return FALSE;
    }

    return TRUE;
}


/**
 * Commits the data in the buffer.
 * @returns Success indicator.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
KBOOL kFile::bufferCommit(void) throw(kError)
{
    ULONG   cbWrote;
    ULONG   ul;

    /* don't commit clean buffers! */
    if (!fBufferDirty)
        return TRUE;

    /* need to change file ptr? */
    if (offBuffer != offReal)
    {
        rc = DosSetFilePtr(OSData.os2.hFile, offBuffer, FILE_BEGIN, &ul);
        if (rc != NO_ERROR)
        {
            if (fThrowErrors)
                throw (kError(rc));
            return FALSE;
        }
        offReal = offBuffer;
    }

    /* write to the file */
    rc = DosWrite(OSData.os2.hFile, pachBuffer, cbBufferValid, &cbWrote);
    fStatusClean = FALSE;
    if (rc == NO_ERROR)
    {
        fBufferDirty = FALSE;
        offReal += cbWrote;
    }
    else
    {
        DosSetFilePtr(OSData.os2.hFile, offReal, FILE_BEGIN, &ul);
        if (fThrowErrors)
            throw (kError(rc));
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
kFile::kFile(HFILE hFile, KBOOL fReadOnly)
:   fReadOnly(fReadOnly),
    fStatusClean(FALSE),
    fThrowErrors(FALSE),
    offVirtual(0),
    offReal(0),
    pszFilename(NULL),
    fStdDev(TRUE),
    pachBuffer(NULL),
    cbBufferValid(0UL),
    offBuffer(~0UL),
    fBufferDirty(FALSE)
{
    OSData.os2.hFile = hFile;
    if (!refreshFileStatus())
        throw (kError(rc));
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
kFile::kFile(const char *pszFilename, KBOOL fReadOnly/*=TRUE*/) throw (kError)
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

    rc = DosOpen((PCSZ)pszFilename, &OSData.os2.hFile, &ulAction, 0, FILE_NORMAL,
                 fulOpenFlags, fulOpenMode, NULL);
    if (rc != NO_ERROR)
        throw (kError(rc));

    if (!refreshFileStatus())
        throw (kError(rc));

    char szFullName[CCHMAXPATH];
    if (DosQueryPathInfo(pszFilename, FIL_QUERYFULLNAME, szFullName, sizeof(szFullName)))
        strcpy(szFullName, pszFilename);
    this->pszFilename = strdup(szFullName);
    if (this->pszFilename == NULL)
        throw (ERROR_NOT_ENOUGH_MEMORY);

    /* Buffering */
    cbBuffer   = (fReadOnly && OSData.os2.filestatus.cbFile < 32768) ? OSData.os2.filestatus.cbFile : 8192;
    pachBuffer = new char[cbBuffer];
    if (pachBuffer == NULL)
        throw (ERROR_NOT_ENOUGH_MEMORY);
    if (fReadOnly && OSData.os2.filestatus.cbFile < 32768)
    {
        if (!bufferRead(0))
            throw (kError(rc));
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
    DosClose(OSData.os2.hFile);
}


/**
 * Reads <cbBuffer> bytes from the current file posistion into the buffer.
 * @returns     0 on success. kError error number.
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to read.
 */
int             kFile::read(void *pvBuffer, long cbBuffer) throw (kError)
{
    ULONG   cbRead;

    /* Validate parameters */
    if (cbBuffer == 0)
        return NO_ERROR;
    if (cbBuffer < 0)
    {
        rc = ERROR_INVALID_PARAMETER;
        if (fThrowErrors)
            throw (kError(rc));
        return rc;
    }

    /* refresh file status (cbFile) */
    if (!refreshFileStatus())
        return rc;

    /* check if valid request */
    if (    offVirtual > OSData.os2.filestatus.cbFile
        ||  offVirtual + cbBuffer > OSData.os2.filestatus.cbFile
        )
    {   /* invalid request */
        rc = ERROR_NO_DATA;
    }
    else if (this->cbBufferValid == OSData.os2.filestatus.cbFile && offBuffer == 0)
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
                cbRead = KMIN(cbRead, cbBuffer);
                memcpy(pvBuffer, &pachBuffer[offVirtual - offBuffer], (size_t)cbRead);
                offVirtual += cbRead;
                pvBuffer = (char*)pvBuffer + cbRead;
                cbBuffer -= cbRead;
            }
            else
            {
                /* read into buffer */
                if (!bufferRead(offVirtual))
                    return rc;
            }
        }

        rc = NO_ERROR;
    }
    else if (position())
    {   /*
         * unbuffered read.
         */
        rc = DosRead(OSData.os2.hFile, pvBuffer, cbBuffer, &cbRead);
        if (rc == NO_ERROR)
            offVirtual = offReal += cbRead;
    }

    /* check for error and return accordingly */
    if (rc && fThrowErrors)
        throw (kError(rc));
    return rc;
}


/**
 * Reads <cbBuffer> bytes at file offset <off>.
 * @returns     0 on success. kError error number.
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to read.
 * @param       off         Absolute file offset.
 */
int             kFile::readAt(void *pvBuffer, long cbBuffer, long off) throw (kError)
{
    if (set(off))
        return rc;
    read(pvBuffer, cbBuffer);
    return rc;
}


/**
 * Reads the entire file into a single memory block.
 * (The memory block has a '\0' at the end just in case you
 *  are using it as a long string.)
 * Must call the static kFile::mapFree function to free the memory block.
 * @returns     Pointer to file in memory.
 */
void *          kFile::mapFile() throw(kError)
{
    void *pv;

    /* allocate memory for the file */
    pv = calloc((size_t)this->getSize() + 1, 1);
    if (pv == NULL)
    {
        if (fThrowErrors)
            throw (kError(kError::NOT_ENOUGH_MEMORY));
        return NULL;
    }

    /* go the start of the file and read it. */
    if (!start() && !read(pv, this->getSize()))
        return pv; // successfull exit!

    /* we failed, cleanup and return NULL */
    free(pv);
    return NULL;
}


/**
 * Reads a single line from the file into the given buffer.
 * Newline is stripped!
 * @returns 0 on success. kError error number.
 * @param   pszBuffer   Pointer to string buffer.
 *                      Will hold a zero-string upon successful return.
 * @param   cchBuffer   Buffer size.
 * @sketch
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int             kFile::readln(char *pszBuffer, long cchBuffer)
{
    long    cbRead;

    /* refresh file status (cbFile) */
    if (!refreshFileStatus())
        return rc;

    /*
     * Buffered read.
     *      Calc max read size.
     *      Loop buffer by buffer looking for a newline.
     */
    cbRead = KMIN(KMAX((long)OSData.os2.filestatus.cbFile - (long)offVirtual, 0), cchBuffer-1);
    if (cbRead == 0)
        return rc = ERROR_HANDLE_EOF;

    while (cbRead > 0)
    {
        char *pszNewLine;
        char *pszReturn;
        char *pszEnd;
        unsigned long cch;

        /* read more buffer ? */
        if (offVirtual >= offBuffer + cbBufferValid || offVirtual < offBuffer)
            if (!bufferRead(offVirtual))
                return rc;

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
            return NO_ERROR;
        }
    }

    return NO_ERROR;
}


/**
 * Writes <cbBuffer> bytes to the file at the current file position.
 * @returns     0 on success. kError error number.
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to write.
 */
int             kFile::write(const void *pv, long cb) throw (kError)
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
                    return NO_ERROR;
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
                        return rc;
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

            return NO_ERROR;
        }
        else if (position())
        {   /* non-buffered write! */
            ULONG   cbWrote;

            rc = DosWrite(OSData.os2.hFile, (PVOID)pv, cb, &cbWrote);
            if (rc == NO_ERROR)
            {
                offVirtual = offReal += cbWrote;
                return NO_ERROR;
            }
        }
    }

    if (fThrowErrors)
        throw (kError(rc));
    return rc;
}


/**
 * Write <cbBuffer> bytes at file offset <off> from <pvBuffer>.
 * @returns     0 on success. kError error number.
 * @param       pvBuffer    Output buffer.
 * @param       cbBuffer    Amount of bytes to write.
 * @param       off         Absolute file offset.
 */
int             kFile::writeAt(const void *pvBuffer, long cbBuffer, long off) throw (kError)
{
    if (set(off))
        return rc;
    return write(pvBuffer, cbBuffer);
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
int             kFile::printf(const char *pszFormat, ...) throw(kError)
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
 * @returns     0 on success. kError error number.
 * @param       cbFile      New filesize.
 *                          Defaults to 0xffffffff, which results in
 *                          cutting the file at the current position.
 */
int             kFile::setSize(unsigned long cbFile/*= ~0UL*/)
{
    if (cbFile == ~0UL)
        cbFile = offVirtual;
    rc = DosSetFileSize(OSData.os2.hFile, cbFile);
    if (rc != NO_ERROR && fThrowErrors)
        throw (kError(rc));

    return rc;
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
    KBOOL   fAppend     = AppendFile.fThrowErrors;
    KBOOL   fThis       = fThrowErrors;

    setThrowOnErrors();
    AppendFile.setThrowOnErrors();

    end();
    AppendFile.start();
    AppendFile.refreshFileStatus();

    cb = KMIN(1024*256, AppendFile.OSData.os2.filestatus.cbFile);
    while (cb > 0)
    {
        AppendFile.read(pachBuffer, cb);
        write(pachBuffer, cb);
        cb = KMIN(1024*256, (long)AppendFile.OSData.os2.filestatus.cbFile - (long)AppendFile.offVirtual);
    }

    delete pachBuffer;
    AppendFile.set(pos);
    AppendFile.fThrowErrors = fAppend;
    fThrowErrors            = fThis;

    return *this;
}


/**
 * Seek relative to the current position.
 * @returns     0 on success. kError error number.
 * @param       off     Relative reposition.
 */
int             kFile::move(long off) throw (kError)
{
    if ((off + offVirtual) & 0x80000000UL) /* above 2GB or negative */
        rc = ERROR_NEGATIVE_SEEK;
    else
    {
        if (off + offVirtual > OSData.os2.filestatus.cbFile && fReadOnly) /* can't expand readonly file. */
            rc = ERROR_HANDLE_EOF;
        else
        {
            offVirtual += off;
            return rc = NO_ERROR;
        }
    }

    if (fThrowErrors)
        throw (kError(rc));
    return rc;
}


/**
 * Seek to an absolute position in the file (off).
 * @returns     0 on success. kError error number.
 * @param       off     New file position.
 */
int             kFile::set(long off) throw (kError)
{
    if (off < 0)
        rc = ERROR_NEGATIVE_SEEK;
    else
    {
        if ((unsigned long)off > OSData.os2.filestatus.cbFile && fReadOnly)
            rc = ERROR_HANDLE_EOF;
        else
        {
            offVirtual = off;
            return rc = NO_ERROR;
        }
    }
    if (fThrowErrors)
        throw (kError(rc));
    return rc;
}


/**
 * Seek to the end of the file.
 * @returns     0 on success. kError error number.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
int             kFile::end() throw (kError)
{
    if (!refreshFileStatus())
        return rc;

    if (!fReadOnly && pachBuffer && offBuffer != ~0UL && offBuffer + cbBufferValid > OSData.os2.filestatus.cbFile)
        /* a writable file with buffer might have uncommited data in the buffer. */
        offVirtual = offBuffer + cbBufferValid;
    else
        offVirtual = OSData.os2.filestatus.cbFile;

    return rc = NO_ERROR;
}


/**
 * Seek to the start of the file.
 * @returns     0 on success. kError error number.
 * @remark      Will never throw errors.
 */
int             kFile::start()
{
    offVirtual = 0;
    return rc = NO_ERROR;
}


/**
 * Get the size of the file.
 * @returns     Returns file size on success.
 *              -1 on error.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
long            kFile::getSize() throw (kError)
{
    if (!refreshFileStatus())
        return -1;

    return OSData.os2.filestatus.cbFile;
}


/**
 * Get current position.
 * @returns     The current file position.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
long            kFile::getPos() const throw (kError)
{
    return offVirtual;
}


/**
 * Checks if we have reached the file end.
 * @returns     TRUE if end-of-file is reached.
 *              FALSE is not end-of-file.
 * @remark      Will only throw error if refreshFileStatus failes.
 */
KBOOL           kFile::isEOF() throw (kError)
{
    #if 0
    throw (kError(kError::NOT_SUPPORTED)); //this method don't currently work! Need to use flag!
    #else
    if (!fReadOnly && !refreshFileStatus())
        return (KBOOL)-1;

    return OSData.os2.filestatus.cbFile <= offVirtual; //??? - !!!
    #endif
}


/**
 * Set error behaviour to fail by throwing the OS/2 return code when an
 * error occures.
 * @remark      Will never throw errors.
 */
void            kFile::setThrowOnErrors()
{
    fThrowErrors = TRUE;
    rc = NO_ERROR;
}


/**
 * Set error behaviour to fail by return FALSE when an error has occures.
 * @remark      Will never throw errors.
 */
void            kFile::setFailOnErrors()
{
    fThrowErrors = FALSE;
    rc = NO_ERROR;
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
void *kFile::mapFile(const char *pszFilename) throw (kError)
{
    kFile file(pszFilename);
    file.setThrowOnErrors();
    return file.mapFile();
}



/**
 * Frees a file mapping done by one of the mapFile members of kFile.
 * @param   pvFileMapping   The pointer mapFile returned.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
void kFile::mapFree(void *pvFileMapping) throw (kError)
{
    if (pvFileMapping)
        free(pvFileMapping);
}

