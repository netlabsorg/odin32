/* $Id: virtual.h,v 1.1 1999-08-21 16:30:53 sandervl Exp $ */

/*
 * Some help functions
 *
 * Copyright 1999 Sander van Leeuwen
 */

#ifndef __VIRTUAL_H
#define __VIRTUAL_H

LPVOID WINAPI VIRTUAL_MapFileA( LPCSTR name );
LPVOID WINAPI VIRTUAL_MapFileW( LPCWSTR name );

#endif  /* __VIRTUAL_H */
