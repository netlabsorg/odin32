; $Id: mytkExecPgm.asm,v 1.13.2.1 2001-09-27 03:08:28 bird Exp $
;
; mytkExecPgm - tkExecPgm overload
;
; Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
;   Defined Constants And Macros
;
CCHFILENAME     EQU 261                 ; The size of the filename buffer
CCHARGUMENTS    EQU 1536                ; The size of the argument buffer
CCHMAXPATH      EQU CCHFILENAME - 1     ; Max path length

;
;   Include files
;
    include devsegdf.inc


;
;   Imported Functions and variables.
;
    extrn  g_tkExecPgm:PROC

    ; Scans strings until empy-string is reached.
    ; input:  bx:di
    ; uses:   nearly all (save bp)
    ; return: cx size - CF clear
    ;         ax error- CF set
    extrn  f_FuStrLenZ:PROC

    ; Stringlength
    ; input:  bx:di
    ; uses:   nearly all (save bp)
    ; return: cx size - CF clear
    ;         ax error- CF set
    extrn  f_FuStrLen:PROC

    ;memcpy
    ;input:  bx:si pointer to source
    ;        es:di pointer to target
    ;        cx    count of bytes to copy
    ;uses:   nearly all (save bp), es, ds
    ;return: success CF clear
    ;        failure CF set
    extrn  f_FuBuff:PROC


    ; 32-bit memcpy. (see OS2KTK.h)
    extrn _TKFuBuff@16:PROC

    ;
    ; LDR semaphore
    ;
    extrn _LdrSem:DWORD
    extrn _LDRClearSem@0:PROC
    extrn _KSEMRequestMutex@8:PROC
    extrn _KSEMQueryMutex@8:PROC

    ;
    ; Loader State
    ;
    extrn ulLDRState:DWORD

    ;
    ; Pointer to current executable module.
    ;
    extrn pExeModule:DWORD

    ;
    ; DevHlp32
    ;
    extrn D32Hlp_VirtToLin:PROC

    ;
    ; TKSSBase (32-bit)
    ;
    extrn _TKSSBase:DWORD

;
;   Exported symbols
;
    public myg_tkExecPgm
    public tkExecPgmCopyEnv

    public fTkExecPgm
    public achTkExecPgmFilename
    public achTkExecPgmArguments



;
; Global data
;

; Filename and arguments buffers + environment pointer
; from the tkExecPgm call.
;
; This data is only valid at isLdrStateExecPgm time
; (and you'll have to be behind the loader semaphore of course!)
DATA16 SEGMENT
fTkExecPgm              db 0            ; 0 - achTkExecPgmFilename and achTkExecPgmArguments is INVALID
                                        ; 1 - achTkExecPgmFilename and achTkExecPgmArguments is VALID.
achTkExecPgmFilename    db CCHFILENAME dup (0)  ; The filename  passed in to tkExecPgm if (fTkExec is TRUE)
achTkExecPgmArguments   db CCHARGUMENTS dup (0) ; The arguments passed in to tkExecPgm if (fTkExec is TRUE)
fpachTkExecPgmEnv       dd 0            ; Far pointer to environment passed in to tkExecPgm.
                                        ; Valid at isLdrStateExecPgm time.
                                        ; NOTE! User data, don't touch it directly!
DATA16 ENDS


CODE32 SEGMENT
;;
; New implementation.
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
myg_tkExecPgm PROC FAR
cchFilename = -4h
cchArgs     = -08h
usExecFlag  = -0ch
SegFilename = -10h
OffFilename = -12h
SegEnv      = -14h
OffEnv      = -16h
SegArg      = -18h
OffArg      = -1ah

    ASSUME CS:CODE32, DS:NOTHING, SS:NOTHING
    push    ebp
    mov     ebp, esp
    lea     esp, [ebp + OffArg]

    ;
    ; Save input parameters
    ;
    mov     [ebp + usExecFlag], ax
    mov     ax, es
    mov     [ebp + SegEnv], ax
    mov     [ebp + OffEnv], bx
    mov     [ebp + SegArg], di
    mov     [ebp + OffArg], si
    mov     bx, ds
    mov     [ebp + SegFilename], bx
    mov     [ebp + OffFilename], dx

    ;
    ; Parameter validations - if any of these fail we'll just pass on to
    ; the real tkExecPgm without setting up any buffers stuff.
    ; 1) validate the file pointer.
    ; 2) validate the file name length < 260
    ; 3) validate that the arguments aren't larger than the buffer.
    ;

    ; Validate filename pointer
    ;
    cmp     bx, 4                       ; pointer to filename
    jb      tkepgm_backout

    ; Validate filename length
    ;
    mov     di, dx                      ; bx:di is now filename address
    push    cs                          ; Problem calling far into the calltab segement.
    call    f_FuStrLen                  ; NB/TODO/FIXME: this is a 16-bit call...
    jc      tkepgm_backout              ; If the FuStrLen call failed we bail out!

    ; if filename length is more that CCHMAXPATH then we don't do anything!.
    cmp     cx, CCHMAXPATH
    jae     tkepgm_backout              ; length >= CCHMAXPATH
    mov     [ebp + cchFilename], cx

    ;
    ; args length
    ; Note: the arguments are a series of ASCIIZs ended by an empty string (ie. '\0').
    ;
    xor     cx, cx                      ; Set length to zero.
    mov     bx, [ebp + SegArg]
    cmp     bx, 4                       ; The argument might me an NULL pointer
    jb      tkepgm1

    mov     di, [ebp + OffArg]          ; bx:di -> arguments
    push    cs                          ; Problem calling far into the calltab segement.
    call    f_FuStrLenZ                 ; NB/TODO/FIXME: this is a 16-bit call...
    jc      tkepgm_backout

