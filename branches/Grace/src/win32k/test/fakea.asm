; $Id: fakea.asm,v 1.1.4.4 2000-08-24 01:36:29 bird Exp $
;
; Fake assembly imports.
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;

    .386

;
;   Defined Constants And Macros
;
    INCL_BASE EQU 1

;
;   Include files
;
    include devsegdf.inc
    include devhlp.inc
    include os2.inc


;
;   Exported symbols
;
    public fakepTCBCur
    public fakepPTDACur
    public fakeptda_start
    public fakeptda_environ
    public fakeptda_module
    public fakef_FuStrLenZ
    public fakef_FuStrLen
    public fakef_FuBuff
    public fakeg_tkExecPgm
    public fake_tkStartProcess
    public CalltkExecPgm


;
;   Imported Functions and Variables.
;
    extrn tkExecPgmWorker:PROC          ; fake.c
    extrn _fakeLDRClearSem@0:PROC       ; fake.c
    extrn _fakeKSEMRequestMutex@8:PROC  ; fake.c
    extrn fakeLDRSem:BYTE               ; fake.c

DATA16 SEGMENT
; Fake data in 16-bit segment.
fakepTCBCur         dd      offset FLAT:fakeTCB
fakepPTDACur        dd      offset FLAT:fakeptda_start

; PTDA - Only use environ and ExecChild.
fakeptda_start              LABEL DWORD
fakeptda_pPTDAParent        dd      0
fakeptda_pPTDASelf          dd      offset FLAT:fakeptda_start
fakeptda_pPTDAFirstChild    dd      0
fakeptda_pPTDAExecChild     dd      offset FLAT:fakeptda_start
fakeptda_dummy              db  123 dup (0)
fakeptda_environ            dw      1   ; 1 is the hardcoded HOB of the win32ktst.exe's environment.
fakeptda_module             dw      1   ; 1 is the hardcoded HMTE of the current executable module.
fakeptda_pBeginLIBPATH      dd      0   ; BEGINLIBPATH not implemented.
                            dd      0   ; ENDLIBPATH not implemented.


; TCB - just needs some dummy data for reading and writing to the TCBFailErr.
fakeTCB                     db 220h dup (0CCh)

DATA16 ENDS


CODE16 SEGMENT


; Scans strings until empy-string is reached.
; input:  bx:di
; uses:   nearly all (save bp)
; return: cx size - CF clear
;         ax error- CF set
fakef_FuStrLenZ PROC FAR
    push    2                           ; required by all 16-bit far procedures.
    push    2                           ; dummy code.
    push    2                           ; dummy code.
    push    2                           ; dummy code.
    push    es

    mov     dx, di                      ; save di pointer.
    cmp     bx, 7                       ; check if NULL ptr.
    jle     ffslz_badselector

    mov     es, bx                      ; es:di -> string
    mov     cx, di
    not     cx                          ; maximum length is to segment end.
    xor     ax, ax                      ; test against zero (scasb uses al to cmp with).

ffslz_loop:
    repnz scasb
    jnz     ffslz_no_term               ; jump if cx = 0
    dec     cx                          ; check if next is NULL too
    scasb
    jnz     ffslz_loop

ffslz_retok:
    mov     cx, di
    sub     cx, dx                      ; cx <- size (end ptr - start ptr)
    pop     es
    add     sp, 8
    clc
    db      66h
    retf

ffslz_badselector:
    mov     ax, ERROR_INVALID_SELECTOR
    jmp     ffslz_reterr

;ffslz_invalidptr:
;    mov     ax, ERROR_INVALID_ACCESS
;    jmp     ffslz_reterr

ffslz_no_term:
    mov     ax, ERROR_TERMINATOR_NOT_FOUND
ffslz_reterr:
    pop     es
    add     sp, 8
    stc
    db      66h
    retf
fakef_FuStrLenZ ENDP


;
; Stringlength - Fake
; input:  bx:di
; uses:   nearly all (save bp)
; return: cx size - CF clear
;         ax error- CF set
fakef_FuStrLen PROC FAR
    push    2                           ; required by all 16-bit far procedures.
    push    2                           ; dummy code.
    push    2                           ; dummy code.
    push    2                           ; dummy code.
    push    es

    mov     dx, di                      ; save di pointer.
    cmp     bx, 7                       ; check if NULL ptr.
    jle     ffslz_badselector

    mov     es, bx                      ; es:di -> string
    mov     cx, di
    not     cx                          ; maximum length is to segment end.
    xor     ax, ax                      ; test against zero (scasb uses al to cmp with).

    repnz scasb
    jnz     ffslz_no_term               ; jump if cx = 0

