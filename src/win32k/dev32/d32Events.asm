; $Id: d32Events.asm,v 1.2.2.1 2001-09-27 03:08:13 bird Exp $
;
; d32Events - Code for overriding system events.
;
; Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Include files
;
    include devsegdf.inc
    include devhlp.inc


;
; Exported symbols
;


;
; Externs
;
    extrn   aSysEventsOverrides:dword
    extrn   dh_SendEvent:near
    extrn   dh_SendEvent_sel:word
    extrn   RASRST:near
    extrn   _Device_Help:dword

CODE16 segment
    ASSUME CS:CODE16, DS:NOTHING, ES:NOTHING, SS:NOTHING


;;
; Function overriding the dh_SendEvent DevHelp function. If we
; are overriding the event send, we'll post the semphore registered.
; @returns  CF set on error, clear on success.
; @param    ah  Event being signalled. One of the following values (defined in devhlp.inc):
;                   event_SM_Mou       (0)   event was Session Manager (mouse)
;                   event_CtrlBrk      (1)   event was Ctrl-Break
;                   event_CtrlC        (2)   event was Ctrl-C
;                   event_CtrlScrLk    (3)   event was Ctrl-ScrollLock
;                   event_CtrlPrtSc    (4)   event was Ctrl-PrtSc
;                   event_ShftPrtSc    (5)   event was Shift-PrtSc
;                   event_SM_Kbd       (6)   event was Session Manager (keyboard)
;                   event_SM_CAD       (7)   event was Ctl-Alt-Del
;                   event_HOT_PLUG     (8)   Keyboard Hot Plug/Reset
;                   event_POWER        (9)   Power suspend event
;                   event_POWEROFF     (10)  Power off event
; @param    bx  Event argument.
; @uses
; @sketch
; @status
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark
mydh_SendEvent PROC FAR
    ; Preserve some registers.
    push    ax
    push    ds
    push    ebx

    ;
    ; Check range of the event id.
    ;
    cmp     ah, event_POWEROFF
    jg      mydse_OS_event              ; Jump if event id is greater than event_POWEROFF

    ;
    ; Check if we will handle this event or not.
    ;   This is done by checking the entry for this eventid (ah) in
    ;   aSysEventsOverrides (k32HandleSystemEvent.cpp).
    ;
    push    seg FLAT:CODE32
    pop     ds
    ASSUME  ds:FLAT
    movzx   ebx, ah
    shl     ebx, 3                      ; entry size is 8 bytes. 2^3 is 8.
    cmp     byte ptr aSysEventsOverrides[ebx+4], 0 ; check bad flag for this entry.
    jnz     mydse_ErrorReturn           ; If the entry is bad we'll fail.

    cmp     dword ptr aSysEventsOverrides[ebx], 0 ; check the hev if it's NULLHANDLE.
    jz      mydse_OS_event


    ;
    ; Post semaphore handle.
    ;
    call    PostEventSem


;
; Return to the caller. (dirty hack)
;   On the stack we have a 16-bit near return address in the
;   same segment as dh_SendEvent. To do a far return as we have to
;   do we'll have to change this into an far 16-bit pointer.
;       0. restore ebx and save bp (since we can't use sp directly).
;       1. reserve a 16-bit word. This is allready done when pushing ax in the prolog.
;       2. move near return to word from step 1. (sp+0)
;       3. get the selector of the dh_SendEvent segment.
;       4. move the selector of the dh_SendEvent segment to
;          the near return word. (sp+2)
;       5. Restore bp and ds, and return far.
mydse_ret:
    pop     ebx                         ; 0
    push    bp                          ; 0
    mov     bp, sp                      ; 0
    push    eax                         ; Save eax value (error code)
    mov     ax, ss:[bp+6]               ; Fetch return offset.
    mov     ss:[bp+4], ax               ; Write over the original ax value pushed at function start.
    push    DATA16                      ; 3
    pop     ds                          ; 3
    ASSUME  ds:DATA16                   ; 3
    mov     ax, cs:dh_SendEvent_sel     ; 3
    mov     ss:[bp+6], ax               ; 4
    pop     eax                         ; restore eax.
    pop     bp                          ; 5
    pop     ds                          ; 5
    retf                                ; 5


;
; We failed to send the signal or the hev is bad.
;
mydse_ErrorReturn:
    stc                                 ; set carry flag.
    jmp     mydse_ret


;
; Let OS/2 take care of this event.
;
mydse_OS_event:
    pop     ebx
    pop     ds
    pop     ax
    jmp     dh_SendEvent
mydh_SendEvent ENDP



;;
;
; @returns  Not expected to return, but keyboard code handles that quite well it seems.
; @param
; @uses
; @equiv
; @time
; @sketch
; @status
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark   Assumes that caller is kdbbase.sys and it is called far.
myRASRST PROC FAR
    ASSUME ds:nothing, ss:nothing

    ; Preserve some registers.
    push    ds
    push    ebx


    ;
    ; Check if we're going to override this dump request.
    ;
    push    seg FLAT:CODE32
    pop     ds
    ASSUME  ds:FLAT
    mov     ebx, 8 * 0ah                ; Entry of the Ctrl-Alt-2xNumLock event.
    cmp     byte ptr aSysEventsOverrides[ebx+4], 0 ; check bad flag for this entry.
    jnz     myRASRST_ret                ; If the entry is bad we'll fail.

    cmp     dword ptr aSysEventsOverrides[ebx], 0 ; check the hev if it's NULLHANDLE.
    jz      myRASRST_dump

    ;
    ; TODO - add check if we're actually being called from kbdbase.sys!
    ;        If not called from kbdbase.sys we should do the dump.


    ;
    ; Post semaphore handle.
    ;
    call    PostEventSem

    ;
    ; Return - here we assume return far to the keybase.sys driver.
    ;
myRASRST_ret:
    pop     ebx
    pop     ds
    retf


;
; Call the real RASRST.
;
myRASRST_dump:
    pop     ebx
    pop     ds
    jmp     RASRST
myRASRST ENDP


;;
; Internal function which posts the semaphore of a given entry.
; @returns  Return from dh_PostEventSem.
;               CF clear on success.
;               CF set on error, with errorcode in eax.
; @param    ebx     Offset of event entry to post.
; @uses     eax.
PostEventSem PROC NEAR
    push    esi                         ; Save registers - can't be too carefull... paranoia
    push    edi
    push    edx
    push    ecx
    push    ebx
    push    ebp
    push    es
    push    ds

    mov     eax, aSysEventsOverrides[ebx] ; Get hev from the entry.
    mov     dh, DevHlp_PostEventSem     ; Post event sem dev help.
    call    [_Device_Help]

    pop     ds                          ; Restore registers.
    pop     es
    pop     ebp
    pop     ebx
    pop     ecx
    pop     edx
    pop     edi
    pop     esi

    jnc     post_ok                     ; Check for errors
    ; Post returned errorcode. We'll ignore ERROR_ALREADY_POSTED and ERROR_TOO_MANY_POSTS.
    cmp     eax, 299 ;(ERROR_ALREADY_POSTED)
    je      post_ok
    cmp     eax, 298 ;(ERROR_TOO_MANY_POSTS)
    je      post_ok

    ;
    ; Failed to post semaphore - set bad flag to true.
    ;
    mov     byte ptr aSysEventsOverrides[ebx+4], 1 ; check bad flag for this entry.

post_ok:
    ret
PostEventSem ENDP


CODE16 ends


CODE32 segment
CODE32START label byte

CODE32 ends

END


