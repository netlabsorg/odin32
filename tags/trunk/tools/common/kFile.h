/* $Id: kFile.h,v 1.1 2000-05-23 18:23:05 bird Exp $
 *
 * kFile - Simple (for the time being) file class.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _kFile_h_
#define _kFile_h_



/**
 * Simple file class.
 * It uses a lazy algorithm for chaning the file position.
 * It is extenable to do buffered reading too.
 * @author      knut st. osmundsen
 */
class kFile
{
protected:
    /** @cat Main datamembers */
    HFILE           hFile;              /* Pointer to stdio filehandle */
    BOOL            fReadOnly;          /* True if readonly access, False is readwrite. */
    int             rc;                 /* Last error (return code). */
    FILESTATUS4     filestatus;         /* Filestatus data. */
    BOOL            fStatusClean;       /* True if filestatus is clean. False is not clean */
    BOOL            fThrowErrors;       /* When true we'll throw the rc on errors, else return FALSE. */

    /** @cat Position datamembers */
    unsigned long   offVirtual;         /* Virtual file position - lazy set. */
    unsigned long   offReal;            /* Real file position. */

    /** @cat Buffering datamembers */

    /** @cat internal methods for maintaing internal structures. */
    BOOL            refreshFileStatus();
    BOOL            position();

public:
    /** @cat constructor */
    kFile(const char *pszFilename, BOOL fReadOnly = TRUE) throw(int);
    ~kFile();

    /** @cat File I/O methods */
    BOOL            read(void *pvBuffer, long cbBuffer);
    BOOL            readAt(void *pvBuffer, long cbBuffer, long off);

    BOOL            write(void *pvBuffer, long cbBuffer);
    BOOL            writeAt(void *pvBuffer, long cbBuffer, long off);

    /** @cat File seek methods */
    BOOL            move(long off);
    BOOL            set(long off);
    BOOL            end();
    BOOL            start();

    /** @cat Query methods */
    LONG            getSize();
    BOOL            isEOF();

    /** @cat Error handling */
    BOOL            setThrowOnErrors();
    BOOL            setFailOnErrors();
    int             getLastError();
};

#endif

