; $Id: mytkExecPgm.asm,v 1.7 2000-02-21 14:53:39 bird Exp $
;
; mytkExecPgm - tkExecPgm overload
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
;   Include files
;
    include devsegdf.inc

;
;   Imported Functions
;
    extrn  _g_tkExecPgm:PROC
    extrn  AcquireBuffer:PROC
    extrn  ReleaseBuffer:PROC
    extrn  QueryBufferSegmentOffset:PROC

    ; Scans strings until empy-string is reached.
    ; input:  bx:di
    ; uses:   nearly all (save bp)
    ; return: cx size - CF clear
    ;         ax error- CF set
    extrn  _f_FuStrLenZ:PROC

    ; Stringlength
    ; input:  bx:di
    ; uses:   nearly all (save bp)
    ; return: cx size - CF clear
    ;         ax error- CF set
    extrn  _f_FuStrLen:PROC

    ;memcpy
    ;input:  bx:si pointer to source
    ;        es:di pointer to target
    ;        cx    count of bytes to copy
    ;uses:   nearly all (save bp), es, ds
    ;return: success CF clear
    ;        failure CF set
    extrn  _f_FuBuff:PROC

;
;   Exported symbols
;
    public mytkExecPgm



CODE32 SEGMENT

;;
;
; @returns   same as tkExecPgm: eax, edx and carry flag
; @param     ax     Exec flag
;            ds:dx  Filename address. (String)
;            es:bx  Environment address. (String)
;            di:si  Argument address. (String)
; @uses      all - bp
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    Current implemententation assumes that there is one buffer,
;            this serializes the usage of the two pointers.
;
;   The buffer we are using is a C struct as follows.
;   struct Buffer
;   {
;       char szFilename[261];  /* offset 0   */
;       char achArg[1536-261]; /* offset 261 */
;   };
;
mytkExecPgm PROC FAR
pBuffer     = dword ptr -04h
SegBuffer   = -08h
OffBuffer   = -0Ch
cchFilename = dword ptr -10h
cchArgs     = dword ptr -14h
;usExecFlag  = -18h
;SegFilename = -1ch
;OffFilename = -1eh
;SegEnv      = -20h
;OffEnv      = -22h
;SegArg      = -24h
;OffArg      = -26h

    ASSUME CS:CODE32, DS:NOTHING, SS:NOTHING
    int 3
    push    ebp
    mov     ebp, esp
    lea     esp, [ebp + cchArgs]

    push    eax
    push    ecx
    push    ds
    push    es
    push    edi

    ; parameter validations
    mov     ax, ds                      ; pointer to filename
    cmp     ax, 4
    jb      mytkExecPgm_CalltkExecPgm_X1

    ;
    ; filename length
    ;
    mov     ax, ds
    mov     es, ax
    pushad
    push    es
    push    ds
    mov     bx, ds
    mov     di, dx                      ; es:di is now filename address (ds:dx).
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuStrLen
    movzx   ecx, cx
    mov     [ebp+cchFilename], ecx
    pop     ds
    pop     es
    popad
    jc      mytkExecPgm_CalltkExecPgm_X1; If the FuStrLen call failed we bail out!

    ;
    ; if filename length is more that CCHMAXPATH then we don't do anything!.
    ;
    cmp     [ebp+cchFilename], 260
    jae     mytkExecPgm_CalltkExecPgm_X1; length >= 260

    ;
    ; args length
    ; Note: the arguments are a series of ASCIIZs ended by an empty string (ie. '\0').
    ;
    pop     edi
    push    edi
    xor     ecx, ecx
    cmp     di, 4                       ; The argument might me a invalid pointer...
    jb      mytkExecPgm_CalltkExecPgm_1

    pushad
    push    es
    push    ds
    mov     bx, di                      ;
    mov     di, si                      ; bx:di -> arguments
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuStrLenZ
    movzx   ecx, cx
    mov     [ebp+cchArgs], ecx
    pop     ds
    pop     es
    popad
    jc      mytkExecPgm_CalltkExecPgm_X1

