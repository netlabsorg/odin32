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

#include <os2sel.h>

/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

// override debugging
//#undef DEBUG
//#define DEBUG

// override profiling
//#undef PROFILE
//#define PROFILE



#define ODIN_INTERNAL _Optlink


#ifdef DEBUG
#  define ODINDEBUGCHANNEL(a) static char* pszOdinDebugChannel=#a;
#  define ODINDEBUGCHANNEL1(a) static char* pszOdinDebugChannel1=#a;
#else
#  define ODINDEBUGCHANNEL(a)
#  define ODINDEBUGCHANNEL1(a)
#endif


//SvL: Define this to use the internal wrapper function of a specific api
#ifdef DEBUG
#define ODIN_EXTERN(a)   ODIN_INTERNAL ODIN_##a
#define CALL_ODINFUNC(a) ODIN_##a
#else
#define ODIN_EXTERN(a)   ODIN_INTERNAL a
#define CALL_ODINFUNC(a) a
#endif


#ifdef DEBUG

//@@@PH 1999/10/25 IBM VAC++ debug memory support
#include <malloc.h>
#include <odin.h>

// ---------------------------------------------------------------------------
#ifdef __WATCOMC__
//this doens't work with watcom because doubly declared somewhere :-(
//extern unsigned long int WIN32API GetCurrentThreadId(void);     // kernel32
#else
extern unsigned long int WIN32API GetCurrentThreadId(void);     // kernel32
#endif
extern unsigned long int WIN32API dbg_GetThreadCallDepth(void); // kernel32
extern void              WIN32API dbg_IncThreadCallDepth(void); // kernel32
extern void              WIN32API dbg_DecThreadCallDepth(void); // kernel32

// ---------------------------------------------------------------------------
//SvL: Only check the heap very frequently when there are problems
//#define DEBUG_ODINHEAP
#ifdef DEBUG_ODINHEAP
#define ODIN_HEAPCHECK() _heap_check()
#else
#define ODIN_HEAPCHECK()
#endif


// ---------------------------------------------------------------------------
// PH: this is for profiling cumulative method call times
#ifdef PROFILE

#define FNINIT \
  LARGE_INTEGER liStart;       \
  LARGE_INTEGER liEnd;         \
  unsigned long ulElapsed;     \
  unsigned short sel;

#  define PROFILE_START(a)        \
     QueryPerformanceCounter(&liStart);


#  define PROFILE_STOP(a)            \
     QueryPerformanceCounter(&liEnd);\
     if (liStart.LowPart > liEnd.LowPart) \
       ulElapsed = 0xFFFFFFFF - liStart.LowPart + liEnd.LowPart; \
     else                                           \
       ulElapsed = liEnd.LowPart - liStart.LowPart; \
                                  \
     dprintf(("%s: %s %u ticks\n",\
              pszOdinDebugChannel,\
              a,                  \
              ulElapsed));
#else
#  define PROFILE_START(a)
#  define PROFILE_STOP(a)
#endif

#ifndef FNINIT
#define FNINIT \
  unsigned short sel;
#endif

#define FNPROLOGUE(a)   \
  sel = GetFS(); \
  dbg_IncThreadCallDepth(); \
  ODIN_HEAPCHECK();     \
  PROFILE_START(a)

#define FNEPILOGUE(a)   \
  PROFILE_STOP(a)       \
  ODIN_HEAPCHECK();     \
  dbg_DecThreadCallDepth(); \
  if (sel != GetFS()) { \
    SetFS(sel); \
    dprintf(("WARNING: FS: for thread %08xh corrupted by "a, GetCurrentThreadId())); \
  }


/****************************************************************************
 * General Wrapper Macros (debug instrumented)                              *
 ****************************************************************************/

