/* $Id: kFile.h,v 1.2 2000-05-27 02:14:21 bird Exp $
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
    BOOL            refreshFileStatus() throw(int);
    BOOL            position() throw(int);

public:
    /** @cat constructor */
    kFile(const char *pszFilename, BOOL fReadOnly = TRUE) throw(int);
    ~kFile();

    /** @cat File I/O methods */
    BOOL            read(void *pvBuffer, long cbBuffer) throw(int);
    BOOL            readAt(void *pvBuffer, long cbBuffer, long off) throw(int);

    BOOL            write(void *pvBuffer, long cbBuffer) throw(int);
    BOOL            writeAt(void *pvBuffer, long cbBuffer, long off) throw(int);

    /** @cat File seek methods */
    BOOL            move(long off) throw(int);
    BOOL            set(long off) throw(int);
    BOOL            end() throw(int);
    BOOL            start();

    /** @cat Query methods */
    long            getSize() throw(int);
    long            getPos() const throw(int);
    BOOL            isEOF() throw(int);

    /** @cat Error handling */
    BOOL            setThrowOnErrors();
    BOOL            setFailOnErrors();
    int             getLastError() const;
};

#endif

