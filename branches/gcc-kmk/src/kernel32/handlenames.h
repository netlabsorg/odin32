/* $Id: handlenames.h,v 1.1 2001-11-23 18:03:18 phaller Exp $ */

/*
 * Win32 Handle Manager Object Namespace for OS/2
 *
 * 2001/11/23 Patrick Haller <patrick.haller@innotek.de>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _HANDLENAMES_H_
#define _HANDLENAMES_H_


/*****************************************************************************
 * Exported Wrapper Functions
 *****************************************************************************/

BOOL HandleNamesResolveName(PCSZ pszName,
                            PSZ pszTarget,
                            ULONG ulTargetLength,
                            BOOL fCaseInsensitive);

BOOL HandleNamesAddSymbolicLink(PCSZ pszSymbolicLink,
                                PCSZ pszTarget);

BOOL HandleNamesRemoveSymbolicLink(PCSZ pszSymbolicLink);

BOOL HandleNamesRemoveTarget(PCSZ pszTarget);


#endif /* _HANDLENAMES_H_ */