/* $Id: OS2KSEC.h,v 1.1.2.1 2000-08-30 04:11:30 bird Exp $
 *
 * OS/2 Kernel Security helpers : prototypes, typedefs and macros.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2KSEC_h_
#define _OS2KSEC_h_


/**
 * Gets the path (name with fully qualified path) from a SFN.
 * @returns     Pointer to the path of hFile.
 * @param       hFile   SFN filehandle.
 */
PSZ SECCALL SecPathFromSFN(SFN hFile);


#endif
