/* $Id: OS2KIO.h,v 1.2 2000-01-17 16:52:25 bird Exp $
 *
 * OS/2 kernel IO, typedefs and macros.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2KIO_h_
#define _OS2KIO_h_

APIRET KRNLCALL IOSftOpen(PSZ pszFilename,
                          ULONG flOpenFlags,  /* probably similar to DosOpen */
                          ULONG fsOpenMode,   /* probably similar to DosOpen */
                          PSFN phFile,
                          PULONG pulsomething); /* EA? */
APIRET KRNLCALL IOSftClose(SFN hFile);
APIRET KRNLCALL IOSftTransPath(PSZ pszPath /* IN and OUT? */);
APIRET KRNLCALL IOSftReadAt(SFN hFile,
                            PULONG pulActual,
                            PVOID pvBuffer,
                            ULONG flFlags,
                            ULONG cbBuffer);
APIRET KRNLCALL IOSftWriteAt(SFN hFile,
                            PULONG pulActual,
                            PVOID pvBuffer,
                            ULONG flFlags,
                            ULONG cbBuffer);
/* APIRET KRNLCALL IOSft(1,2,3,4,5); */

#endif
