; $Id: kRxa.asm,v 1.1 2000-06-03 03:50:45 bird Exp $
;
; kRxa - Small rexx script interpreter.
;
; Assembly edition.
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;


    .386p
    .model flat
    .stack 10000h

;DEBUG EQU 1

;
;   External Functions
;
    extrn   DosWrite:PROC
    extrn   RexxStart:PROC


;
;   Defined Constants And Macros
;
MAX_ARG_LENGTH      EQU     1000h

;
;   Structures and Typedefs
;
RXSTRING STRUC
cb      dd  ?
pach    dd  ?
RXSTRING ENDS



DATA32 segment dword public 'DATA' use32
;
;   Global Variables
;
ifdef DEBUG
    ;Debug stuff
szMsgDebug1 db "Debug1",13,10,0
szMsgDebug2 db "Debug2",13,10,0
szMsgDebug3 db "Debug3",13,10,0
endif

    ;Error messages.
szMsgScriptnameTooLong  db "Fatal error: The scriptname is too long.",13,10,0
szMsgArgumentsTooLong   db "Fatal error: The arguments are too long.",13,10,0
szMsgRexxStartFailed    db "Fatal error: RexxStart failed.",13,10,0

    ;Rexx argument string
rxstrArgs  RXSTRING <0, offset szArgs>

    ;Command env. name - RexxStart param - dont't know what it is used for...
szEnv                   db "hmm", 0

    ;Some uninitialized buffers
szScriptname            db  260d dup (?)
szArgs                  db  MAX_ARG_LENGTH dup (?)

DATA32 ends



CODE32 segment dword public 'CODE' use32

;;
; Exe Entry Point.
; @returns
; @param        eax = eax = ebx = ecx = esi = edi = ebp = 0
; @param        [esp + 00h]     Return address. (32-bit flat)
; @param        [esp + 04h]     Module handle for program module.
; @param        [esp + 08h]     Reserved
; @param        [esp + 0ch]     Environment data object address.
; @param        [esp + 10h]     Command line linear address in environemnt data object.
; @uses
; @equiv
; @time
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
main PROC NEAR
hModule         EQU [ebp + 08h]
pEnvironment    EQU [ebp + 10h]
pCommandline    EQU [ebp + 14h]
    LOCAL   sRexxRc:word
    LOCAL   cbActual:dword

    ;
    ; Find the arguments
    ;
    mov     eax, pCommandline
    call    strlen
    add     eax, pCommandline

    ;
    ; The script name is the first argument to this program
    ; The script arguments are following, so we'll have to find them..
    ;


    ;
    ; Copy the script name.
    ;
    mov     ecx, 260                    ; Max length.
    xchg    esi, eax                    ; esi <- Pointer to the arguments (script name) (-1).
    mov     edi, offset szScriptname    ; edi <- Target string buffer for the script name.

    mov     ah, ' '                     ; terminator is ' ' or '\0'
 skiploop1:                             ; skip spaces
    inc     esi                         ; next char (initially at terminator of exe name).
    mov     al, byte ptr [esi]          ; Get first char to determin terminator type.
    cmp     al, ah
    jz      skiploop1

    cmp     al, '"'                     ; if not '"' then goto copy loop.
    jne     loop1

    mov     al, '"'                     ; terminator is '"' or '\0'
    inc     eax                         ; skip '"'.


loop1:
    or      al, al                      ; zero terminator?
    jz      endloop1

    cmp     al, ah                      ; the other terminator?
    jz      endloop1

    stosb                               ; copy the byte


    ; next char
    inc     esi                         ; next
    dec     ecx                         ; check that we haven't reached the max length.
    jz      Scriptname_Too_Long

    mov     al, byte ptr [esi]          ; read (next) byte
    jmp     loop1

endloop1:
    mov     byte ptr [edi], 0           ; terminate scriptname.

    ; more arguments?
    or      al, al                      ; stopped at null terminator, then no more args.
    jz      callrexx
    inc     esi                         ; skip terminator.

    ;
    ; skip spaces between the scriptname and the first argument.
