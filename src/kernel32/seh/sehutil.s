/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Compiler-level Win32 SEH support for OS/2
 *
 * Copyright 2010 Dmitry A. Kuminov
 */

.global ___seh_handler

/*
 * int __seh_handler(PEXCEPTION_RECORD pRec,
 *                   struct ___seh_PEXCEPTION_FRAME *pFrame,
 *                   PCONTEXT pContext, PVOID)
 *
 * Win32 structured exception handler that implements the __try/__except
 * functionality for GCC.
 *
 * NOTE: This is a heavily platform specific stuff. The code depends on the
 * struct ___seh_PEXCEPTION_FRAME layout so be very careful and keep both
 * in sync!
 *
 * __cdecl: EAX/ECX/EDX are not preserved, result in EAX/EDX, caller cleans up
 * the stack.
 */
___seh_handler:

    pushl %ebp
    movl %esp, %ebp

    /*
     * 8(%ebp)  - pRec
     * 12(%ebp) - pFrame
     * 16(%ebp) - pContext
     * 20(%ebp) - pVOid
     */

    /* preserve used registers */
    pushl %ebx
    pushl %edi
    pushl %esi

    /* skip EH_UNWINDING calls (for compatibility with MSVC) */
    movl 8(%ebp), %ebx
    movl 4(%ebx), %eax /* pRec->ExceptionFlags */
    testl $0x2, %eax /* EH_UNWINDING? */
    movl $1, %eax /* ExceptionContinueSearch */
    jne ___seh_handler_Return

    /* save handler's context */
    pushl %ebp
    pushl $0   /* reserve space for length, must be saved right before ESP! */
    pushl %esp /* ESP must be saved last! */

    movl 12(%ebp), %ebx
    movl $0f, 12(%ebx) /* pFrame->pHandlerCallback */

    /* get the size of the handler's stack */
    movl 40(%ebx), %ecx /* pFrame->pTryRegs[4] is ESP */
    subl %esp, %ecx
    jle ___seh_handler_Error /* Invalid stack! */
    movl %ecx, 4(%esp) /* save length */

    /* save the handler's stack on heap */
    movl %ecx, %eax /* size_t */
    subl $16, %esp
    movl %eax, 0(%esp)
    call odin_malloc /* _Optlink, rtl, EAX/EDX/ECX-in, caller cleans stack */
    addl $16, %esp
    testl %eax, %eax
    je ___seh_handler_Error /* No memory! */
    movl 4(%esp), %ecx
    movl %eax, %edi
    movl %edi, 16(%ebx) /* pFrame->pHandlerContext */
    movl %esp, %esi
    rep movsb

    /* prepare a jump to the filter callback */
    movl 12(%ebp), %eax

    movl 8(%ebp), %ebx
    movl %ebx, 48(%eax) /* pFrame->Pointers.ExceptionRecord */
    movl 16(%ebp), %ebx
    movl %ebx, 52(%eax) /* pFrame->Pointers.ContextRecord */

    /* restore __try/__catch context */
    movl 24(%eax), %ebx /* pFrame->pTryRegs */
    movl 28(%eax), %esi
    movl 32(%eax), %edi
    movl 36(%eax), %ebp
    movl 40(%eax), %esp

    /* jump to the filter callback */
    movl $1, 56(%eax) /* pFrame->state */
    jmp *8(%eax) /* pFrame->pFilterCallback */

0:
    /* restore handler's context (we assume that the callback puts the address
     * of pFrame back to EBX!) */
    movl 16(%ebx), %esi /* pFrame->pHandlerContext */
    movl 0(%esi), %esp  /* restore saved ESP */
    movl 4(%esi), %ecx  /* saved stack length */
    subl $4, %esp /* correct ESP to compensate for PUSH ESP logic */
    movl %esp, %edi
    rep movsb

    popl %esp
    addl $4, %esp
    popl %ebp

    /* free heap block */
    movl 16(%ebx), %eax /* pFrame->pHandlerContext */
    subl $16, %esp
    movl %eax, 0(%esp)
    call odin_free /* _Optlink, rtl, EAX/EDX/ECX-in, caller cleans stack */
    addl $16, %esp

    /* analyze filter result */
    movl 20(%ebx), %eax /* pFrame->filterResult */
    cmpl $1, %eax /* EXCEPTION_EXECUTE_HANDLER? */
    je ___seh_handler_Unwind
    cmpl $-1, %eax /* EXCEPTION_CONTINUE_EXECUTION? */
    jne 1f
    movl $0, 56(%ebx) /* pFrame->state */
    movl $0, %eax /* ExceptionContinueExecution */
    jmp ___seh_handler_Return
1:
    /* Assume EXCEPTION_CONTINUE_SEARCH (0) */
    movl $1, %eax /* ExceptionContinueSearch */
    jmp ___seh_handler_Return

___seh_handler_Unwind:

    /* Unwind Win32 exception chain up to ours */
    pushl $0        /* DWORD (unused) */
    pushl 8(%ebp)   /* PEXCEPTION_RECORD */
    pushl $0        /* LPVOID (unused) */
    pushl 12(%ebp)  /* PEXCEPTION_FRAME */
    call _RtlUnwind@16 /* _stdcall, rtl, callee cleans stack */

    /* Unwind OS/2 exception chain */
    movl 12(%ebp), %eax
    pushl $0        /* PEXCEPTIONREPORTRECORD */
    pushl $1f       /* PVOID pTargetIP */
    pushl 44(%eax)  /* PEXCEPTIONREGISTRATIONRECORD */
    call _DosUnwindException /* _syscall, rtl, caller cleans stack */
1:
    addl $12, %esp

    /* restore __try/__except context */
    movl 12(%ebp), %eax
    movl 24(%eax), %ebx
    movl 28(%eax), %esi
    movl 32(%eax), %edi
    movl 36(%eax), %ebp
    movl 40(%eax), %esp

    /* jump to __except */
    movl $2, 56(%eax) /* pFrame->state */
    jmp *8(%eax) /* pFrame->pFilterCallback */

___seh_handler_Error:

    addl $8, %esp
    popl %ebp

    movl $1, %eax /* ExceptionContinueSearch */

___seh_handler_Return:

    popl %esi
    popl %edi
    popl %ebx

    popl %ebp
    ret

