#ifndef __DBBWRAP_H__
#define __DBBWRAP_H__

#include <odinwrap.h>

#define NO_CALLER
#ifndef NO_CALLER
#define DBGWRAP_CALLER_FMT  "%08x "
#define DBGWRAP_CALLER_ARG  ((unsigned *)&(arg1))[-1],
#else
#define DBGWRAP_CALLER_FMT
#define DBGWRAP_CALLER_ARG
#endif

typedef DWORD (* WIN32API DBG_WINPROC0)();
typedef DWORD (* WIN32API DBG_WINPROC4)(DWORD);
typedef DWORD (* WIN32API DBG_WINPROC4_NORET)(DWORD);
typedef DWORD (* WIN32API DBG_WINPROC8)(DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC12)(DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC16)(DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC20)(DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC24)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC28)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC32)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC36)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC40)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC44)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC48)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC52)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
typedef DWORD (* WIN32API DBG_WINPROC56)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);


#define DEBUGWRAP0(a) \
DWORD WIN32API Dbg##a()             \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_MODULE": %s", #a));         \
    dbg_ThreadPushCall(#a); \
    ret = (DWORD)a();         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_MODULE": %s returned %x", #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP0_NORET(a) \
void WIN32API Dbg##a()             \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_MODULE": %s", #a));         \
    dbg_ThreadPushCall(#a); \
    a();         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_MODULE": %s returned (void)\n", #a));  \
}

#define DEBUGWRAP4(a) \
DWORD WIN32API Dbg##a(DWORD arg1)             \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x\n", DBGWRAP_CALLER_ARG #a, arg1));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC4)a)(arg1);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP4_NORET(a) \
void WIN32API Dbg##a(DWORD arg1)             \
{                                          \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x\n", DBGWRAP_CALLER_ARG #a, arg1));         \
    dbg_ThreadPushCall(#a); \
    ((DBG_WINPROC4_NORET)a)(arg1);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned void\n", DBGWRAP_CALLER_ARG #a));  \
}

#define DEBUGWRAP8(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC8)a)(arg1, arg2);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP8_NORET(a) \
void WIN32API Dbg##a(DWORD arg1, DWORD arg2) \
{                                          \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2));         \
    dbg_ThreadPushCall(#a); \
    ((DBG_WINPROC8)a)(arg1, arg2);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned void\n", DBGWRAP_CALLER_ARG #a)); \
}

#define DEBUGWRAP12(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC12)a)(arg1, arg2, arg3);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP16(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC16)a)(arg1, arg2, arg3, arg4);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP20(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC20)a)(arg1, arg2, arg3, arg4, arg5);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP24(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC24)a)(arg1, arg2, arg3, arg4, arg5, arg6);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP28(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC28)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP32(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC32)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP36(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC36)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP40(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC40)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP44(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC44)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP48(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC48)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP52(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC52)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP56(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13, DWORD arg14) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC56)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

//level 2 logging

#define DEBUGWRAP_LVL2_0(a) \
DWORD WIN32API Dbg##a()             \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_MODULE": %s\n", #a));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC0)a)();         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_MODULE": %s returned %x\n", #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_4(a) \
DWORD WIN32API Dbg##a(DWORD arg1)             \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x\n", DBGWRAP_CALLER_ARG #a, arg1));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC4)a)(arg1);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_8(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC8)a)(arg1, arg2);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_12(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC12)a)(arg1, arg2, arg3);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_16(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC16)a)(arg1, arg2, arg3, arg4);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_20(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC20)a)(arg1, arg2, arg3, arg4, arg5);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_24(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC24)a)(arg1, arg2, arg3, arg4, arg5, arg6);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_28(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC28)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_32(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC32)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_36(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC36)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_40(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC40)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_44(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC44)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_48(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12) \
{                                          \
    DWORD ret;                             \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC48)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define DEBUGWRAP_LVL2_52(a) \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13) \
{         \
    DWORD ret;                                 \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC52)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);         \
    dbg_ThreadPopCall(); \
    dprintf2((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret; \
}


//level 1 logging without function definitions

#define NODEF_DEBUGWRAP0(a) \
DWORD WIN32API a();  \
DWORD WIN32API Dbg##a()             \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_MODULE": %s\n", #a));         \
    dbg_ThreadPushCall(#a); \
    ret = (DWORD)a();         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_MODULE": %s returned %x\n", #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP0_NORET(a) \
void WIN32API a();  \
void WIN32API Dbg##a()             \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_MODULE": %s\n", #a));         \
    dbg_ThreadPushCall(#a); \
    a();         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_MODULE": %s returned (void)\n", #a));  \
}

#define NODEF_DEBUGWRAP4(a) \
DWORD WIN32API a(DWORD arg1);             \
DWORD WIN32API Dbg##a(DWORD arg1)             \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x\n", DBGWRAP_CALLER_ARG #a, arg1));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC4)a)(arg1);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP4_NORET(a) \
void WIN32API a(DWORD arg1);             \
void WIN32API Dbg##a(DWORD arg1)             \
{                                          \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x\n", DBGWRAP_CALLER_ARG #a, arg1));         \
    dbg_ThreadPushCall(#a); \
    ((DBG_WINPROC4_NORET)a)(arg1);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned (void)\n", DBGWRAP_CALLER_ARG #a));  \
}

#define NODEF_DEBUGWRAP8(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC8)a)(arg1, arg2);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP12(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC12)a)(arg1, arg2, arg3);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP16(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC16)a)(arg1, arg2, arg3, arg4);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP20(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC20)a)(arg1, arg2, arg3, arg4, arg5);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP24(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC24)a)(arg1, arg2, arg3, arg4, arg5, arg6);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP28(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC28)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP32(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC32)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP36(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC36)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP40(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC40)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP44(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC44)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP48(a) \
DWORD WIN32API a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12) \
{                                          \
    DWORD ret;                             \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC48)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret;                            \
}

#define NODEF_DEBUGWRAP52(a) \
DWORD WIN32API #a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13); \
DWORD WIN32API Dbg##a(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5, DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13) \
{         \
    DWORD ret;                                 \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s %x %x %x %x %x %x %x %x %x %x %x %x %x\n", DBGWRAP_CALLER_ARG #a, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));         \
    dbg_ThreadPushCall(#a); \
    ret = ((DBG_WINPROC52)a)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);         \
    dbg_ThreadPopCall(); \
    dprintf((DBGWRAP_CALLER_FMT DBGWRAP_MODULE": %s returned %x\n", DBGWRAP_CALLER_ARG #a, ret)); \
    return ret; \
}

#endif