loop2:
    mov     al, byte ptr [esi]
    cmp     al, ' '
    jne     endloop2
    inc     esi
    jmp     loop2
endloop2:

    ;
    ; Copy arguments, we'll copy till '\0'
    ;
    mov     eax, esi
    call    strlen                      ; get argument length
    mov     ecx, eax                    ; ecx <- number of bytes to copy (argument length).
    mov     rxstrArgs.cb, ecx                 ; Set length of rexx argument string.
    cmp     ecx, MAX_ARG_LENGTH
    jae     Arguments_Too_Long          ; Overflow detected.
    mov     edi, offset szArgs          ; edi <- Target argument buffer.
    repnz movsb                         ; memcpy(edi, esi, ecx);
                                        ; Not sure if it copies the '\0', but that
                                        ; don't matter while the memory is initialized
                                        ; to zero by the OS!

callrexx:
    ; init return string.
    xor     ebx, ebx                    ; ebx <- 0

    push    ebx                         ; NULL - Rexx program return string.
    lea     eax, sRexxRc
    push    eax                         ; Rexx program return code.
    push    ebx                         ; NULL - EXIT (FIXME?)
    push    ebx                         ; RXCOMMAND (0) - Program called as Command.
    push    offset szEnv                ; Command env. name. What is this????
    push    ebx                         ; NULL - Pointer to INSTORE? Not used.
    push    offset szScriptname         ; Name of REXX script.
    push    offset rxstrArgs            ; Pointer to argument array (rexx strings) - 1 element.
    push    1                           ; Number of arguments - one.
    call    RexxStart
    sub     esp, DWORD * 9

    ;Check return value from RexxStart
    or      eax, eax
    jnz     RexxStartFailed

;
; return rexx error code.
;
    mov     ax, sRexxRc
    ret


;
; errors
;
Scriptname_Too_Long:
    mov     eax, offset szMsgScriptnameTooLong
    jmp     PrintError

Arguments_Too_Long:
    mov     eax, offset szMsgArgumentsTooLong
    jmp     PrintError

RexxStartFailed:
    mov     eax, offset szMsgRexxStartFailed
    jmp     PrintError

PrintError:
    lea     ebx, cbActual
    push    ebx                         ; cbActual - last arg for DosWrite
    mov     ebx, eax                    ; ebx <- string pointer
    call    strlen
    push    eax                         ; Bytes to write
    push    ebx                         ; Pointer to string.
    push    1                           ; stderr
    call    DosWrite
    sub     esp, DWORD * 4              ; clean stack.

    mov     eax, -10002
    ret
main ENDP

;;
; String length function.
; @cproto       int _Optlink strlen(const char *)
; @returns      eax = string length.
; @param        eax     pointer to string.
; @uses         ecx, edx
strlen PROC NEAR
    push    edi

    xor     ecx, ecx
    dec     ecx                         ; ecx <- 0ffffffffh
    mov     edi, eax                    ; esi <- string pointer
    xor     eax, eax                    ; scan until null terminator is found.

    repnz   scasb                       ; the actual scan.
    neg     ecx                         ; ecx is ffffffffh - string length
                                        ; negate it to get the string length.
    mov     eax, ecx                    ; mov string length into eax (return value)
    sub     eax, 2                      ; addjust it.

    pop     edi
    ret
strlen ENDP


ifdef DEBUG
;;
; Write debug string to stdout.
;
; @param     eax    Pointer to string.
; @uses
; @equiv
; @time
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
debugwrite PROC NEAR
    LOCAL   cbActual:dword
    push    ebx
    push    esi
    push    edi

    ;
    ; Write the arguments to stdout.
    ;
    push    ebx
    lea     ebx, cbActual
    push    ebx                         ; cbActual - last arg for DosWrite
    mov     ebx, eax
    call    strlen
    push    eax                         ; Bytes to write
    push    ebx                         ; Pointer to string.
    push    1                           ; stdout
    call    DosWrite
    sub     esp, DWORD * 4              ; clean stack.

    pop     edi
    pop     esi
    pop     ebx
    ret
debugwrite ENDP

endif


CODE32 ENDS

END main
