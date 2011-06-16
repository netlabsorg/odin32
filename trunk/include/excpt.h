/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Compiler-level Win32 SEH support for OS/2
 *
 * Copyright 2010 Dmitriy Kuminov
 */

/*
 * NOTE: This __try/__except and __try/__finally/__leave implementation is not
 * backed up by the low level compiler support and therefore the following
 * limitations exist comparing to the MSVC implementation (breaking them will
 * crash the application):
 *
 * 1. You cannot use the return statement within __try or __except or __finally
 *    blocks.
 *
 * 2. You cannot use the goto statement or the longjmp() function within __try
 *    or __except or __finally blocks if it passes control outside these blocks.
 *
 * 2. If you use __try and friends inside a do/while/for/switch block, you will
 *    lose the meaning of break and continue statements and must not use them.
 *
 * 3. The scopes of C and C++ exception blocks may not overlap (i.e. you cannot
 *    use try/catch inside __try/__except and vice versa).
 *
 * 4. There may be some other (yet unknown) limitations.
 *
 * Fortunately, in most cases, these limitations may be worked around by
 * slightly changing the original source code.
 */

#ifndef __EXCPT_H__
#define __EXCPT_H__

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__)

struct ___seh_EXCEPTION_FRAME;
typedef int (*__seh_PEXCEPTION_HANDLER)(PEXCEPTION_RECORD,
                                        struct ___seh_EXCEPTION_FRAME *,
                                        PCONTEXT, PVOID);

#pragma pack(1)

typedef struct ___seh_EXCEPTION_FRAME
{
    /* + 0 */ struct ___seh_EXCEPTION_FRAME *pPrev;
    /* + 4 */ __seh_PEXCEPTION_HANDLER pHandler;
    /* + 8 */ void *pFilterCallback;
    /* +12 */ void *pHandlerCallback;
    /* +16 */ void *pHandlerContext;
    /* +20 */ int filterResult;
    /* +24 */ DWORD EBX;
    /* +28 */ DWORD ESI;
    /* +32 */ DWORD EDI;
    /* +36 */ DWORD EBP;
    /* +40 */ DWORD ESP;
    /* +44 */ DWORD pPrevFrameOS2;
    /* +48 */ EXCEPTION_POINTERS Pointers;
    /* +56 */ int state;
    /* +60 */ DWORD pPrevFrameWin32;
    /* +64 */ DWORD Win32FS;
}
__seh_EXCEPTION_FRAME;

#pragma pack()

extern int __seh_handler(PEXCEPTION_RECORD pRec,
                         struct ___seh_EXCEPTION_FRAME *pFrame,
                         PCONTEXT pContext, PVOID pVoid);

#define _exception_code() (__seh_frame.Pointers.ExceptionRecord->ExceptionCode)
#define _exception_info() ((void *)&__seh_frame.Pointers)

#define exception_code _exception_code
#define exception_info (PEXCEPTION_POINTERS)_exception_info

#define GetExceptionCode _exception_code
#define GetExceptionInformation (PEXCEPTION_POINTERS)_exception_info

