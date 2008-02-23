/* $Id: OS2KIO.h,v 1.2 2001-09-26 03:52:35 bird Exp $
 *
 * OS/2 kernel I/O: prototypes, typedefs and macros.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (kosmunds@csc.com)
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
APIRET KRNLCALL OrgIOSftOpen(
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
APIRET KRNLCALL OrgIOSftClose(
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
APIRET KRNLCALL OrgIOSftTransPath(
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
APIRET KRNLCALL OrgIOSftReadAt(
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
APIRET KRNLCALL OrgIOSftWriteAt(
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
APIRET KRNLCALL OrgSftFileSize(
    SFN hFile,
    PULONG pcbFile
    );

#endif
