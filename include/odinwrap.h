/* $Id: odinwrap.h,v 1.1 1999-08-11 12:46:00 phaller Exp $ */

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


/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

#define ODIN_INTERNAL _Optlink


/****************************************************************************
 * General Wrapper Macros                                                   *
 ****************************************************************************/

/* ---------- 0 parameters ---------- */
#define ODINFUNCTION0(cRet,cName) \
  cRet ODIN_INTERNAL cName (void);\
  cRet WINAPI ODIN_##cName(void)  \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName();         \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (void)


#define ODINPROCEDURE0(cName)     \
  void ODIN_INTERNAL cName (void);\
  void WINAPI ODIN_##cName(void)  \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName();         \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (void)


/* ---------- 1 parameters ---------- */
#define ODINFUNCTION1(cRet,cName,t1,a1)  \
  cRet ODIN_INTERNAL cName (t1 a1);\
  cRet WINAPI ODIN_##cName(t1 a1) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1);       \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1)

#define ODINPROCEDURE1(cName,t1,a1)  \
  void ODIN_INTERNAL cName (t1 a1);\
  void WINAPI ODIN_##cName(t1 a1) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1);       \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1)


/* ---------- 2 parameters ---------- */
#define ODINFUNCTION2(cRet,cName,t1,a1,t2,a2)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2);    \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2)

#define ODINPROCEDURE2(cName,t1,a1,t2,a2)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2);\
  void WINAPI ODIN_##cName(t1 a1,t2 a2) \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2);    \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2)


/* ---------- 3 parameters ---------- */
#define ODINFUNCTION3(cRet,cName,t1,a1,t2,a2,t3,a3)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2,a3); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3)

#define ODINPROCEDURE3(cName,t1,a1,t2,a2,t3,a3)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3);  \
  void WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2,a3); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3)


/* ---------- 4 parameters ---------- */
#define ODINFUNCTION4(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2,a3,a4); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4)

#define ODINPROCEDURE4(cName,t1,a1,t2,a2,t3,a3,t4,a4)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4);  \
  void WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2,a3,a4); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4)


/* ---------- 5 parameters ---------- */
#define ODINFUNCTION5(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2,a3,a4,a5); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)

#define ODINPROCEDURE5(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5);  \
  void WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2,a3,a4,a5); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5)


/* ---------- 6 parameters ---------- */
#define ODINFUNCTION6(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2,a3,a4,a5,a6); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)

#define ODINPROCEDURE6(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6);  \
  void WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2,a3,a4,a5,a6); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6)


/* ---------- 7 parameters ---------- */
#define ODINFUNCTION7(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2,a3,a4,a5,a6,a7); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)

#define ODINPROCEDURE7(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7);  \
  void WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2,a3,a4,a5,a6,a7); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7)


/* ---------- 8 parameters ---------- */
#define ODINFUNCTION8(cRet,cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);      \
  cRet WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)        \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
    cRet   rc  = cName(a1,a2,a3,a4,a5,a6,a7,a8); \
    SetFS(sel);                   \
    return rc;                    \
  }                               \
                                  \
  cRet ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4,a4,t5 a5,t6 a6,t7 a7,t8 a8)

#define ODINPROCEDURE8(cName,t1,a1,t2,a2,t3,a3,t4,a4,t5,a5,t6,a6,t7,a7,t8,a8)  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8);  \
  void WINAPI ODIN_##cName(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)    \
  {                               \
    unsigned short sel = RestoreOS2FS();  \
                 cName(a1,a2,a3,a4,a5,a6,a7,a8); \
                 SetFS(sel);      \
  }                               \
                                  \
  void ODIN_INTERNAL cName (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5,t6 a6,t7 a7,t8 a8)



#endif /* _ODINWRAP_H_ */