/* ---------- 0 parameters ---------- */
#define ODINFUNCTION0(cRet,cName)             \
  cRet ODIN_INTERNAL ODIN_##cName (void);     \
  cRet WINAPI cName(void)                     \
  {                                           \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"()",   \
             pszOdinDebugChannel));           \
    FNPROLOGUE(#cName)                        \
    rc  = ODIN_##cName();                     \
    FNEPILOGUE(#cName)                        \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (void)


#define ODINPROCEDURE0(cName)                 \
  void ODIN_INTERNAL ODIN_##cName (void);     \
  void WINAPI cName(void)                     \
  {                                           \
    FNINIT                                    \
    dprintf(("%s: void "#cName"()",   \
             pszOdinDebugChannel));           \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName();                           \
    FNEPILOGUE(#cName)                        \
    dprintf(("%s: void "#cName"() leave\n",   \
             pszOdinDebugChannel));           \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (void)


/* ---------- 1 parameters ---------- */
#define ODINFUNCTION1(cRet,cName,t1,a1)       \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1);    \
  cRet WINAPI cName(t1 a1)                    \
  {                                           \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh)", \
             pszOdinDebugChannel,             \
             a1));                            \
    FNPROLOGUE(#cName)                        \
    rc  = ODIN_##cName(a1);                   \
    FNEPILOGUE(#cName)                        \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1)

#define ODINPROCEDURE1(cName,t1,a1)           \
  void ODIN_INTERNAL ODIN_##cName (t1 a1);    \
  void WINAPI cName(t1 a1)                    \
  {                                           \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh)", \
             pszOdinDebugChannel,             \
             a1));                            \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1);                         \
    FNEPILOGUE(#cName)                        \
    dprintf(("%s: void "#cName"() leave\n",   \
             pszOdinDebugChannel));           \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (t1 a1)


/* ---------- 2 parameters ---------- */
#define ODINFUNCTION2(cRet,cName,t1,a1,t2,a2)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2); \
  cRet WINAPI cName(t1 a1,t2 a2)               \
  {                                            \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh)", \
             pszOdinDebugChannel,              \
             a1,a2));                          \
    FNPROLOGUE(#cName)                        \
    rc  = ODIN_##cName(a1,a2);                \
    FNEPILOGUE(#cName)                        \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,              \
             rc));                             \
    return rc;                                 \
  }                                            \
                                               \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)

#define ODINPROCEDURE2(cName,t1,a1,t2,a2)     \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);     \
  void WINAPI cName(t1 a1,t2 a2)              \
  {                                           \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2));                         \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2);                      \
    FNEPILOGUE(#cName)                        \
    dprintf(("%s: void "#cName"() leave\n",   \
             pszOdinDebugChannel));           \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)


/* ---------- 3 parameters ---------- */
#define ODINFUNCTION3(cRet,cName,t1,a1,t2,a2,t3,a3)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3)        \
  {                                           \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3);            \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)

#define ODINPROCEDURE3(cName,t1,a1,t2,a2,t3,a3)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3);              \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: void "#cName"() leave\n",   \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)


/* ---------- 4 parameters ---------- */
#define ODINFUNCTION4(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh, "#t4" "#a4"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4));                   \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)

#define ODINPROCEDURE4(cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh, "#t4" "#a4"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4));                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",   \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)


/* ---------- 5 parameters ---------- */
#define ODINFUNCTION5(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5));                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)

#define ODINPROCEDURE5(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5));                \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",   \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)


/* ---------- 6 parameters ---------- */
#define ODINFUNCTION6(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6));             \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)

#define ODINPROCEDURE6(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6));             \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)


/* ---------- 7 parameters ---------- */
#define ODINFUNCTION7(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7));          \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)

#define ODINPROCEDURE7(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7));          \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)


/* ---------- 8 parameters ---------- */
#define ODINFUNCTION8(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8));       \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)

#define ODINPROCEDURE8(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8));       \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)


/* ---------- 9 parameters ---------- */
#define ODINFUNCTION9(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9));    \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)

#define ODINPROCEDURE9(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9));    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)


/* ---------- 10 parameters ---------- */
#define ODINFUNCTION10(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10));\
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)

#define ODINPROCEDURE10(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh" \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3));                      \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)


/* ---------- 11 parameters ---------- */
#define ODINFUNCTION11(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"  \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)); \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)

#define ODINPROCEDURE11(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"  \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)); \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)


/* ---------- 12 parameters ---------- */
#define ODINFUNCTION12(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)); \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)

#define ODINPROCEDURE12(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12));  \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)


/* ---------- 13 parameters ---------- */
#define ODINFUNCTION13(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh, "#t13" "#a13"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)); \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)

#define ODINPROCEDURE13(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh, "#t13" "#a13"=%08xh, )", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13));  \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)


