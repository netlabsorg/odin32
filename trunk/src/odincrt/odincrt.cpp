/************************************************************************/
/*                             odincrt.c                                */
/************************************************************************/
/*                                                                      */
/*         Odin shared C runtime library for IBM VAC++ 3.08             */
/*                                                                      */
/************************************************************************/
/* Created:     99/08/08                                                */
/* Last Edited: 99/00/08                                                */
/************************************************************************/
/* (C)'99 Patrick Haller, Achim Hasenmueller                            */
/************************************************************************/

/* we need all prototypes for the C runtime library                     */
/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <odincrt.h>

typedef unsigned short USHORT;


/****************************************************************************
 * Macros                                                                   *
 ****************************************************************************/

#define ODIN_TEB_OFF         {            \
                               USHORT sel = RestoreOS2FS();

#define ODIN_TEB_ON1(rc)       SetFS(sel);\
                               return(rc);\
                             }

#define ODIN_TEB_ON0()         SetFS(sel);\
                             }


/****************************************************************************
 * Memory Management                                                        *
 ****************************************************************************/

void* ODINAPI ODIN_calloc ( size_t s1, size_t s2 )
  ODIN_TEB_OFF
  void* rc =  calloc(s1,s2);
  ODIN_TEB_ON1(rc)

void* ODINAPI ODIN_realloc( void * p1, size_t s1 )
  ODIN_TEB_OFF
  void* rc = realloc(p1,s1);
  ODIN_TEB_ON1(rc)

void* ODINAPI ODIN_malloc (size_t size)
  ODIN_TEB_OFF
  void *rc = malloc(size);
  ODIN_TEB_ON1(rc)

void ODINAPI ODIN_free    (void *ptr)
  ODIN_TEB_OFF
  free(ptr);
  ODIN_TEB_ON0()

void * ODINAPI  ODIN_debug_calloc ( size_t s1, size_t s2, const char *p1, size_t s3)
  ODIN_TEB_OFF
  void* rc =  _debug_calloc(s1,s2,p1,s3);
  ODIN_TEB_ON1(rc)

void   ODINAPI  ODIN_debug_free   ( void *p1, const char *p2, size_t s1)
  ODIN_TEB_OFF
  _debug_free(p1,p2,s1);
  ODIN_TEB_ON0()

void * ODINAPI  ODIN_debug_malloc ( size_t s1, const char * p1, size_t s2)
  ODIN_TEB_OFF
  void *rc = _debug_malloc(s1,p1,s2);
  ODIN_TEB_ON1(rc)

void * ODINAPI  ODIN_debug_realloc( void *p1, size_t s1, const char *p2, size_t s2)
  ODIN_TEB_OFF
  void* rc = _debug_realloc(p1,s1,p2,s2);
  ODIN_TEB_ON1(rc)




/****************************************************************************
 * String operations                                                        *
 ****************************************************************************/

char* ODINAPI ODIN_strdup( const char *s1)
  ODIN_TEB_OFF
  char *rc = strdup(s1);
  ODIN_TEB_ON1(rc)


/****************************************************************************
 * C++ operator wrappers (experimental)                                     *
 ****************************************************************************/

// operator new()
// @@@PH wrong calling convention
//void* _Optlink __nw__FUi(void* p1)
//  ODIN_TEB_OFF
//  void *rc = __nw__FUi(p1);
//  ODIN_TEB_ON1(rc)

