/* $Id: kFile.h,v 1.10 2002-02-24 02:47:24 bird Exp $
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
    /** @cat constructors */
private:
    #ifndef __OS2DEFS__
    kFile(unsigned long hFile, KBOOL fReadOnly);
    #else
    kFile(HFILE hFile, KBOOL fReadOnly);
    #endif
public:
    kFile(const char *pszFilename, KBOOL fReadOnly = TRUE) throw(kError);
    ~kFile();

    /** @cat File I/O methods */
    int             read(void *pvBuffer, long cbBuffer) throw(kError);
    int             readAt(void *pvBuffer, long cbBuffer, long off) throw(kError);
    int             readln(char *pszBuffer, long cchBuffer);

    int             write(const void *pvBuffer, long cbBuffer) throw(kError);
    int             writeAt(const void *pvBuffer, long cbBuffer, long off) throw(kError);

    int             printf(const char *pszFormat, ...) throw(kError);

    int             setSize(unsigned long cbFile = ~0UL);

    kFile &         operator+=(kFile &AppendFile);

    void *          mapFile() throw(kError);
    static void *   mapFile(const char *pszFilename) throw(kError);
    static void     mapFree(void *pvFileMapping) throw(kError);


    /** @cat File seek methods */
    int             move(long off) throw(kError);
    int             set(long off) throw(kError);
    int             end() throw(kError);
    int             start();

    /** @cat Query methods */
    long            getSize() throw(kError);
    long            getPos() const throw(kError);
    KBOOL           isEOF() throw(kError);
    const char *    getFilename()       { return pszFilename; }

    /** @cat Error handling */
    void            setThrowOnErrors();
    void            setFailOnErrors();
    int             getLastError() const;

    /** standard files */
    static kFile    StdOut;
    static kFile    StdIn;
    static kFile    StdErr;


protected:
    /** @cat Main datamembers */
    KBOOL           fReadOnly;          /* True if readonly access, False is readwrite. */
    KBOOL           fStdDev;            /* True if stdio, stderr or stdin. Filestatus is invalid with this is set.*/
    unsigned long   rc;                 /* Last error (return code). */
    KBOOL           fStatusClean;       /* True if filestatus is clean. False is not clean */
    KBOOL           fThrowErrors;       /* When true we'll throw the rc on errors, else return FALSE. */
    char *          pszFilename;        /* Pointer to filename. */

    /** @cat Position datamembers */
    unsigned long   offVirtual;         /* Virtual file position - lazy set. */
    unsigned long   offReal;            /* Real file position. */

    /** @cat Buffering datamembers */
    char *          pachBuffer;         /* Pointer to the buffer. NULL if not buffering. */
    unsigned long   cbBuffer;           /* Count of allocated bytes. */
    unsigned long   offBuffer;          /* Virtual file offset where the buffer starts. */
    unsigned long   cbBufferValid;      /* Count of valid bytes in the buffer. */
    KBOOL           fBufferDirty;       /* Dirty flag. Set when the buffer needs to be committed. */

    /** @cat OS specific data */
    union _kFileOSSpecific
    {
        char        achFiller[14+2 + 16]; /* OS2 uses 16 bytes currently. Add 16-bytes just in case. */
        #ifdef __OS2DEF__
        struct _kFileOS2
        {
            HFILE           hFile;        /* Pointer to stdio filehandle */
            FILESTATUS4     filestatus;   /* Filestatus data. */
        } os2;
        #endif
    } OSData;

    /** @cat internal buffer methods */
    KBOOL           bufferRead(unsigned long offFile) throw(kError);
    KBOOL           bufferCommit(void) throw(kError);

    /** @cat internal methods for maintaing internal structures. */
    KBOOL           refreshFileStatus() throw(kError);
    KBOOL           position() throw(kError);

};

#endif

