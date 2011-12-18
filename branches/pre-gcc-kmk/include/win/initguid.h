/* $Id: initguid.h,v 1.1 1999-05-24 20:19:13 ktk Exp $ */

/*
 * defines a minimum set of macros create GUID's to keep the size 
 * small
 *
 * this file should be included into "only GUID definition *.h" like
 * shlguid.h
 */

#ifndef __INIT_GUID_H
#define __INIT_GUID_H

#include "wtypes.h"

/*****************************************************************************
 * Macros to declare the GUIDs
 */
#undef DEFINE_GUID

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const GUID name = \
	{ l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#endif 
