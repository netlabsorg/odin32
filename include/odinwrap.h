/* $Id: odinwrap.h,v 1.15 1999-11-09 19:21:35 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * ODIN FS: Selector function wrapper macros
 *
 * Copyright 1999 Patrick Haller
 *
 */


#ifndef _ODINWRAP_H_
#define _ODINWRAP_H_


/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/


#define ODIN_INTERNAL _Optlink


#ifdef DEBUG
#  define ODINDEBUGCHANNEL(a) static char* pszOdinDebugChannel=#a;
#else
#  define ODINDEBUGCHANNEL(a)
#endif


//SvL: Define this to use the internal wrapper function of a specific api
#define ODIN_EXTERN(a)  ODIN_INTERNAL ODIN_##a

#ifdef DEBUG

//@@@PH 1999/10/25 IBM VAC++ debug memory support
#include <malloc.h>

#if 1
#define CheckFS(a)
#else
//SvL: Eases locating apis that corrupt FS
#define error_FSSelector "FS Selector for thread %d corrupted!!!"

extern int IsExeStarted(); //kernel32

#define CheckFS(sel) if(sel == 0x150b && IsExeStarted()) {          \
                          dprintf(((char *)error_FSSelector, GetCurrentThreadId()));    \
                     }
#endif

//SvL: Only check the heap very frequently when there are problems
//#define DEBUG_ODINHEAP

#ifdef DEBUG_ODINHEAP
#define ODIN_HEAPCHECK() _heap_check()
#else
#define ODIN_HEAPCHECK()
#endif

/****************************************************************************
 * General Wrapper Macros (debug instrumented)                              *
 ****************************************************************************/

/* ---------- 0 parameters ---------- */
#define ODINFUNCTION0(cRet,cName)             \
  cRet ODIN_INTERNAL ODIN_##cName (void);            \
  cRet WINAPI cName(void)              \
  {                                           \
    unsigned short sel = RestoreOS2FS();      \
    dprintf(("%s: "#cRet" "#cName"() enter\n",  \
             pszOdinDebugChannel));           \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName();              \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                               \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (void)


#define ODINPROCEDURE0(cName)                 \
  void ODIN_INTERNAL ODIN_##cName (void);            \
  void WINAPI cName(void)              \
  {                                           \
    unsigned short sel = RestoreOS2FS();      \
    dprintf(("%s: void "#cName"() enter\n",    \
             pszOdinDebugChannel));           \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName();                                  \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);                               \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (void)


/* ---------- 1 parameters ---------- */
#define ODINFUNCTION1(cRet,cName,t1,a1)       \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1);           \
  cRet WINAPI cName(t1 a1)             \
  {                                           \
    unsigned short sel = RestoreOS2FS();      \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1));                            \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1);                   \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                               \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1)

#define ODINPROCEDURE1(cName,t1,a1)           \
  void ODIN_INTERNAL ODIN_##cName (t1 a1);           \
  void WINAPI cName(t1 a1)             \
  {                                           \
    unsigned short sel = RestoreOS2FS();      \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1));                            \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1);                                \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);                               \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (t1 a1)


/* ---------- 2 parameters ---------- */
#define ODINFUNCTION2(cRet,cName,t1,a1,t2,a2)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);      \
  cRet WINAPI cName(t1 a1,t2 a2)        \
  {                                            \
    unsigned short sel = RestoreOS2FS();       \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2));                         \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2);                \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                               \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)

#define ODINPROCEDURE2(cName,t1,a1,t2,a2)     \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);     \
  void WINAPI cName(t1 a1,t2 a2)       \
  {                                           \
    unsigned short sel = RestoreOS2FS();      \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2));                         \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2);                             \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);                               \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)


/* ---------- 3 parameters ---------- */
#define ODINFUNCTION3(cRet,cName,t1,a1,t2,a2,t3,a3)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)

#define ODINPROCEDURE3(cName,t1,a1,t2,a2,t3,a3)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3);              \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);                   \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)


/* ---------- 4 parameters ---------- */
#define ODINFUNCTION4(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh, "#t4" "#a4"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4));                   \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)

#define ODINPROCEDURE4(cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh, "#t4" "#a4"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4));                    \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)


/* ---------- 5 parameters ---------- */
#define ODINFUNCTION5(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5));                \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)

#define ODINPROCEDURE5(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5));                \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)


/* ---------- 6 parameters ---------- */
#define ODINFUNCTION6(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6));             \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)

#define ODINPROCEDURE6(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6));             \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)


/* ---------- 7 parameters ---------- */
#define ODINFUNCTION7(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7));          \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)

#define ODINPROCEDURE7(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7));          \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)


/* ---------- 8 parameters ---------- */
#define ODINFUNCTION8(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8));       \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)

#define ODINPROCEDURE8(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8));       \
    _heap_check();                            \
    CheckFS(sel)                              \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)


/* ---------- 9 parameters ---------- */
#define ODINFUNCTION9(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9));    \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)

#define ODINPROCEDURE9(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9));    \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)


/* ---------- 10 parameters ---------- */
#define ODINFUNCTION10(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10));\
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)

#define ODINPROCEDURE10(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)


/* ---------- 11 parameters ---------- */
#define ODINFUNCTION11(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"  \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)); \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)

#define ODINPROCEDURE11(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"  \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)); \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)


/* ---------- 12 parameters ---------- */
#define ODINFUNCTION12(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)); \
    CheckFS(sel)                              \
    _heap_check();                            \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    _heap_check();                            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)

#define ODINPROCEDURE12(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,10,a11,a12));  \
    CheckFS(sel)                              \
    _heap_check();                            \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    _heap_check();                            \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
    SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)