#define __try \
    volatile __seh_EXCEPTION_FRAME __seh_frame;                                \
    __seh_frame.pHandler = __seh_handler;                                      \
    __seh_frame.Pointers.ExceptionRecord = NULL;                               \
    __seh_frame.Pointers.ContextRecord = NULL;                                 \
    __seh_frame.state = 0;                                                     \
    /* install exception handler, both Win32 and OS/2 chains (note: EDX is in  \
     * clobber too since it may carry any value when we jump back to           \
     * pFilterCallback from the handler */                                     \
    __asm__ ("leal %0, %%ecx; "                                                \
             "movl %%fs, %%eax; "                                              \
             "andl $0x0000FFFF, %%eax; "                                       \
             "movl %%eax, 64(%%ecx); "                                         \
             "movl %%fs:0, %%eax; "                                            \
             "movl %%eax, 0(%%ecx); "                                          \
             "movl %%eax, 60(%%ecx); "                                         \
             "movl $0f, 8(%%ecx); "                                            \
                                                                               \
             "movl %%ebx, 24(%%ecx); "                                         \
             "movl %%esi, 28(%%ecx); "                                         \
             "movl %%edi, 32(%%ecx); "                                         \
             "movl %%ebp, 36(%%ecx); "                                         \
             "movl %%esp, 40(%%ecx); "                                         \
                                                                               \
             "pushl %%fs; "                                                    \
             "pushl $Dos32TIB; "                                               \
             "popl %%fs; "                                                     \
             "movl %%fs:0, %%eax; "                                            \
             "movl %%eax, 44(%%ecx); "                                         \
             "movl %%ecx, %%fs:0; "                                            \
             "popl %%fs; "                                                     \
                                                                               \
             "movl %%ecx, %%fs:0; "                                            \
                                                                               \
             "\n0: /* pFilterCallback */ \n"                                   \
                                                                               \
             : : "m" (__seh_frame)                                             \
             : "%eax", "%ecx", "%edx");                                        \
    for (; __seh_frame.state <= 3; ++__seh_frame.state)                        \
        if (__seh_frame.state == 0)                                            \
        {                                                                      \
            {

#define __except(filter_expr) \
            }                                                                  \
            /* cause the next state to be 3 */                                 \
            __seh_frame.state = 2;                                             \
        }                                                                      \
        else if (__seh_frame.state == 1) {                                     \
            /* execption caught, call filter expression */                     \
            __seh_frame.filterResult = (filter_expr);                          \
            __asm__("leal %0, %%ebx; jmp *%1"                                  \
                    : : "m"(__seh_frame), "m"(__seh_frame.pHandlerCallback)    \
                    : "%ebx");                                                 \
        }                                                                      \
        else if (__seh_frame.state == 3)                                       \
            /* remove exception handler (note that for some reason SMP kernel  \
             * seems to garbage the Win32FS:[0] cell with the OS/2 exception   \
             * registration record, so use the original __seh_frame value) */  \
            __asm__ ("leal %0, %%ecx; "                                        \
                                                                               \
                     "movl 64(%%ecx), %%eax; "                                 \
                     "movl %%eax, %%fs; "                                      \
                                                                               \
                     "movl 60(%%ecx), %%eax; "                                 \
                     "movl %%eax, %%fs:0; "                                    \
                                                                               \
                     "pushl %%fs; "                                            \
                     "pushl $Dos32TIB; "                                       \
                     "popl %%fs; "                                             \
                     "movl 44(%%ecx), %%eax; "                                 \
                     "movl %%eax, %%fs:0; "                                    \
                     "popl %%fs; "                                             \
                     : : "m"(__seh_frame)                                      \
                     : "%eax", "%ecx");                                        \
        else /* __seh_frame.state == 2 -> execute except block */

#define __finally \
            }                                                                  \
            /* cause the next state to be 2 */                                 \
            __seh_frame.state = 1;                                             \
        }                                                                      \
        else if (__seh_frame.state == 1) {                                     \
            /* execption caught, handle and proceed to the filally block */    \
            __seh_frame.filterResult = EXCEPTION_EXECUTE_HANDLER;              \
            __asm__("leal %0, %%ebx; jmp *%1"                                  \
                    : : "m"(__seh_frame), "m"(__seh_frame.pHandlerCallback)    \
                    : "%ebx");                                                 \
        }                                                                      \
        else if (__seh_frame.state == 3)                                       \
            /* remove exception handler (note that for some reason SMP kernel  \
             * seems to garbage the Win32FS:[0] cell with the OS/2 exception   \
             * registration record, so use the original __seh_frame value) */  \
            __asm__ ("leal %0, %%ecx; "                                        \
                                                                               \
                     "movl 64(%%ecx), %%eax; "                                 \
                     "movl %%eax, %%fs; "                                      \
                                                                               \
                     "movl 60(%%ecx), %%eax; "                                 \
                     "movl %%eax, %%fs:0; "                                    \
                                                                               \
                     "pushl %%fs; "                                            \
                     "pushl $Dos32TIB; "                                       \
                     "popl %%fs; "                                             \
                     "movl 44(%%ecx), %%eax; "                                 \
                     "movl %%eax, %%fs:0; "                                    \
                     "popl %%fs; "                                             \
                     : : "m"(__seh_frame)                                      \
                     : "%eax", "%ecx");                                        \
        else /* __seh_frame.state == 2 -> execute finally block */

#define __leave \
            /* cause the next state to be 2 */                                 \
            __seh_frame.state = 1;                                             \
            continue;

#else /* defined(__GNUC__) */

#warning "Structured exception handling is not supported for this compiler!"

#endif /* defined(__GNUC__) */

#ifdef __cplusplus
}
#endif

#endif /* __EXCPT_H__ */