ffslz_retok:
    mov     cx, di
    sub     cx, dx                      ; cx <- size (end ptr - start ptr)
    pop     es
    add     sp, 8
    clc
    db      66h
    retf

ffslz_badselector:
    mov     ax, ERROR_INVALID_SELECTOR
    jmp     ffslz_reterr

;ffslz_invalidptr:
;    mov     ax, ERROR_INVALID_ACCESS
;    jmp     ffslz_reterr

ffslz_no_term:
    mov     ax, ERROR_TERMINATOR_NOT_FOUND
ffslz_reterr:
    pop     es
    add     sp, 8
    stc
    db      66h
    retf
fakef_FuStrLen ENDP


;memcpy
;input:  bx:si pointer to source
;        es:di pointer to target
;        cx    count of bytes to copy
;uses:   nearly all (save bp), es, ds
;return: success CF clear
;        failure CF set
fakef_FuBuff PROC FAR
    push    2                           ; required by all 16-bit far procedures.
    push    2                           ; dummy code.
    push    2                           ; dummy code.
    push    2                           ; dummy code.
    push    es

    cmp     bx, 7                       ; check if NULL ptr.
    jle     ffslz_badselector
    mov     ax, es
    cmp     bx, 7                       ; check if NULL ptr.
    jle     ffslz_badselector

    mov     ds, bx                      ; ds:si -> string
    mov     ax, di
    not     ax
    cmp     ax, cx                      ; crosses segment boundrary...
    jb      ffslz_invalidptr
    mov     ax, si
    not     ax
    cmp     ax, cx                      ; crosses segment boundrary...
    jb      ffslz_invalidptr

;    movzx   esi, si
;    movzx   edi, di
;    movzx   ecx, cx
;    db      66h                         ; force it use extended registers.
    rep movsb

ffslz_retok:
    xor     ax, ax
    mov     cx, di
    sub     cx, dx                      ; cx <- size (end ptr - start ptr)
    pop     es
    add     sp, 8
    clc
    db      66h
    retf

ffslz_badselector:
    mov     ax, ERROR_INVALID_SELECTOR
    jmp     ffslz_reterr

ffslz_invalidptr:
    int     3
    mov     ax, ERROR_INVALID_ACCESS
    jmp     ffslz_reterr

ffslz_no_term:
    mov     ax, ERROR_TERMINATOR_NOT_FOUND
ffslz_reterr:
    pop     es
    add     sp, 8
    stc
    db      66h
    retf
fakef_FuBuff ENDP




CODE16 ENDS



CODE32 SEGMENT
;;
; Faker of which simply clears the loader semaphore.
; @cproto    none! (void _Optlink   fake_tkStartProcess(void))
; @returns
; @param
; @uses
; @equiv
; @time
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
fake_tkStartProcess PROC NEAR
    push    ebp
    mov     ebp, esp

    push    ebx
    push    ecx

    call    _fakeLDRClearSem@0

    pop     ecx
    pop     ebx

    xor     eax, eax
    leave
    ret
fake_tkStartProcess ENDP


;;
; Fake g_tkExecPgm implementation.
; @proto     none. (void _Optlink fakeg_tkExecPgm(void);)
; @returns   same as tkExecPgm: eax, edx and carry flag
; @param     ax     Exec flag
;            ds:dx  Filename address. (String)
;            es:bx  Environment address. (String)
;            di:si  Argument address. (String)
; @uses      all - bp
; @sketch    Copy the filename and arguments into a buffer we
;            may modify later if this is a UNIX shellscript or
;            a PE-file started by pe.exe.
; @status    completely implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
;
;
fakeg_tkExecPgm PROC NEAR
    push    ebp
    mov     ebp, esp

    ;
    ; Call C worker
    ;
    sub     esp, 10h
    movzx   eax, ax
    mov     [esp + 00h], eax            ; ExecFlag DWORD

    mov     ax, es
    SelToFlat
    mov     [esp + 08h], eax            ; Environment ptr.
    mov     ecx, eax

    mov     ax, ds
    mov     bx, dx
    SelToFlat
    mov     [esp + 0ch], eax            ; Filename ptr

    mov     ax, di
    mov     bx, si
    SelToFlat
    mov     [esp + 04h], eax            ; Argument ptr
    mov     edx, eax

    mov     eax, [esp + 00h]            ; ExecFlag DWORD in eax

    mov     bx, seg FLAT:DATA32
    mov     ds, bx                      ; Make ds flat
    mov     es, bx                      ; Make es flat

    call    tkExecPgmWorker             ;(ULONG flags, arg,    env,   prog)
    add     esp, 10                     ;       eax,   edx,    ecx,
                                        ;     ebp+8, ebp+c, ebp+10, ebp+14
                                        ;     esp+0, esp+4, esp+08, esp+0c
    or      eax, eax
    jnz     ftkep_ret
    call    fake_tkStartProcess         ; If succesfully so far. call start process.
    jmp     ftkep_ret2                  ; <Currently no parameters are implemented.>

