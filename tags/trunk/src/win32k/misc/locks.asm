; $Id: locks.asm,v 1.2 2001-01-21 07:55:28 bird Exp $
;
; R/W spinlocks.
;
; Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;

    .486p

;
;   Include files
;
    include devsegdf.inc


;
;   Exported symbols
;
    public RWLockAcquireRead
    public RWLockReleaseRead
    public RWLockAcquireWrite
    public RWLockReleaseWrite


;
;   Defined Constants And Macros
;
ifdef DEBUG
DEBUGINT3 EQU int 3
else
DEBUGINT3 EQU ;
endif



CODE32 segment


;;
; Acquires Read access to a R/W lock.
; @cproto   APIRET _Optlink RWLockAcquireRead(PRWLOCK pLock)
; @returns  NO_ERROR (never failes)
; @param    eax     pLock - pointer to lock structure.
; @uses     eax, ecx, edx
; @sketch   spin while write locked.
;           try increment.
;           if failed restart spin loop.
;           else return successfully.
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark   This is non-blocking.
RWLockAcquireRead proc near
    ASSUME ds:FLAT
    mov     edx, eax                    ; edx = lock pointer.
                                        ; eax will be used later.
rwlar_writeloop:
    mov     eax, dword ptr [edx]        ; get lock data.
rwlar_reloop:
    bt      eax, 1fh                    ; test if write bit is set.
    jnc     rwlar_tryinc                ; spin until it's cleared.
    nop                                 ; We're not in a hurry any longer.
    nop
    nop
    jmp rwlar_writeloop                 ; spin.

rwlar_tryinc:
    les     ecx, [eax+1]                ; ecx <- new read lock count (eax + 1)
    lock cmpxchg dword ptr [edx], ecx
    jz      rwlar_ret                   ; ZF set ok success.
    jmp     rwlar_reloop                ; ZF clear on error, eax is [edx] - restart spin loop.

rwlar_ret:
    xor     eax, eax
    ret                                 ; Return successfully.
RWLockAcquireRead endp


;;
; Releases a read lock.
; @cproto   APIRET _Optlink RWLockReleaseRead(PRWLOCK pLock)
; @returns  NO_ERROR on success.
;           -1 on error.
; @param    eax     pLock - Pointer to the lock structure.
; @uses     eax, edx
; @sketch   If the lock count is larger than zero Then
;               Descrement it and return successfully.
;           else
;               Fail.
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
RWLockReleaseRead proc near
    mov     edx, dword ptr [eax]        ; Get lock count.
    and     edx, 7fffffffh              ; Sanity check.
    jnz     rwlrr_ok                    ; Jump if sane.

rwlrr_error:                            ; We're insane.
    DEBUGINT3                           ; This should _never_ _ever_ happen!
    mov     eax, 0ffffffffh             ; Return failure.
    ret

rwlrr_ok:
    lock dec dword ptr [eax]            ; Decrement the read count (works with write bit set too).
    xor     eax, eax                    ; Return successfully.
    ret
RWLockReleaseRead endp



;;
; Acquire write access to a R/W lock.
; @cproto   APIRET _Optlink RWLockAcquireWrite(PRWLOCK pLock)
; @returns  NO_ERROR (never failes)
; @param    eax     pLock - Pointer to lock structure.
; @uses     eax, edx
; @sketch   Spin while trying to set write lock flag (bit 31).
;           Spin while still read locked. (until lower 31 bits is zero)
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark   Will never block.
RWLockAcquireWrite proc near
    ASSUME ds:FLAT

rwlaw_writeloop:
    lock bts dword ptr [eax], 01fh      ; Try set the write bit.
    jnc  rwlaw_readloop                 ; break loop - we set the write bit.
    nop                                 ; We're not in a hurry any longer.
    nop
    nop
    jmp rwlaw_writeloop                 ; Spin

; Wait for reads to finish.
rwlaw_readloop:
    mov     edx, dword ptr [eax]        ; Get lock value.
    bt      edx, 01fh                   ; Sanity check - Ensure that the write bit is not cleared.
    jc      rwlaw_ok                    ; Jump if sane.

rwlaw_error:                            ; We're insane!
    DEBUGINT3                           ; This should _never_ _ever_ happen!
    mov     eax, 0ffffffffh             ; Return failure.
    ret

rwlaw_ok:
    and     edx, 7fffffffh              ; Spin while until read lock count is 0.
    jz      rwlaw_ret                   ; break loop
    nop                                 ; We're not in a hurry any longer.
    nop
    nop
    jmp     rwlaw_ret                   ; Spin.

rwlaw_ret:
    xor     eax, eax                    ; successful return.
    ret
RWLockAcquireWrite endp


;;
; Releases a Write lock.
; @cproto   APIRET _Optlink RWLockReleaseWrite(PRWLOCK pLock)
; @returns  NO_ERROR on success.
;           -1 on error.
; @param    eax     pLock - pointer to the lock structure.
; @uses     eax, edx
; @sketch   If write bit is set Then
;               Clear entire lock and return succesfully.
;           else
;               Return failure.
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
RWLockReleaseWrite proc near
    cmp     dword ptr [eax], 80000000h  ; Sanity check - state should be write locked and no read locks.
    je      rwlrw_ok                    ; Jump if sane.

rwlrw_error:
    DEBUGINT3                           ; This should _never_ _ever_ happen!
    mov     eax, 0ffffffffh             ; Return failure.
    ret

rwlrw_ok:
    xor     edx, edx
    lock xchg edx, [eax]                ; Write 0 to the entire lock dword.

rwlrw_ret:
    xor     eax, eax                    ; Return successfully.
    ret
RWLockReleaseWrite endp



CODE32 ends

end