/* ---------- 14 parameters ---------- */
#define ODINFUNCTION14(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13,t14,a14)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    dprintf(("%s: "#cRet" "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh, "#t13" "#a13"=%08xh, "#t14" "#a14"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)); \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14); \
    FNEPILOGUE(#cName)            \
    dprintf(("%s: "#cRet" "#cName"() leave = %08xh\n", \
             pszOdinDebugChannel,             \
             rc));                            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)

#define ODINPROCEDURE14(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13,t14,a14)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)    \
  {                               \
    FNINIT                                    \
    dprintf(("%s: void "#cName"("#t1" "#a1"=%08xh, "#t2" "#a2"=%08xh, "#t3" "#a3"=%08xh)" \
             ", "#t4" "#a4"=%08xh, "#t5" "#a5"=%08xh, "#t6" "#a6"=%08xh, "#t7" "#a7"=%08xh"     \
             ", "#t8" "#a8"=%08xh, "#t9" "#a9"=%08xh, "#t10" "#a10"=%08xh, "#t11" "#a11"=%08xh" \
             ", "#t12" "#a12"=%08xh, "#t13" "#a13"=%08xh, "#t14" "#a14"=%08xh)", \
             pszOdinDebugChannel,             \
             a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14));  \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14); \
    FNEPILOGUE(#cName)       \
    dprintf(("%s: void "#cName"() leave\n",    \
             pszOdinDebugChannel));           \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)


#else

/****************************************************************************
 * General Wrapper Macros                                                   *
 ****************************************************************************/

#define FNINIT          \
  unsigned short sel;

#define FNPROLOGUE(a)   \
  sel = GetFS();

#define FNEPILOGUE(a)   \
  SetFS(sel);

#define ODINFUNCTION0   ODINFUNCTIONNODBG0
#define ODINFUNCTION1   ODINFUNCTIONNODBG1
#define ODINFUNCTION2   ODINFUNCTIONNODBG2
#define ODINFUNCTION3   ODINFUNCTIONNODBG3
#define ODINFUNCTION4   ODINFUNCTIONNODBG4
#define ODINFUNCTION5   ODINFUNCTIONNODBG5
#define ODINFUNCTION6   ODINFUNCTIONNODBG6
#define ODINFUNCTION7   ODINFUNCTIONNODBG7
#define ODINFUNCTION8   ODINFUNCTIONNODBG8
#define ODINFUNCTION9   ODINFUNCTIONNODBG9
#define ODINFUNCTION10  ODINFUNCTIONNODBG10
#define ODINFUNCTION11  ODINFUNCTIONNODBG11
#define ODINFUNCTION12  ODINFUNCTIONNODBG12
#define ODINFUNCTION13  ODINFUNCTIONNODBG13
#define ODINFUNCTION14  ODINFUNCTIONNODBG14

#define ODINPROCEDURE0   ODINPROCEDURENODBG0
#define ODINPROCEDURE1   ODINPROCEDURENODBG1
#define ODINPROCEDURE2   ODINPROCEDURENODBG2
#define ODINPROCEDURE3   ODINPROCEDURENODBG3
#define ODINPROCEDURE4   ODINPROCEDURENODBG4
#define ODINPROCEDURE5   ODINPROCEDURENODBG5
#define ODINPROCEDURE6   ODINPROCEDURENODBG6
#define ODINPROCEDURE7   ODINPROCEDURENODBG7
#define ODINPROCEDURE8   ODINPROCEDURENODBG8
#define ODINPROCEDURE9   ODINPROCEDURENODBG9
#define ODINPROCEDURE10  ODINPROCEDURENODBG10
#define ODINPROCEDURE11  ODINPROCEDURENODBG11
#define ODINPROCEDURE12  ODINPROCEDURENODBG12
#define ODINPROCEDURE13  ODINPROCEDURENODBG13
#define ODINPROCEDURE14  ODINPROCEDURENODBG14

#endif

/****************************************************************************
 * General Wrapper Macros                                                   *
 ****************************************************************************/


