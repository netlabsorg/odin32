/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Compiler-level Win32 SEH support for OS/2
 *
 * Copyright 2010 Dmitry A. Kuminov
 */

#ifndef __EXCPT_H__
#define __EXCPT_H__

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__)

struct ___seh_PEXCEPTION_FRAME;
typedef int (*__seh_PEXCEPTION_HANDLER)(PEXCEPTION_RECORD,
                                        struct ___seh_PEXCEPTION_FRAME *,
                                        PCONTEXT, PVOID);

typedef struct ___seh_PEXCEPTION_FRAME
{
    struct ___seh_PEXCEPTION_FRAME *pPrev;
    __seh_PEXCEPTION_HANDLER pHandler;
    void *pFilterCallback;
    void *pHandlerCallback;
    void *pHandlerContext;
    int filterResult;
    DWORD pTryRegs[6]; /* EBX/ESI/EDI/EBP/ESP/ExceptionHandler */
    EXCEPTION_POINTERS Pointers;
    int state;
}
__seh_PEXCEPTION_FRAME;

int __seh_handler(PEXCEPTION_RECORD pRec,
                  struct ___seh_PEXCEPTION_FRAME *pFrame,
                  PCONTEXT pContext, PVOID pVoid);

#define _exception_code() (__seh_frame.Pointers.ExceptionRecord->ExceptionCode)
#define _exception_info() (&__seh_frame.Pointers)

#define GetExceptionCode _exception_code
#define GetExceptionInformation _exception_info

#define __try \
    volatile __seh_PEXCEPTION_FRAME __seh_frame;                               \
    __seh_frame.state = 0;                                                     \
    __asm__("\n0:\n"); /* pFilterCallback */                                   \
    for (; __seh_frame.state <= 3; ++__seh_frame.state)                        \
        if (__seh_frame.state == 0)                                            \
        {                                                                      \
            /* install exception handler */                                    \
            __asm__ ("\n.extern ___seh_handler\n"                              \
                     ""                                                        \
                     "leal %0, %%ecx; "                                        \
                     "movl %%fs:0, %%eax; "                                    \
                     "movl %%eax, 0(%%ecx); "                                  \
                     "movl $___seh_handler, %%eax; "                           \
                     "movl %%eax, 4(%%ecx); "                                  \
                     "movl $0b, 8(%%ecx); "                                    \
                     ""                                                        \
                     "movl %%ebx, 24(%%ecx); "                                 \
                     "movl %%esi, 28(%%ecx); "                                 \
                     "movl %%edi, 32(%%ecx); "                                 \
                     "movl %%ebp, 36(%%ecx); "                                 \
                     "movl %%esp, 40(%%ecx); "                                 \
                     ""                                                        \
                     "pushl %%fs; "                                            \
                     "pushl $Dos32TIB; "                                       \
                     "popl %%fs; "                                             \
                     "movl %%fs:0, %%eax; "                                    \
                     "movl %%eax, 44(%%ecx); "                                 \
                     "popl %%fs; "                                             \
                     ""                                                        \
                     "movl %%ecx, %%fs:0; "                                    \
                     : : "m" (__seh_frame)                                     \
                     : "%eax", "%ecx");                                        \
            {

#define __except(filter_expr) \
            }                                                                  \
            __seh_frame.state = 2;                                             \
        }                                                                      \
        else if (__seh_frame.state == 1) {                                     \
            __seh_frame.filterResult = (filter_expr);                          \
            __asm__("leal %0, %%ebx; jmp *%1"                                  \
                    : : "m"(__seh_frame), "m"(__seh_frame.pHandlerCallback)    \
                    : "%ebx");                                                 \
        }                                                                      \
        else if (__seh_frame.state == 3)                                       \
            /* remove exception handler */                                     \
            __asm__ ("movl %%fs:0, %%eax; "                                    \
                     "movl 0(%%eax), %%eax; "                                  \
                     "movl %%eax, %%fs:0; "                                    \
                     : :                                                       \
                     : "%eax");                                                \
        else /* __seh_frame.state == 2 */

#else /* defined(__GNUC__) */

#warning "Structured exception handling is not supported for this compiler!"

#endif /* defined(__GNUC__) */

#ifdef __cplusplus
}
#endif

#endif /* __EXCPT_H__ */

