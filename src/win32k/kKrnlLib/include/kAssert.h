/* $Id: kAssert.h,v 1.1 2001-09-14 01:50:17 bird Exp $
 *
 * kAssert - assert macro(s).
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 */
#ifndef _kAssert_h_
#define _kAssert_h_

#ifndef _kTypes_h_
#include <kTypes.h>
#endif

KBOOL KLIBCALL kAssertMsg(const char *pszExpr, const char *pszFilename, unsigned uLine, const char *pszFunction);

#define kASSERT(expr) \
    ((expr) ? (void)0 : (kAssertMsg(#expr, __FILE__, __LINE__, __FUNCTION__) ? INT3() : (void)0))

#endif /* _kAssert_h_ */


