/* $Id: wprocess.h,v 1.1 1999-06-19 13:58:26 sandervl Exp $ */
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

void InitializeTIB(BOOL fMainThread = FALSE);
void DestroyTIB();

#endif 