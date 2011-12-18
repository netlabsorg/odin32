/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Compiler-level Win32 SEH support for OS/2
 *
 * Copyright 2010 Dmitriy Kuminov
 */

.global ___seh_handler
.global ___seh_get_prev_frame

/*
 * extern "C"
 * int __seh_handler(PEXCEPTION_RECORD pRec,
 *                   struct ___seh_EXCEPTION_FRAME *pFrame,
 *                   PCONTEXT pContext, PVOID)
 *
 * Win32 structured exception handler that implements the __try/__except
 * functionality for GCC.
 *
 * NOTE: This is a heavily platform specific stuff. The code depends on the
 * struct ___seh_EXCEPTION_FRAME layout so be very careful and keep both
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
     * 20(%ebp) - pVoid
     */

    /* preserve used registers */
    pushl %ebx
    pushl %edi
    pushl %esi

    movl %fs, %eax
    andl $0x0000FFFF, %eax
    cmpl $Dos32TIB, %eax /* Running along the OS/2 chain? */
    jne ___seh_handler_Win32 /* No, assume the Win32 chain */

    /* Note: Unwinding is disabled here since a) it is more correct to do
     * centralized unwinding from OS2ExceptionHandler2ndLevel() (i.e. not only
     * for SEH frames) and b) it crashes under SMP kernel due to stack being
     * corrupt by the time when unwinding happens. See comments in
     * OS2ExceptionHandler2ndLevel() for more details. */

#if 0

    movl 8(%ebp), %eax
    movl 4(%eax), %eax /* fHandlerFlags */
    testl $0x02, %eax  /* EH_UNWINDING? */
    jne ___seh_handler_OS2_Unwind

    /* restore the OS/2 chain in our frame */
    movl 12(%ebp), %eax
    movl 44(%eax), %ecx /* pPrevFrameOS2 */
    movl %ecx, 0(%eax)  /* pPrev */

    xorl %eax, %eax  /* return XCPT_CONTINUE_SEARCH (0) */
    jmp ___seh_handler_Return

___seh_handler_OS2_Unwind:

    /* unwind the Win32 chain including our frame as someone's definitely
     * jumping outside it if we're being unwound by OS/2 */
    movl 12(%ebp), %eax
    cmpl $0, 64(%eax)                   /* Win32FS == 0? */
    je ___seh_handler_OS2_Unwind_End    /* Yes, we already unwound this frame */

    /* restore the Win32 chain in our frame */
    movl 60(%eax), %ebx /* pPrevFrameWin32 */
    movl %ebx, 0(%eax)  /* pPrev */

    pushl %fs

    pushl 64(%eax)  /* Win32FS */
    popl %fs

    pushl $0        /* DWORD (unused) */
    pushl $0        /* PEXCEPTION_RECORD */
    pushl $0        /* LPVOID (unused) */
    pushl %ebx      /* PEXCEPTION_FRAME */
    call _RtlUnwind@16 /* _stdcall, rtl, callee cleans stack */

    popl %fs

    /* restore the OS/2 chain in our frame */
    movl 12(%ebp), %eax
    movl 44(%eax), %ecx /* pPrevFrameOS2 */
    movl %ecx, 0(%eax)  /* pPrev */

___seh_handler_OS2_Unwind_End:

    xor %eax, %eax  /* return code is irrelevant for EH_UNWINDING */
    jmp ___seh_handler_Return

#else

    /* restore the OS/2 chain in our frame */
    movl 12(%ebp), %eax
    movl 44(%eax), %ecx /* pPrevFrameOS2 */
    movl %ecx, 0(%eax)  /* pPrev */

    xorl %eax, %eax  /* return XCPT_CONTINUE_SEARCH (0) */
    jmp ___seh_handler_Return

#endif

___seh_handler_Win32:

    /* restore the Win32 chain in our frame */
    movl 12(%ebp), %eax
    movl 60(%eax), %ecx /* pPrevFrameWin32 */
    movl %ecx, 0(%eax)  /* pPrev */

    /* skip EH_UNWINDING calls (for compatibility with MSVC) */
    movl 8(%ebp), %ebx
    movl 4(%ebx), %eax                      /* pRec->ExceptionFlags */
    testl $0x2, %eax                        /* EH_UNWINDING? */
    je ___seh_handler_Win32_NotUnwinding    /* No, continue normally */

    /* See the comment above */
#if 0
    /* clear out the Win32FS field so that we will not attempt to unwind twice
     * (first, as a result of forced unwind from ExitProcess/ExitThread/etc and
     * then from the OS/2 EH_UNWINDING call of our handler) */
    movl 12(%ebp), %eax
    movl $0, 64(%eax)   /* Win32FS */
#endif

    movl $1, %eax /* ExceptionContinueSearch */
    jmp ___seh_handler_Return

