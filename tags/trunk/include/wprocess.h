/* $Id: wprocess.h,v 1.2 1999-06-19 17:58:50 sandervl Exp $ */
/*
 * Process help functions
 *
 * Copyright (C) 1999 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _WPROCESS_H__
#define _WPROCESS_H__

//#define WIN32_TIBSEL

void InitializeTIB(BOOL fMainThread = FALSE);
void DestroyTIB();

#endif 