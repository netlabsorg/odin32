; $Id: fakea.asm,v 1.6 2001-07-10 05:26:52 bird Exp $
;
; Fake assembly imports.
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
    include options.inc


;
;   Exported symbols
;
    public fakepTCBCur
    public fakepPTDACur
    public fakeptda_start
    public fakeptda_environ
    public fakeptda_handle
    public fakeptda_module
    public fakeptda_ptdasem
    public fakeptda_pBeginLIBPATH
    public fakef_FuStrLenZ
    public fakef_FuStrLen
    public fakef_FuBuff
    public fakeg_tkExecPgm
    public faketkStartProcess
    public CalltkExecPgm
    public _fakeldrOpenPath@20
    public fakeKMEnterKmodeSEF
    public fakeKMExitKmodeSEF8
    public fakeRASRST
    public fakedh_SendEvent
    public fakeh_POST_SIGNAL


;
;   Imported Functions and Variables.
;
    extrn tkExecPgmWorker:PROC          ; fake.c
    extrn _fakeLDRClearSem@0:PROC       ; fake.c
    extrn _fakeKSEMRequestMutex@8:PROC  ; fake.c
    extrn _fakeldrOpenPath_old@16:PROC  ; fake.c
    extrn _fakeldrOpenPath_new@20:PROC  ; fake.c
    extrn _options:options              ; d16globl.c


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
fakeptda_ptdasem            db  20  dup (0) ; PTDA semaphore - Intra-Process serialisation mutex KSEM (sg244640).
fakeptda_handle             dw      2   ; 2 is the hardcoded HPTDA of the current process.
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


;;
; RASRST faker.
; @status   stupid stub.
fakeRASRST proc near
    mov     ax, ds
    mov     ax, ds
    mov     ax, ds
    mov     ax, ds
    int     3
fakeRASRST endp


;;
; dh_SendEvent faker.
; @status   stupid stub.
fakedh_SendEvent proc near
    mov     ax, ds
    mov     ax, ds
    mov     ax, ds
    mov     ax, ds
    int     3
fakedh_SendEvent endp


;;
; POST_SIGNAL faker
fakeh_POST_SIGNAL proc near
    ; dummy prolog.
    mov     ax, ds
    mov     ax, ds
    mov     ax, ds
    mov     ax, ds
    int     3
fakeh_POST_SIGNAL endp


CODE16 ENDS



CODE32 SEGMENT
;;
; Faker of which simply clears the loader semaphore.
; @cproto    none! (void _Optlink   faketkStartProcess(void))
; @returns
; @param
; @uses
; @equiv
; @time
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark
faketkStartProcess PROC NEAR
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
faketkStartProcess ENDP


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
; @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
    call    faketkStartProcess          ; If succesfully so far. call start process.
    jmp     ftkep_ret2                  ; <Currently no parameters are implemented.>

ftkep_ret:
    push    eax
    call    _fakeLDRClearSem@0          ; clear the semaphore.
    pop     eax

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
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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


;;
; Wrapper for fakeldrOpenPath.
; @cproto       ULONG LDRCALL   fakeldrOpenPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful, ULONG lLibPath);
; @returns      Return of the current fakeldrOpenPath
; @param        pachFilename  Pointer to modulename. Not zero terminated!
; @param        cchFilename   Modulename length.
; @param        plv           Loader local variables? (Struct from KERNEL.SDF)
; @param        pful          Pointer to flags which are passed on to ldrOpen.
; @param        lLibPath      New parameter in build 14053.
;                             ldrGetMte calls with 1
;                             ldrOpenNewExe calls with 3
;                             This is compared to the initial libpath index.
;                                 The libpath index is:
;                                     BEGINLIBPATH    1
;                                     LIBPATH         2
;                                     ENDLIBPATH      3
;                                 The initial libpath index is either 1 or 2.
;                             - ignored -
; @uses         ecx, eax, edx
; @sketch
; @status
; @author       knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark
_fakeldrOpenPath@20 PROC NEAR
    ASSUME ds:FLAT
    ; dummy prolog.
    push    ebp
    mov     ebp, esp
    sub     esp, 10h
    add     esp, 10h
    pop     ebp
    ; real code
    cmp     FLAT:DATA16:_options.ulBuild, 14053
    jge     new
    jmp     near ptr FLAT:CODE32:_fakeldrOpenPath_old@16
new:
    jmp     near ptr FLAT:CODE32:_fakeldrOpenPath_new@20
_fakeldrOpenPath@20 ENDP



;;
; This is called at kernel entry.
; @cproto   none
; @returns  nothing
; @param    none
; @uses     nothing
; @status   stub.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
fakeKMEnterKmodeSEF proc near
    ; dummy prolog.
    push    ebp
    mov     ebp, esp
    sub     esp, 10h
    leave
    ret
fakeKMEnterKmodeSEF endp


;;
; This is called at kernel exit.
; @cproto   none
; @returns  nothing
; @param    none
; @uses     nothing
; @status   stub.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
fakeKMExitKmodeSEF8 proc near
    ; dummy prolog.
    push    ebp
    mov     ebp, esp
    sub     esp, 10h
    leave
    ret
fakeKMExitKmodeSEF8 endp

CODE32 ENDS

END