tkepgm1:
    mov     [ebp + cchArgs], cx
    add     cx, [ebp + cchFilename]     ; filename length
    add     cx, 3 + 260                 ;  260 = additional arguments from a script file or something.
                                        ;    3 = two '\0's and a space after added argument.
    cmp     cx, CCHARGUMENTS            ; argument Buffersize.
    jae     tkepgm_backout              ; jmp if argument + file + additional arguments >= buffer size


    ;
    ; Aquire the OS/2 loader semaphore
    ;   Since parameters looks good, we're ready for getting the loader semaphore.
    ;   We use the loader semaphore to serialize access to the win32k.sys loader
    ;   subsystem.
    ;   Before we can get the loader semaphore, we'll need to set ds and es to
    ;   flat R0 context.
    ;   The loader semaphore is later requested by the original tkExecPgm so
    ;   this shouldn't break anything.
    ;
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    mov     es, ax
    ASSUME  DS:FLAT, ES:FLAT

    mov     eax, offset _LdrSem         ; Get pointer to the loader semaphore.
    or      eax, eax                    ; Check if null. (paranoia)
    jz      tkepgm_backout              ; Fail if null.

    push    0ffffffffh                  ; Wait indefinitely.
    push    eax                         ; Push LdrSem address (which is the handle).
    call    near ptr FLAT:_KSEMRequestMutex@8
    or      eax, eax                    ; Check if failed.
    jnz     tkepgm_backout              ; Backout on failure.


    ;
    ; From here on we won't backout to the tkepgm_backout lable but
    ; the tkepgm_backout2 lable. (This will restore the parameters
    ; and jump in at the call to tkExecPgm behind the Loader Sem.)
    ;


    ;
    ; Set global data:
    ;   Zeros pointer to exemodule to NULL (a bit paranoia).
    ;   Mark global data valid.
    ;   Store Environment pointer.
    ;   Set loader state.
    ;
    mov     pExeModule, 0               ; Sets the exemodule pointer to NULL.
    mov     fTkExecPgm, 1               ; Optimistic, mark the global data valid.
    mov     eax, [ebp + OffEnv]         ; Environment FAR pointer.
    mov     fpachTkExecPgmEnv, eax      ; Store the Environment pointer. This will
                                        ; later permit us to get the passed in
                                        ; environment in for ex. ldrOpenPath.
    mov     ulLDRState, 1               ; Set the loader state to LDRSTATE_TKEXECPGM!
    ASSUME  DS:NOTHING, ES:NOTHING


    ;
    ; Copy filename to achBuffer.
    ;
    mov     di, seg achTkExecPgmFilename
    mov     es, di
    mov     edi, offset achTkExecPgmFilename
                                        ; es:(e)di -> &achTkExecPgmFilename[0]
    mov     si, [ebp + OffFilename]
    mov     bx, [ebp + SegFilename]     ; bx:si  Filename pointer (input ds:dx)
    ASSUME DS:NOTHING
    mov     cx, [ebp + cchFilename]     ; cx = length of area to copy
    push    cs                          ; Problem calling far into the calltab segement.
    call    f_FuBuff                    ; NB/TODO/FIXME: this is a 16-bit call...
    jc      tkepgm_backout2             ; In case of error back (quite unlikely).


    ;
    ; Copy Args to achTkExecPgmArguments
    ;
    mov     di, seg achTkExecPgmArguments
    mov     es, di
    mov     edi, offset achTkExecPgmArguments
                                        ; es:(e)di -> &achTkExecPgmArguments[0]
    mov     word ptr es:[edi], 0        ; Terminate the argument string in case
                                        ; there aren't any arguments.('\0\0')
                                        ; (We're just about to find that out.)
    mov     bx, [ebp + SegArg]
    cmp     bx, 4                       ; Is the argument pointer a null-pointer?
    jb      tkepgm_setup_parms          ; Skip copy if null pointer.
                                        ; Argument string is '\0\0'.
    mov     si, [ebp + OffArg]          ; bx:si -> arguments
    mov     cx, [ebp + cchArgs]         ; cx = length of area to copy
    push    cs                          ; Problem calling far into the calltab segement.
    call    f_FuBuff                    ; NB/TODO/FIXME: this is a 16-bit call...
    jc      tkepgm_backout2             ; In case of error back (quite unlikely).


    ;
    ; Setup new input parameters (call g_tkExecPgm)
    ;
    ; ds:dx is to become &achTkExecPgmFilename[0]
    ; di:si is to become &achTkExecPgmArguments[0]
    ;
tkepgm_setup_parms:
    mov     ax, [ebp + usExecFlag]
    mov     di, seg achTkExecPgmArguments
    mov     esi, offset achTkExecPgmArguments ; di:si  &achTkExecPgmArguments[0]
    mov     ds, di                            ; Assumes same segment (which of course is true).
    mov     edx, offset achTkExecPgmFilename  ; ds:dx  &achTkExecPgmFilename[0]
    mov     bx, [ebp + SegEnv]
    mov     es, bx
    mov     bx, [ebp + OffEnv]                ; es:bx  Environment


    ;
    ; Call g_tkExecPgm
    ;
tkepgm_callbehind:
    push    cs                          ; Problem calling far into the calltab segement.
    call    g_tkExecPgm
    pushfd                              ; preserve flags
    push    eax                         ; preserve result.
    push    ecx                         ; preserve ecx just in case
    push    edx                         ; preserve edx just in case
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    mov     es, ax
    ASSUME  ds:FLAT, es:FLAT            ; both ds and es are now FLAT


    ;
    ; Clear loader semaphore?
    ; and clear loader state, current exe module and tkExecPgm global data flag.
    ;
    push    0                           ; Usage count variable.
    mov     eax, _TKSSBase              ; Get TKSSBase
    add     eax, esp                    ; Added TKSSBase to the usage count pointer
    push    eax                         ; Push address of usage count pointer.
    push    offset _LdrSem              ; Push pointer to loader semaphore ( = handle).
    call    _KSEMQueryMutex@8
    or      eax, eax                    ; Check return code. (1 = our / free; 0 = not our but take)
    pop     eax                         ; Pops usage count.
    je      tkepgm_callbehindret        ; jmp if not taken by us (rc=FALSE).
    or      eax, eax                    ; Check usage count.
    jz      tkepgm_callbehindret        ; jmp if 0 (=free).
    mov     ulLDRState, 0               ; Clears loaderstate. (LDRSTATE_UNKNOWN)
    mov     pExeModule, 0               ; Sets the exemodule pointer to NULL.
    mov     fTkExecPgm, 0               ; Marks global data invalid.
    call    near ptr FLAT:_LDRClearSem@0

    ;
    ; Restore ds and es (probably unecessary but...) and Return
    ;
tkepgm_callbehindret:
    push    dword ptr [ebp + SegFilename]
    pop     ds
    push    dword ptr [ebp + SegEnv]
    pop     es
    pop     edx                         ; restore edx
    pop     ecx                         ; restore ecx
    pop     eax                         ; restore result.
    popfd                               ; restore flags
    leave
    retf


;
; This is a backout were tkExecPgm probably will backout and we're
; allready behind the loader semaphore.
;
tkepgm_backout2:
    ;
    ; Set Flat context and invalidate buffer.
    ;
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    ASSUME ds:FLAT
    mov     fTkExecPgm, 0               ; Marks global data invalid.

    ;
    ; Restore parameters. and call the original tkExecPgm
    ;
    mov     ax, [ebp + usExecFlag]
    mov     dx, [ebp + SegFilename]
    mov     ds, dx
    mov     dx, [ebp + OffFilename]
    mov     bx, [ebp + SegEnv]
    mov     es, bx
    mov     bx, [ebp + OffEnv]
    mov     di, [ebp + SegArg]
    mov     si, [ebp + OffArg]
    jmp     tkepgm_callbehind


;
; This is a backout were tkExecPgm too is exspected to back out.
;
tkepgm_backout:
    ;
    ; Restore parameters. and call the original tkExecPgm
    ;
    mov     ax, [ebp + usExecFlag]
    mov     dx, [ebp + SegFilename]
    mov     ds, dx
    mov     dx, [ebp + OffFilename]
    mov     bx, [ebp + SegEnv]
    mov     es, bx
    mov     bx, [ebp + OffEnv]
    mov     di, [ebp + SegArg]
    mov     si, [ebp + OffArg]

myg_tkExecPgm_CalltkExecPgm:
    push    cs                          ; Problem calling far into the calltab segement.
    call    g_tkExecPgm
    leave
    retf
myg_tkExecPgm ENDP



;;
; Function which copies the environment data passed into tkExecPgm
; to a given buffer.
; @cproto    ULONG _Optlink tkExecPgmCopyEnv(char *pachBuffer, unsigned cchBuffer);
; @returns   OS/2 return code - NO_ERROR on success.
;            0 on error or no data.
; @param     pachBuffer     Pointer to buffer which the environment data is
;                           to be copied to.
;                           (eax)
; @param     cchBuffer      Size of the buffer.
;                           (edx)
; @uses      eax, edx, ecx
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
tkExecPgmCopyEnv PROC NEAR
cchEnv  = -04h
    ASSUME ds:FLAT, es:FLAT, ss:NOTHING
    push    ebp
    mov     ebp, esp
    lea     esp, [ebp + cchEnv]

    push    ebx
    mov     ebx, eax                    ; ebx now holds the buffer pointer.

    ;
    ; Call tkExecPgmEnvLength to get length and check that pointer is valid.
    ;
    push    edx
    call    tkExecPgmEnvLength
    pop     ecx                         ; ecx now holds the buffer length.

    cmp     eax, 0
    ja      tkepce_ok1
    mov     eax, 232                    ; ERROR_NO_DATA
    jmp     tkepce_ret                  ; Fail if no data or any other error.

tkepce_ok1:
    cmp     eax, ecx                    ; (ecx is the buffer size.)
    jbe     tkepce_ok2                  ; Fail if buffer too small.
    mov     eax, 111                    ; ERROR_BUFFER_OVERFLOW
    jmp     tkepce_ret

tkepce_ok2:
    mov     [ebp + cchEnv], eax         ; Save environment length.


    ;
    ; Thunk the environment 16-bit far pointer to 32-bit.
    ;
    mov     eax, fpachTkExecPgmEnv
    call    D32Hlp_VirtToLin
    or      eax, eax                    ; check if thunking were successful.
    jnz     tkepce_ok3                  ; Jump if success.
    mov     eax, edx                    ; A special feature for D32Hlp_VirtToLin is that edx
                                        ; have the error code in case on failure.
    jmp tkepce_ret

tkepce_ok3:
    ;
    ; Copy the environment data.
    ;
    push    3                           ; Fatal if error.
    push    dword ptr [ebp + cchEnv]    ; Number of bytes to copy
    push    eax                         ; Source buffer pointer. (user)
    push    ebx                         ; Target buffer pointer.
    call    _TKFuBuff@16

tkepce_ret:
    pop     ebx
    leave
    ret
tkExecPgmCopyEnv ENDP



;;
; This function gets the length of the tkExecPgm environment data.
; @cproto    ULONG _Optlink tkExecPgmEnvLength(void);
; @returns   Environment data length in bytes.
; @uses      eax, edx, ecx
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
tkExecPgmEnvLength PROC NEAR
    ASSUME ds:FLAT, es:FLAT, ss:NOTHING
    push    ebp
    mov     ebp, esp

    ;
    ; Push register which needs to be presered.
    ;
    push    es
    push    ds
    push    esi
    push    edi
    push    ebx


    ;
    ; Check that the data is valid.
    ;
    cmp     ulLDRState, 1               ; LDRSTATE_TKEXECPGM
    jnz     tkepel_err_ret


    ;
    ; Check if the environment pointer is NULL.
    ;
    mov     ebx, fpachTkExecPgmEnv
    ror     ebx, 16
    cmp     bx, 4
    jb      tkepel_err_ret


tkepel1:
    ;
    ; Get the environment length
    ;
    mov     edi, ebx
    ror     edi, 16                     ; bx:di -> [fpachTkExecPgmEnv]
    xor     ecx, ecx
    push    cs                          ; Problem calling far into the calltab segement.
    call    f_FuStrLenZ                 ; NB/TODO/FIXME: this is a 16-bit call...
    jc      tkepel_err_ret
    movzx   eax, cx
    jmp     tkepel_ret


; Failure
tkepel_err_ret:
    xor     eax, eax


; Return
tkepel_ret:
    pop     ebx                         ; restore registers
    pop     edi
    pop     esi
    pop     ds
    pop     es
    leave
    ret
tkExecPgmEnvLength ENDP





CODE32 ENDS


END

