/* $Id: odinwrap.h,v 1.6 1999-08-17 16:58:19 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * ODIN FS: Selector function wrapper macros
 *
 * Copyright 1999 Patrick Haller
 *
 */


#ifndef _ODINWRAP_H_
#define _ODINWARP_H_

#include <os2sel.h>

/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

#define ODIN_INTERNAL _Optlink _Export


#ifdef DEBUG
#  define ODINDEBUGCHANNEL(a) static char* pszOdinDebugChannel=#a;
#else
#  define ODINDEBUGCHANNEL(a)
#endif


#ifdef DEBUG

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
    cRet   rc  = cName();                     \
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
    ODIN_##cName();                                  \
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
    cRet   rc  = ODIN_##cName(a1);                   \
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
    ODIN_##cName(a1);                                \
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
    cRet   rc  = ODIN_##cName(a1,a2);                \
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
    ODIN_##cName(a1,a2);                             \
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
    cRet   rc  = ODIN_##cName(a1,a2,a3); \
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
    ODIN_##cName(a1,a2,a3);              \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh, "#t4" "#a4"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4));                   \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4); \
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
    ODIN_##cName(a1,a2,a3,a4); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5));                \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5); \
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
    ODIN_##cName(a1,a2,a3,a4,a5); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6); \
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
             a1,a2,a3));                      \
    ODIN_##cName(a1,a2,a3,a4,a5,a6); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
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
             a1,a2,a3));                      \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
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
             a1,a2,a3));                      \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
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
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
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
             a1,a2,a3));                      \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
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
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"  \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
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
             a1,a2,a3));                      \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
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
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh) enter\n", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    cRet   rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
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
             a1,a2,a3));                      \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
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
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4,a4,t5 a5,t6 a6,t7 a7,t8 a8)

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
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4,a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)

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
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4,a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)

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
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4,a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)

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
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4,a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)

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

#endif /* _ODINWRAP_H_ */
