; $Id: mytkExecPgm.asm,v 1.1 2000-02-18 19:28:17 bird Exp $
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
    extrn  g_tkExecPgm:PROC
    extrn  AcquireBuffer:PROC
    extrn  ReleaseBuffer:PROC
    extrn  QueryBufferSegmentOffset:PROC

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


DATA16 SEGMENT
flatselector dw FLAT
DATA16 ENDS

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
;       char szFilename[261]; /* offset 0   */
;       char szArg[4096-261]; /* offset 261 */
;   };
;
mytkExecPgm PROC FAR
pBuffer     = dword ptr -04h
SegBuffer   = word  ptr -08h
OffBuffer   = word  ptr -0Ch
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

    ; init local variables
    mov     [ebp+pBuffer], 0

    ; filename length
    mov     ax, ds
    mov     es, ax
    xor     eax, eax
    movzx   edi, dx                     ; es:di is now filename address (ds:dx).
    mov     ecx, 0ffffffffh
    repne scasb
    not     ecx

    ;
    ; if filename length is more that CCHMAXPATH then we don't do anything!.
    ;
    cmp     ecx, 260
    jae     mytkExecPgm_CalltkExecPgm_X1; length >= 260
    mov     [ebp+cchFilename], ecx

    ;
    ; args length
    ;
    pop     edi
    push    edi
    mov     es, di
    movzx   edi, si                     ; es:di is now args address (di:si), eax is still 0
    mov     ecx, 0ffffffffh
    repne scasb
    not     ecx

    mov     [ebp+cchArgs], ecx
    add     ecx, [ebp+cchFilename]      ; filename
    add     ecx, 3 + 260                ;  260 = new argument from a scrip file or something.
                                        ;    3 = two '\0's and a space after added argument.
    cmp     ecx, 4096                   ; 4096 = Buffersize.  FIXME! Define this!!!
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
    push    esi
    mov     edi, eax                    ; es:di  pBuffer
    movzx   esi, dx                     ; ds:si  Filename pointer (input ds:dx)
    mov     ecx, [ebp+cchFilename]
    repnz movsb

    ;
    ; Copy Args to pBuffer + 261
    ;
    ; stack: esi, edi, es, ds, ecx, eax
    pop     esi
    pop     edi
    push    edi
    push    esi
    mov     ds, di                      ; ds:si -> arguments
    mov     edi, eax
    add     edi, 261                    ; es:di -> pBuffer + 261
    mov     ecx, [ebp+cchArgs]
    repnz movsb

    ;
    ; Set Pointers, pszFilename and pszArguments
    ;
    push    DATA16
    pop     ds
    mov     ax, ds:flatselector
    mov     ds, ax
    ASSUME  ds:FLAT
    mov     eax, ss:[ebp+pBuffer]
    mov     pszFilename, eax
    add     eax, 261
    mov     pszArguments, eax

    ;
    ; Restore variables pushed on the stack
    ;
    ; stack: esi, edi, es, ds, ecx, eax
    pop     esi
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
    call far ptr FLAT:g_tkExecPgm;WARNING far call!!!
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
    call    far ptr FLAT:g_tkExecPgm
    leave
    retf
mytkExecPgm ENDP

CODE32 ENDS
END

