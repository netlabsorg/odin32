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
 
BOOL HandleNamesResolveName(PSZ pszName,
                            PSZ pszTarget,
                            ULONG ulTargetLength,
                            BOOL fCaseInsensitive);

BOOL HandleNamesAddSymbolicLink(PSZ pszSymbolicLink,
                                PSZ pszTarget);

BOOL HandleNamesRemoveSymbolicLink(PSZ pszSymbolicLink);

BOOL HandleNamesRemoveTarget(PSZ pszTarget);


#endif /* _HANDLENAMES_H_ */