/* ---------- 0 parameters ---------- */
#define ODINFUNCTIONNODBG0(cRet,cName)             \
  cRet ODIN_INTERNAL ODIN_##cName (void);     \
  cRet WINAPI cName(void)                     \
  {                                           \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                        \
    rc  = ODIN_##cName();              \
    FNEPILOGUE(#cName)                        \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (void)


#define ODINPROCEDURENODBG0(cName)                 \
  void ODIN_INTERNAL ODIN_##cName (void);     \
  void WINAPI cName(void)                     \
  {                                           \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName();                           \
    FNEPILOGUE(#cName)                        \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (void)


/* ---------- 1 parameters ---------- */
#define ODINFUNCTIONNODBG1(cRet,cName,t1,a1)       \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1);    \
  cRet WINAPI cName(t1 a1)                    \
  {                                           \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1);            \
    FNEPILOGUE(#cName)                        \
    return rc;                                \
  }                                           \
                                              \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1)

#define ODINPROCEDURENODBG1(cName,t1,a1)           \
  void ODIN_INTERNAL ODIN_##cName (t1 a1);    \
  void WINAPI cName(t1 a1)                    \
  {                                           \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1);                         \
    FNEPILOGUE(#cName)                        \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (t1 a1)


/* ---------- 2 parameters ---------- */
#define ODINFUNCTIONNODBG2(cRet,cName,t1,a1,t2,a2)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2); \
  cRet WINAPI cName(t1 a1,t2 a2)               \
  {                                            \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                        \
    rc  = ODIN_##cName(a1,a2);          \
    FNEPILOGUE(#cName)                         \
    return rc;                                 \
  }                                            \
                                               \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)

#define ODINPROCEDURENODBG2(cName,t1,a1,t2,a2)     \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2);     \
  void WINAPI cName(t1 a1,t2 a2)              \
  {                                           \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2);                      \
    FNEPILOGUE(#cName)                        \
  }                                           \
                                              \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2)


/* ---------- 3 parameters ---------- */
#define ODINFUNCTIONNODBG3(cRet,cName,t1,a1,t2,a2,t3,a3)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3)        \
  {                                           \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3);      \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)

#define ODINPROCEDURENODBG3(cName,t1,a1,t2,a2,t3,a3)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3);              \
    FNEPILOGUE(#cName)            \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3)


/* ---------- 4 parameters ---------- */
#define ODINFUNCTIONNODBG4(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)

#define ODINPROCEDURENODBG4(cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4); \
  FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4)


/* ---------- 5 parameters ---------- */
#define ODINFUNCTIONNODBG5(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)

#define ODINPROCEDURENODBG5(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)


/* ---------- 6 parameters ---------- */
#define ODINFUNCTIONNODBG6(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)

#define ODINPROCEDURENODBG6(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)


/* ---------- 7 parameters ---------- */
#define ODINFUNCTIONNODBG7(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)

#define ODINPROCEDURENODBG7(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)


/* ---------- 8 parameters ---------- */
#define ODINFUNCTIONNODBG8(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)

#define ODINPROCEDURENODBG8(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)


/* ---------- 9 parameters ---------- */
#define ODINFUNCTIONNODBG9(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)

#define ODINPROCEDURENODBG9(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9)


/* ---------- 10 parameters ---------- */
#define ODINFUNCTIONNODBG10(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)

#define ODINPROCEDURENODBG10(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10)


/* ---------- 11 parameters ---------- */
#define ODINFUNCTIONNODBG11(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)

#define ODINPROCEDURENODBG11(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11)


/* ---------- 12 parameters ---------- */
#define ODINFUNCTIONNODBG12(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)

#define ODINPROCEDURENODBG12(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12)


/* ---------- 13 parameters ---------- */
#define ODINFUNCTIONNODBG13(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)

#define ODINPROCEDURENODBG13(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13)


/* ---------- 14 parameters ---------- */
#define ODINFUNCTIONNODBG14(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13,t14,a14)  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14);      \
  cRet WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)        \
  {                               \
    FNINIT                                    \
    cRet   rc;                                \
    FNPROLOGUE(#cName)                       \
    rc  = ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14); \
    FNEPILOGUE(#cName)            \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)

#define ODINPROCEDURENODBG14(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8,t9,a9,t10,a10,t11,a11,t12,a12,t13,a13,t14,a14)  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14);  \
  void WINAPI cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)    \
  {                               \
    FNINIT                                    \
    FNPROLOGUE(#cName)                       \
    ODIN_##cName(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14); \
    FNEPILOGUE(#cName)       \
  }                               \
                                  \
  void ODIN_INTERNAL ODIN_##cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8,t9 a9,t10 a10,t11 a11,t12 a12,t13 a13,t14 a14)

#endif /* _ODINWRAP_H_ */
