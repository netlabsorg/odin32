/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Compiler-level Win32 SEH support for OS/2
 *
 * Copyright 2010 Dmitry A. Kuminov
 */

#include "excpt.h"

extern void * _Optlink odin_malloc(int);

void __seh_makePointers(__seh_PEXCEPTION_FRAME *pFrame,
                        PEXCEPTION_RECORD pRecord, PCONTEXT pContext)
{
    // make copies of Record and Context as we will temporarily
    // discard the exception handler's stack when jumping to the
    // except()'s filter function
    pFrame->Pointers.ExceptionRecord =
        (PEXCEPTION_RECORD)odin_malloc(sizeof(EXCEPTION_RECORD));
    *pFrame->Pointers.ExceptionRecord = *pRecord;
    pFrame->Pointers.ContextRecord =
        (PCONTEXT)odin_malloc(sizeof(CONTEXT));
    *pFrame->Pointers.ContextRecord = *pContext;
}