#else

/****************************************************************************
 * General Wrapper Macros                                                   *
 ****************************************************************************/

/* ---------- 0 parameters ---------- */
#define ODINFUNCTION0(cRet,cName) \
  cRet ODIN_INTERNAL ODIN_##cName (void);\
  cRet WINAPI cName(void)  \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName();         \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (void)


#define ODINPROCEDURE0(cName)     \
  void ODIN_INTERNAL ODIN_##cName (void);\
  void WINAPI cName(void)  \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName();         \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (void)


/* ---------- 1 parameters ---------- */
#define ODINFUNCTION1(cRet,cName,t1,a1)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1);\
  cRet WINAPI cName(t1 a1) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1);       \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1)

#define ODINPROCEDURE1(cName,t1,a1)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1);\
  void WINAPI cName(t1 a1) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1);       \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1)


/* ---------- 2 parameters ---------- */
#define ODINFUNCTION2(cRet,cName,t1,a1,t2,a2)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);      \
  cRet WINAPI cName(t1 a1,t2 a2)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2);    \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)

#define ODINPROCEDURE2(cName,t1,a1,t2,a2)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);\
  void WINAPI cName(t1 a1,t2 a2) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2);    \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)


/* ---------- 3 parameters ---------- */
#define ODINFUNCTION3(cRet,cName,t1,a1,t2,a2,t3,a3)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)

#define ODINPROCEDURE3(cName,t1,a1,t2,a2,t3,a3)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)


/* ---------- 4 parameters ---------- */
#define ODINFUNCTION4(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)

#define ODINPROCEDURE4(cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)


/* ---------- 5 parameters ---------- */
#define ODINFUNCTION5(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)

#define ODINPROCEDURE5(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)


/* ---------- 6 parameters ---------- */
#define ODINFUNCTION6(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)

#define ODINPROCEDURE6(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)


/* ---------- 7 parameters ---------- */
#define ODINFUNCTION7(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)

#define ODINPROCEDURE7(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)


/* ---------- 8 parameters ---------- */
#define ODINFUNCTION8(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)

#define ODINPROCEDURE8(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)


/* ---------- 9 parameters ---------- */
#define ODINFUNCTION9(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)

#define ODINPROCEDURE9(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)


/* ---------- 10 parameters ---------- */
#define ODINFUNCTION10(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)

#define ODINPROCEDURE10(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)


/* ---------- 11 parameters ---------- */
#define ODINFUNCTION11(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)

#define ODINPROCEDURE11(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)


/* ---------- 12 parameters ---------- */
#define ODINFUNCTION12(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)

#define ODINPROCEDURE12(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)



#endif

/****************************************************************************
 * General Wrapper Macros                                                   *
 ****************************************************************************/

/* ---------- 0 parameters ---------- */
#define ODINFUNCTIONNODBG0(cRet,cName) \
  cRet ODIN_INTERNAL ODIN_##cName (void);\
  cRet WINAPI cName(void)  \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName();         \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (void)


#define ODINPROCEDURENODBG0(cName)     \
  void ODIN_INTERNAL ODIN_##cName (void);\
  void WINAPI cName(void)  \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName();         \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (void)


/* ---------- 1 parameters ---------- */
#define ODINFUNCTIONNODBG1(cRet,cName,t1,a1)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1);\
  cRet WINAPI cName(t1 a1) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1);       \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1)

#define ODINPROCEDURENODBG1(cName,t1,a1)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1);\
  void WINAPI cName(t1 a1) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1);       \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1)


/* ---------- 2 parameters ---------- */
#define ODINFUNCTIONNODBG2(cRet,cName,t1,a1,t2,a2)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);      \
  cRet WINAPI cName(t1 a1,t2 a2)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2);    \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)

#define ODINPROCEDURENODBG2(cName,t1,a1,t2,a2)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);\
  void WINAPI cName(t1 a1,t2 a2) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2);    \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)


/* ---------- 3 parameters ---------- */
#define ODINFUNCTIONNODBG3(cRet,cName,t1,a1,t2,a2,t3,a3)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)

#define ODINPROCEDURENODBG3(cName,t1,a1,t2,a2,t3,a3)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)


/* ---------- 4 parameters ---------- */
#define ODINFUNCTIONNODBG4(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)

#define ODINPROCEDURENODBG4(cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)


/* ---------- 5 parameters ---------- */
#define ODINFUNCTIONNODBG5(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)

#define ODINPROCEDURENODBG5(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)


/* ---------- 6 parameters ---------- */
#define ODINFUNCTIONNODBG6(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)

#define ODINPROCEDURENODBG6(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)


/* ---------- 7 parameters ---------- */
#define ODINFUNCTIONNODBG7(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)

#define ODINPROCEDURENODBG7(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)


/* ---------- 8 parameters ---------- */
#define ODINFUNCTIONNODBG8(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)

#define ODINPROCEDURENODBG8(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)


/* ---------- 9 parameters ---------- */
#define ODINFUNCTIONNODBG9(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)

#define ODINPROCEDURENODBG9(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)


/* ---------- 10 parameters ---------- */
#define ODINFUNCTIONNODBG10(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)

#define ODINPROCEDURENODBG10(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)


/* ---------- 11 parameters ---------- */
#define ODINFUNCTIONNODBG11(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)

#define ODINPROCEDURENODBG11(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)


/* ---------- 12 parameters ---------- */
#define ODINFUNCTIONNODBG12(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)

#define ODINPROCEDURENODBG12(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)




#endif /* _ODINWRAP_H_ */