ftkep_ret:
    call    _fakeLDRClearSem@0

ftkep_ret2:
    leave
    ret
fakeg_tkExecPgm ENDP


;;
; Calls the fakeg_tkExecPgm procedure.
; @cproto   ULONG _Optlink  CalltkExecPgm(
;                               ULONG   execFlags,
;                               PCSZ    pArg,
;                               PCSZ    pEnv,
;                               PCSZ    pExecName);
; @returns
; @param    eax (ebp +08h)  execFlags
; @param    edx (ebp +0ch)  pArg
; @param    ecx (ebp +10h)  pEnv
; @param    ebp + 14h       pExecName
; @uses     eax, ecx, edx
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
CalltkExecPgm PROC NEAR
    push    ebp
    mov     ebp, esp
    push    ebx
    push    edi
    push    esi
    push    es
    push    ds

    ; Save parameters
    mov     [ebp+08h], eax

    ;
    ; create input for fakeg_tkExecPgm
    ;
    mov     eax, edx
    xor     edi, edi
    xor     esi, esi
    or      eax, eax
    jz      ctkep1
    FlatToSel
    mov     di, ax
    mov     esi, ebx                    ; di:si -> arguments
ctkep1:

    mov     eax, [ebp + 14h]
    xor     edx, edx
    mov     ds, dx
    or      eax, eax
    jz      ctkep2
    FlatToSel
    mov     ds, ax
    mov     edx, ebx                    ; ds:dx -> executable filename
ctkep2:

    mov     eax, ecx
    xor     ebx, ebx
    mov     es, bx
    or      eax, eax
    jz      ctkep3
    FlatToSel
    mov     es, ax
    mov     eax, ebx                    ; es:bx -> environment.
ctkep3:

    mov     eax, [ebp+08h]              ; ax = exec flags.

    call    far ptr FLAT:fakeg_tkExecPgm

    ;
    ; Restore and return.
    ;
    pop     ds
    pop     es
    pop     esi
    pop     edi
    pop     ebx
    leave
    ret
CalltkExecPgm ENDP


;;
; Gets the selector for the CODE16 segment.
; @cproto   USHORT  _Optlink GetSelectorCODE16(void)
; @returns  Selector for the CODE16 segment.
; @uses     eax
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
GetSelectorCODE16 PROC NEAR
    xor     eax, eax
    mov     ax, seg CODE16
    ret
GetSelectorCODE16 ENDP


;;
; Gets the selector for the DATA16 segment.
; @cproto   USHORT  _Optlink GetSelectorDATA16(void)
; @returns  Selector for the DATA16 segment.
; @uses     eax
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
GetSelectorDATA16 PROC NEAR
    xor     eax, eax
    mov     ax, seg DATA16
    ret
GetSelectorDATA16 ENDP


;;
; Gets the selector for the CODE32 segment.
; @cproto   USHORT  _Optlink GetSelectorCODE32(void)
; @returns  Selector for the CODE32 segment.
; @uses     eax
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
GetSelectorCODE32 PROC NEAR
    xor     eax, eax
    mov     ax, seg FLAT:CODE32
    ret
GetSelectorCODE32 ENDP

;;
; Gets the selector for the DATA32 segment.
; @cproto   USHORT  _Optlink GetSelectorDATA32(void)
; @returns  Selector for the DATA32 segment.
; @uses     eax
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
GetSelectorDATA32 PROC NEAR
    xor     eax, eax
    mov     ax, seg FLAT:DATA32
    ret
GetSelectorDATA32 ENDP



CODE32 ENDS

END
