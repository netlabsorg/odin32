/* $Id: odincrt.h,v 1.1 1999-08-09 11:21:45 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * 1998/08/09
 *
 * Copyright 1998 Patrick Haller
 */


/*****************************************************************************
 * Name      : ORINCRT.H
 * Purpose   : This module maps all VAC++ runtime functions to thread-safe,
 *             Win32-TEB-safe functions.
 *****************************************************************************/


#ifndef __ODIN_H__
#define __ODIN_H__


#ifdef __cplusplus
  extern "C" {
#endif


/****************************************************************************
 * Macros                                                                   *
 ****************************************************************************/

#define ODINAPI _Export
typedef unsigned short USHORT;


/****************************************************************************
 * Prototypes                                                               *
 ****************************************************************************/

USHORT _System GetFS();
USHORT _System RestoreOS2FS();
void   _System SetFS(USHORT selector);
USHORT _System SetReturnFS(USHORT selector);

void * ODINAPI  ODIN_calloc ( size_t, size_t );
void   ODINAPI  ODIN_free   ( void * );
void * ODINAPI  ODIN_malloc ( size_t );
void * ODINAPI  ODIN_realloc( void *, size_t );

//@@@PH ODIN_new(myClass, ##myParams) new ....
//@@@PH ODIN_delete(myObject) ... delete myObject ...

#ifdef __cplusplus
  }
#endif

#endif /* __ODIN_H_ */