___seh_handler_Win32_NotUnwinding:

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

    /* check that EXCEPTION_RECORD and CONTEXT are on our stack
     * and save their offsets in pFrame */
    movl 8(%ebp), %eax
    subl %esp, %eax
    jl ___seh_handler_Error /* Invalid stack! */
    cmpl %ecx, %eax
    jg ___seh_handler_Error /* Invalid stack! */
    movl %eax, 48(%ebx) /* pFrame->Pointers.ExceptionRecord */

    movl 16(%ebp), %eax
    subl %esp, %eax
    jl ___seh_handler_Error /* Invalid stack! */
    cmpl %ecx, %eax
    jg ___seh_handler_Error /* Invalid stack! */
    movl %eax, 52(%ebx) /* pFrame->Pointers.ContextRecord */

    /* save the handler's stack on heap */
    movl %ecx, %eax /* size_t */
    subl $4, %esp
    movl %eax, 0(%esp)
    call _malloc /* __cdecl (and _Optlink compatible -> EAX/EDX/ECX-in) */
    addl $4, %esp
    testl %eax, %eax
    je ___seh_handler_Error /* No memory! */
    movl 4(%esp), %ecx
    movl %eax, %edi
    movl %edi, 16(%ebx) /* pFrame->pHandlerContext */
    movl %esp, %esi
    rep movsb

    /* correct Pointers offsets to point to the saved stack on heap */
    movl 16(%ebx), %eax /* pFrame->pHandlerContext */
    addl %eax, 48(%ebx) /* pFrame->Pointers.ExceptionRecord */
    addl %eax, 52(%ebx) /* pFrame->Pointers.ContextRecord */

    /* restore __try/__catch context */
    movl 12(%ebp), %eax
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
    subl $4, %esp
    movl %eax, 0(%esp)
    call _free /* __cdecl (and _Optlink compatible -> EAX/EDX/ECX-in) */
    addl $4, %esp

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
    /* assume EXCEPTION_CONTINUE_SEARCH (0) */
    movl $1, %eax /* ExceptionContinueSearch */
    jmp ___seh_handler_Return

___seh_handler_Unwind:

    /* unwind Win32 exception chain up to ours */
    pushl $0        /* DWORD (unused) */
    pushl 8(%ebp)   /* PEXCEPTION_RECORD */
    pushl $0        /* LPVOID (unused) */
    pushl 12(%ebp)  /* PEXCEPTION_FRAME */
    call _RtlUnwind@16 /* _stdcall, rtl, callee cleans stack */

    /* restore the OS/2 chain in our frame */
    movl 12(%ebp), %eax
    movl 44(%eax), %ecx /* pPrevFrameOS2 */
    movl %ecx, 0(%eax)  /* pPrev */

    /* unwind OS/2 exception chain up to ours */
    movl %fs, %ebx
    pushl $Dos32TIB
    popl %fs
    pushl $0        /* PEXCEPTIONREPORTRECORD */
    pushl $1f       /* PVOID pTargetIP */
    pushl 12(%ebp)  /* PEXCEPTIONREGISTRATIONRECORD */
    call _DosUnwindException /* _syscall, rtl, but stack is not restored! */
1:
    movl %ebx, %fs

    /* restore the Win32 chain in our frame */
    movl 12(%ebp), %eax
    movl 60(%eax), %ecx /* pPrevFrameWin32 */
    movl %ecx, 0(%eax)  /* pPrev */

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

/*
 * extern "C"
 * EXCEPTION_FRAME *__seh_get_prev_frame(EXCEPTION_FRAME *pFrame)
 *
 * Returns the previous Win32 exception frame given an existing frame.
 *
 * If SEH is used, the pFrame->Prev expression will not always return a correct
 * result: the SEH handler is installed on both the Win32 and OS/2 exception
 * chains and it dynamically substitutes the Prev field in its frame with the
 * Win32 or OS/2 tail depending on the source of the exception. This function
 * takes this into account and always returns the correct pointer to the
 * previous Win32 frame.
 *
 * NOTE: This is a heavily platform specific stuff. The code depends on the
 * struct ___seh_EXCEPTION_FRAME layout so be very careful and keep both
 * in sync!
 *
 * __cdecl: EAX/ECX/EDX are not preserved, result in EAX/EDX, caller cleans up
 * the stack.
 */

___seh_get_prev_frame:

    /*
     * 4(%esp) - pFrame
     */

    movl 4(%esp), %eax
    /*leal ___seh_handler, %ecx*/
    movl 4(%eax), %ecx /* pFrame->Handler */
    cmpl $___seh_handler, %ecx
    jne ___seh_get_prev_frame_Normal

    movl 60(%eax), %eax /* pPrevFrameWin32 */
    ret

___seh_get_prev_frame_Normal:

    movl 0(%eax), %eax /* pFrame->Prev */
    ret