mytkExecPgm_CalltkExecPgm_1:
    mov     ecx, [ebp+cchArgs]
    add     ecx, [ebp+cchFilename]      ; filename
    add     ecx, 3 + 260                ;  260 = new argument from a scrip file or something.
                                        ;    3 = two '\0's and a space after added argument.
    cmp     ecx, 1536                   ; 1536 = Buffersize.  FIXME! Define this!!!
    jae     mytkExecPgm_CalltkExecPgm_X1; jmp if argument + file + new file > buffer size

    ;
    ; Aquire a buffer
    ;
    call    AcquireBuffer
    or      eax, eax
    jz      mytkExecPgm_CalltkExecPgm_X1; Failed to get buffer.
    mov     [ebp+pBuffer], eax

    ;
    ; Get Segment and offset for the buffer
    ;
    call    QueryBufferSegmentOffset
    mov     cx, es
    mov     [ebp+OffBuffer], ax
    mov     [ebp+SegBuffer], es
    test    eax, 000570000h
    jnz     mytkExecPgm_CalltkExecPgm_X2

    ;
    ; Copy filename to pBuffer.
    ;
    pushad
    push    es
    push    ds
    mov     di, ax                      ; es:di  pBuffer
    mov     si, dx
    mov     bx, ds                      ; bx:si  Filename pointer (input ds:dx)
    mov     ecx, [ebp+cchFilename]
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuBuff
    pop     ds
    pop     es
    popad
    jc      mytkExecPgm_CalltkExecPgm_X2

    ;
    ; Copy Args to pBuffer + 261
    ;
    ; stack: edi, es, ds, ecx, eax
    pop     edi
    push    edi
    add     eax, 261                    ; we'll use eax in the branch
    cmp     di, 4
    jb      mytkExecPgm_CalltkExecPgm_2
    pushad
    push    es
    push    ds
    mov     ecx, [ebp+cchArgs]
    mov     bx, di                      ; ds:si -> arguments
    mov     di, ax                      ; es:di -> buffer + 261
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuBuff
    pop     ds
    pop     es
    popad
    jc      mytkExecPgm_CalltkExecPgm_X2
    jmp     mytkExecPgm_CalltkExecPgm_3

mytkExecPgm_CalltkExecPgm_2:
    mov     word ptr es:[eax], 0        ; Terminate the empty string!

    ;
    ; Restore variables pushed on the stack
    ;
    ; stack: edi, es, ds, ecx, eax
mytkExecPgm_CalltkExecPgm_3:
    pop     edi
    pop     es
    pop     ds
    pop     ecx
    pop     eax

    ;
    ; Set new input parameters (call _g_tkExecPgm)
    ;
    ; ds:dx is to become SegBuffer:OffBuffer
    ; di:si is to become SegBuffer:OffBuffer+261
    ;
    ; The some of the old values are stored on the stack (for the time being)
    push    ds
    push    edi
    push    esi

    mov     di, [ebp+SegBuffer]
    mov     ds, di
    mov     si, [ebp+OffBuffer]
    mov     dx, si                      ; ds:dx  SegBuffer:OffBuffer
    add     si, 261                     ; di:si  SegBuffer:OffBuffer+261

    ;
    ; Call _g_tkExecPgm
    ;
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_g_tkExecPgm
    pushfd

    ;
    ; Release buffer
    ;
    push    eax
    mov     eax, [ebp + pBuffer]
    call    ReleaseBuffer
    mov     [ebp + pBuffer], 0
    pop     eax

    ;
    ; Return
    ;
    popfd
    pop     esi
    pop     edi
    pop     ds
    leave
    retf

mytkExecPgm_CalltkExecPgm_X2:
    ;
    ; Release buffer
    ;
    mov     eax, [ebp + pBuffer]
    call    ReleaseBuffer
    mov     [ebp + pBuffer], 0

mytkExecPgm_CalltkExecPgm_X1:
    pop     edi
    pop     es
    pop     ds
    pop     ecx
    pop     eax

mytkExecPgm_CalltkExecPgm:
    push    cs
    call    near ptr FLAT:_g_tkExecPgm
    leave
    retf
mytkExecPgm ENDP



CODE32 ENDS

if 0 ; alternate implementation.
mytkExecPgm PROC FAR
pBuffer     = dword ptr -04h
SegBuffer   = -08h
OffBuffer   = -0Ch
cchFilename = -10h
cchArgs     = -14h
usExecFlag  = -18h
SegFilename = -1ch
OffFilename = -1eh
SegEnv      = -20h
OffEnv      = -22h
SegArg      = -24h
OffArg      = -26h

    ASSUME CS:CODE32, DS:NOTHING, SS:NOTHING
    int 3
    push    ebp
    mov     ebp, esp
    lea     esp, [ebp + OffArg]

    ; save input parameters
    mov     [ebp + usExecFlag], ax
    mov     ax, es
    mov     [ebp + SegEnv], ax
    mov     [ebp + OffEnv], bx
    mov     [ebp + SegArg], di
    mov     [ebp + OffArg], si
    mov     ax, ds
    mov     [ebp + SegFilename], ax
    mov     [ebp + OffFilename], dx

    ; parameter validations
    cmp     ax, 4                       ; pointer to filename
    jb      mytkExecPgm_CalltkExecPgm_X1

    ;
    ; filename length
    ;
    mov     bx, ax
    mov     di, dx                      ; bx:di is now filename address
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuStrLen
    jc      mytkExecPgm_CalltkExecPgm_X1; If the FuStrLen call failed we bail out!

    ;
    ; if filename length is more that CCHMAXPATH then we don't do anything!.
    ;
    cmp     cx, 260
    jae     mytkExecPgm_CalltkExecPgm_X1; length >= 260
    mov     [ebp+cchFilename], cx

    ;
    ; args length
    ; Note: the arguments are a series of ASCIIZs ended by an empty string (ie. '\0').
    ;
    mov     bx, [ebp+SegArg]
    cmp     bx, 4                       ; The argument might me an NULL pointer
    xor     cx, cx
    jb      mytkExecPgm_CalltkExecPgm_1

    mov     di, [ebp+OffArg]            ; bx:di -> arguments
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuStrLenZ
    mov     [ebp+cchArgs], cx
    jc      mytkExecPgm_CalltkExecPgm_X1

