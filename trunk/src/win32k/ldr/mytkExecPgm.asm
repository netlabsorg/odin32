; $Id: mytkExecPgm.asm,v 1.5 2000-02-21 09:24:01 bird Exp $
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
DATA32 SEGMENT
    extrn  g_tkExecPgm:PROC
DATA32 ENDS
    extrn  AcquireBuffer:PROC
    extrn  ReleaseBuffer:PROC
    extrn  QueryBufferSegmentOffset:PROC

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
    ;uses:   nearly all (save bp)
    ;return: success CF clear
    ;        failure CF set
    extrn  f_FuBuff:PROC

;
;   Exported symbols
;
    public mytkExecPgm
    public pszFilename
    public pszArguments


DATA32 SEGMENT
pszFilename     dd 0                    ; Pointer to the filename (in the buffer)
pszArguments    dd 0                    ; Pointer to the arguments (int the buffer)
DATA32 ENDS


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
;SegBuffer   =  word ptr (dword ptr -08h)
SegBuffer   = -08h
;OffBuffer   =  word ptr (dword ptr -0Ch)
OffBuffer   = -0Ch
cchFilename = dword ptr -10h
cchArgs     = dword ptr -14h
    ASSUME CS:CODE32, DS:NOTHING, SS:NOTHING
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
    mov     bx, ds
    mov     di, dx                      ; es:di is now filename address (ds:dx).
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:f_FuStrLen
    movzx   ecx, cx
    mov     [ebp+cchFilename], ecx
    popad
    jc      mytkExecPgm_CalltkExecPgm_X1; If the FuStrLen call failed we bail out!

    ;
    ; if filename length is more that CCHMAXPATH then we don't do anything!.
    ;
    mov     ecx, [ebp+cchFilename]
    cmp     ecx, 260
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
    mov     bx, di                      ;
    mov     di, si                      ; bx:di -> arguments
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:f_FuStrLenZ
    movzx   ecx, cx
    mov     [ebp+cchArgs], ecx
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
    mov     di, ax                      ; es:di  pBuffer
    mov     si, dx
    mov     bx, ds                      ; bx:si  Filename pointer (input ds:dx)
    mov     ecx, [ebp+cchFilename]
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:f_FuBuff
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
    mov     ecx, [ebp+cchArgs]
    mov     bx, di                      ; ds:si -> arguments
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:f_FuBuff
    popad
    jc      mytkExecPgm_CalltkExecPgm_X2
    jmp     mytkExecPgm_CalltkExecPgm_3

mytkExecPgm_CalltkExecPgm_2:
    mov     word ptr es:[eax], 0        ; Terminate the empty string!

    ;
    ; Set Pointers, pszFilename and pszArguments
    ;
mytkExecPgm_CalltkExecPgm_3:
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    ASSUME  ds:FLAT
    mov     eax, ss:[ebp+pBuffer]
    mov     pszFilename, eax
    add     eax, 261
    mov     pszArguments, eax

    ;
    ; Restore variables pushed on the stack
    ;
    ; stack: edi, es, ds, ecx, eax
    pop     edi
    pop     es
    pop     ds
    pop     ecx
    pop     eax

    ;
    ; Set new input parameters (call g_tkExecPgm)
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
    ; Call g_tkExecPgm
    ;
    push    cs                          ; Problem calling far into the calltab segement.
    call    near ptr FLAT:g_tkExecPgm
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
    call    far ptr FLAT:g_tkExecPgmStub
    leave
    retf
mytkExecPgm ENDP


;;
; Stub which jumps to g_tkExecPgmStub.
; (This way I will hopefully get the right selector.)
g_tkExecPgmStub PROC FAR
    jmp     near ptr FLAT:g_tkExecPgm
g_tkExecPgmStub ENDP


CODE32 ENDS
END

