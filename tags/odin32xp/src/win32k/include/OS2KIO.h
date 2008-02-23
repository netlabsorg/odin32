/* $Id: OS2KIO.h,v 1.4 2000-02-19 08:40:30 bird Exp $
 *
 * OS/2 kernel IO: prototypes, typedefs and macros.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2KIO_h_
#define _OS2KIO_h_


/**
 * Opens a given file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    pszFilename   Pointer to filename.
 * @param    flOpenFlags   Open flags. (similar to DosOpen)
 * @param    fsOpenMode    Open mode flags. (similar to DosOpen)
 * @param    phFile        Pointer to filehandle.
 * @param    pulsomething  16-bit near (?) pointer to a variable - unknown. NULL is allowed. EA?
 */
APIRET KRNLCALL IOSftOpen(
    PSZ pszFilename,
    ULONG flOpenFlags,
    ULONG fsOpenMode,
    PSFN phFile,
    PULONG pulsomething
    );


/**
 * Closes the specified file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    hFile      File handle - System File Number.
 */
APIRET KRNLCALL IOSftClose(
    SFN hFile
    );


/**
 * Probably this function will expand a relative path to a full path.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    pszPath  Pointer to path to expand. Contains the full path upon return. (?)
 *                    This buffer should probably be of CCHMAXPATH length.
 */
APIRET KRNLCALL IOSftTransPath(
    PSZ pszPath
    );


/**
 * Read at a given offset in the a file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    hFile      File handle - System File Number.
 * @param    pcbActual  Pointer to variable which upon input holds the number
 *                      of bytes to read, on output the actual number of bytes read.
 * @param    pvBuffer   Pointer to the read buffer.
 * @param    flFlags    Read flags?
 * @param    ulOffset   File offset to read from. (0=start of file)
 */
APIRET KRNLCALL IOSftReadAt(
    SFN hFile,
    PULONG pcbActual,
    PVOID pvBuffer,
    ULONG flFlags,
    ULONG ulOffset
    );


/**
 * Write at a given offset in the a file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    hFile      File handle - System File Number.
 * @param    pcbActual  Pointer to variable which upon input holds the number
 *                      of bytes to write, on output the actual number of bytes write.
 * @param    pvBuffer   Pointer to the write buffer.
 * @param    flFlags    Read flags?
 * @param    ulOffset   File offset to write from. (0=start of file)
 */
APIRET KRNLCALL IOSftWriteAt(
    SFN hFile,
    PULONG pcbActual,
    PVOID pvBuffer,
    ULONG flFlags,
    ULONG ulOffset
    );


/**
 * Gets the filesize.
 * @returns   NO_ERROR on success; OS/2 error code on error.
 * @param     hFile    File handle - System File Number.
 * @param     pcbFile  Pointer to ULONG which will hold the file size upon return.
 */
APIRET KRNLCALL SftFileSize(
    SFN hFile,
    PULONG pcbFile
    );

#endif
