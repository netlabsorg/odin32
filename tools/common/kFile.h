/* $Id: kFile.h,v 1.8 2001-04-17 00:26:10 bird Exp $
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
    BOOL            fStdDev;            /* True if stdio, stderr or stdin. Filestatus is invalid with this is set.*/
    APIRET          rc;                 /* Last error (return code). */
    FILESTATUS4     filestatus;         /* Filestatus data. */
    BOOL            fStatusClean;       /* True if filestatus is clean. False is not clean */
    BOOL            fThrowErrors;       /* When true we'll throw the rc on errors, else return FALSE. */
    PSZ             pszFilename;        /* Pointer to filename. */

    /** @cat Position datamembers */
    unsigned long   offVirtual;         /* Virtual file position - lazy set. */
    unsigned long   offReal;            /* Real file position. */

    /** @cat Buffering datamembers */
    char *          pachBuffer;         /* Pointer to the buffer. NULL if not buffering. */
    unsigned long   cbBuffer;           /* Count of allocated bytes. */
    unsigned long   offBuffer;          /* Virtual file offset where the buffer starts. */
    unsigned long   cbBufferValid;      /* Count of valid bytes in the buffer. */
    BOOL            fBufferDirty;       /* Dirty flag. Set when the buffer needs to be committed. */

    /** @cat internal buffer methods */
    BOOL            bufferRead(ULONG offFile) throw (int);
    BOOL            bufferCommit(void) throw (int);

    /** @cat internal methods for maintaing internal structures. */
    BOOL            refreshFileStatus() throw(int);
    BOOL            position() throw(int);

    /** @cat constructors */
private:
    kFile(HFILE hFile, BOOL fReadOnly);
public:
    kFile(const char *pszFilename, BOOL fReadOnly = TRUE) throw(int);
    ~kFile();

    /** @cat File I/O methods */
    BOOL            read(void *pvBuffer, long cbBuffer) throw(int);
    BOOL            readAt(void *pvBuffer, long cbBuffer, long off) throw(int);
    void *          readFile() throw(int);
    static void *   readFile(const char *pszFilename) throw(int);
    BOOL            readln(char *pszBuffer, long cchBuffer);

    BOOL            write(void *pvBuffer, long cbBuffer) throw(int);
    BOOL            writeAt(void *pvBuffer, long cbBuffer, long off) throw(int);

    int             printf(const char *pszFormat, ...) throw (int);

    BOOL            setSize(unsigned long cbFile = ~0UL);

    kFile &         operator+=(kFile &AppendFile);

    /** @cat File seek methods */
    BOOL            move(long off) throw(int);
    BOOL            set(long off) throw(int);
    BOOL            end() throw(int);
    BOOL            start();

    /** @cat Query methods */
    long            getSize() throw(int);
    long            getPos() const throw(int);
    BOOL            isEOF() throw(int);
    const char *    getFilename()       { return pszFilename; }

    /** @cat Error handling */
    BOOL            setThrowOnErrors();
    BOOL            setFailOnErrors();
    int             getLastError() const;

    /** standard files */
    static kFile    StdOut;
    static kFile    StdIn;
    static kFile    StdErr;
};

#endif

