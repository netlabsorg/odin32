/* $Id: resource.h,v 1.2 1999-09-02 10:08:38 phaller Exp $ */

/*
 * Resource definitions
 *
 * Copyright 1995 Alexandre Julliard
 */

#ifndef __WINE_RESOURCE_H
#define __WINE_RESOURCE_H

#include "windef.h"

#ifndef __WRC_RSC_H
#include "wrc_rsc.h"
#endif

/*
 * BS: I comment this out to catch all occurences
 * of reference to this structure which is now
 * rendered obsolete.
 *
 * struct resource
 * {
 *     int id;
 *     int type;
 *     const char *name;
 *     const unsigned char* bytes;
 *     unsigned size;
 * };
 */

/* Built-in resources */
typedef enum
{
    SYSRES_MENU_SYSMENU,
    SYSRES_MENU_EDITMENU,
    SYSRES_DIALOG_MSGBOX
} SYSTEM_RESOURCE;

extern void LIBRES_RegisterResources(const wrc_resource32_t * const * Res);

#if defined(__GNUC__) && (__GNUC__ == 2) && (__GNUC_MINOR__ >= 7)
#define WINE_CONSTRUCTOR  __attribute__((constructor))
#define HAVE_WINE_CONSTRUCTOR
#else
#define WINE_CONSTRUCTOR
#undef HAVE_WINE_CONSTRUCTOR
#endif

extern HGLOBAL16 SYSRES_LoadResource( SYSTEM_RESOURCE id );
extern void SYSRES_FreeResource( HGLOBAL16 handle );
extern LPCVOID SYSRES_GetResPtr( SYSTEM_RESOURCE id );

#endif /* __WINE_RESOURCE_H */