mytkExecPgm_CalltkExecPgm_1:
    add     cx, [ebp+cchFilename]       ; filename length
    add     cx, 3 + 260                 ;  260 = new argument from a scrip file or something.
                                        ;    3 = two '\0's and a space after added argument.
    cmp     ecx, 1536                   ; 1536 = Buffersize.  FIXME! Define this!!!
    jae     mytkExecPgm_CalltkExecPgm_X1; jmp if argument + file + new file > buffer size

    ;
    ; Aquire a buffer
    ;
    call    AcquireBuffer
    mov     [ebp+pBuffer], eax
    or      eax, eax
    jz      mytkExecPgm_CalltkExecPgm_X1; Failed to get buffer.

    ;
    ; Get Segment and offset for the buffer
    ;
    call    QueryBufferSegmentOffset
    mov     cx, es
    mov     [ebp+OffBuffer], ax
    mov     [ebp+SegBuffer], es
    test    eax, 000570000h
    jnz     mytkExecPgm_CalltkExecPgm_X2

    ;
    ; Copy filename to pBuffer.
    ;
    mov     di, ax                      ; es:di  pBuffer
    mov     si, dx
    mov     bx, ds                      ; bx:si  Filename pointer (input ds:dx)
    mov     cx, [ebp+cchFilename]       ; cx = length of area to copy
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuBuff
    jc      mytkExecPgm_CalltkExecPgm_X2

    ;
    ; Copy Args to pBuffer + 261
    ;
    mov     si, [ebp+SegArg]
    cmp     si, 4
    jb      mytkExecPgm_CalltkExecPgm_2
    mov     ds, si
    mov     si, [ebp+OffArg]            ; ds:si -> arguments
    mov     di, [ebp+SegBuffer]
    mov     es, di
    mov     di, [ebp+OffBuffer]
    add     di, 261                     ; es:di -> buffer + 261
    mov     cx, [ebp+cchArgs]           ; cx = length of area to copy
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_f_FuBuff
    jc      mytkExecPgm_CalltkExecPgm_X2
    jmp     mytkExecPgm_CalltkExecPgm_3

mytkExecPgm_CalltkExecPgm_2:
    mov     word ptr es:[eax], 0        ; Terminate the empty string!

    ;
    ; Set new input parameters (call _g_tkExecPgm)
    ;
    ; ds:dx is to become SegBuffer:OffBuffer
    ; di:si is to become SegBuffer:OffBuffer+261
    ;
mytkExecPgm_CalltkExecPgm_3:
    mov     di, [ebp+SegBuffer]
    mov     ds, di
    mov     si, [ebp+OffBuffer]
    mov     dx, si                      ; ds:dx  SegBuffer:OffBuffer
    add     si, 261                     ; di:si  SegBuffer:OffBuffer+261
    mov     bx, [ebp+SegEnv]
    mov     es, bx
    mov     bx, [ebp+SegEnv]

    ;
    ; Call _g_tkExecPgm
    ;
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:_g_tkExecPgm
    pushfd

    ;
    ; Release buffer
    ;
    push    eax
    mov     eax, [ebp + pBuffer]
    call    ReleaseBuffer
    mov     [ebp + pBuffer], 0
    pop     eax

    ;
    ; Return
    ;
    push    [ebp + SegFilename]
    pop     ds
    push    [ebp + SegEnv]
    pop     es
    popfd
    leave
    retf

mytkExecPgm_CalltkExecPgm_X2:
    ;
    ; Release buffer
    ;
    mov     eax, [ebp + pBuffer]
    call    ReleaseBuffer
    mov     [ebp + pBuffer], 0

mytkExecPgm_CalltkExecPgm_X1:
    pop     ds

mytkExecPgm_CalltkExecPgm:
    push    cs
    call    near ptr FLAT:_g_tkExecPgm
    leave
    retf
mytkExecPgm ENDP

CODE32 ENDS
endif


END

