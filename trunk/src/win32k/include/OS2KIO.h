/* $Id: OS2KIO.h,v 1.1 2000-01-14 19:21:12 bird Exp $
 *
 * OS/2 kernel IO, typedefs and macros.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2KIO_h_
#define _OS2KIO_h_

APIRET KRNLCALL IOSftOpen(PSZ pszFilename, ULONG ul2, ULONG ul3,
                          PSFN phFile, ULONG ul5);
APIRET KRNLCALL IOSftClose(SFN hFile);
APIRET KRNLCALL IOSftTransPath(
APIRET KRNLCALL IOSftReadAt(SFN hFile, PULONG pulActual, PVOID pvBuffer, ULONG flFlags, ULONG cbBuffer);
APIRET KRNLCALL IOSftWriteAt(SFN hFile, PULONG pulActual, PVOID pvBuffer, ULONG flFlags, ULONG cbBuffer);
APIRET KRNLCALL IOSft(1,2,3,4,5);

#endif
