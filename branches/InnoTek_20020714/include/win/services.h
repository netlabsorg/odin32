/* $Id: services.h,v 1.1 1999-05-24 20:19:18 ktk Exp $ */

/*
 * Kernel Service Thread API
 *
 * Copyright 1999 Ulrich Weigand
 */

#ifndef __WINE_SERVICES_H
#define __WINE_SERVICES_H

#include "winbase.h"

BOOL SERVICE_Init( void );

HANDLE SERVICE_AddObject( HANDLE object,
                          PAPCFUNC callback, ULONG_PTR callback_arg );

HANDLE SERVICE_AddTimer( LONG rate,
                         PAPCFUNC callback, ULONG_PTR callback_arg );

BOOL SERVICE_Delete( HANDLE service );

BOOL SERVICE_Enable( HANDLE service );

BOOL SERVICE_Disable( HANDLE service );


#endif /* __WINE_SERVICES_H */

