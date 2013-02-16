; $Id: stackcorruption.asm,v 1.1 2002-04-29 14:24:27 bird Exp $
;
; 'Generic' wrapper function to find stack curruptions.
;
; Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
;
; Project Odin Software License can be found in LICENSE.TXT
;


        .386
        .MODEL FLAT


;
;   Defined Constants And Macros
;
FUNCTION_NAME           equ     WriteLog
FUNCTION_NAME_WRAPPED   equ     _WriteLog_
COUNT_OF_ARGS           equ     30h


;
;       Externs
;
extrn   DosSetMem:near
extrn   Dos32TIB:abs
extrn   FUNCTION_NAME_WRAPPED:near


CODE32 segment


FUNCTION_NAME proc near
save_eax        = dword ptr -4
save_ebx        = dword ptr -8
save_ecx        = dword ptr -12
save_edx        = dword ptr -16
pv              = dword ptr -20
cb              = dword ptr -24
        push    ebp
        mov     ebp, esp
        sub     esp, 30

        ;
        ; Save registers.
        ;
        mov     [ebp + save_eax], eax
        mov     [ebp + save_ebx], ebx
        mov     [ebp + save_ecx], ecx
        mov     [ebp + save_edx], edx

        ;
        ; Align stack at page boundary
        ;
        lea     esp, [esp - (COUNT_OF_ARGS * 4)]
        and     esp, NOT 0fffh

        ;
        ; Copy parameters.
        ;
        mov     ebx, COUNT_OF_ARGS*4
copyloop:
        sub     ebx, 4
        mov     eax, [ebp+ebx+8]
        mov     [esp+ebx], eax

        or      ebx, ebx
        jnz     copyloop

        ;
        ; Get stack range
        ;
        push    fs
        push    Dos32TIB
        pop     fs
        mov     eax, fs:[4]             ; tib_pstack
        mov     ecx, fs:[8]             ; tip_pstacklimit
        pop     fs
        sub     ecx, esp
        mov     [ebp+pv], esp
        mov     [ebp+cb], ecx


        ;
        ; Freeze the stack
        ;
        push    1                       ; PAG_READ
        push    [ebp+cb]                ; the size.
        push    [ebp+pv]                ; the pointer.
        call    DosSetMem
        add     esp, 0ch
        test    eax, eax
        jz      ok1
        int     3

ok1:

        ;
        ; Restore registers.
        ;
        mov     eax, [ebp-04h]
        mov     ecx, [ebp-08h]
        mov     edx, [ebp-0ch]
        mov     ebx, [ebp-10h]

        ;
        ; Call the original function with readonly parameters.
        ;
        call    FUNCTION_NAME_WRAPPED
        push    eax
        push    ecx
        push    edx


        ;
        ; UnFreeze the stack
        ;
        push    3                       ; PAG_READ | PAG_WRITE
        push    [ebp+cb]                ; the size.
        push    [ebp+pv]                ; the pointer.
        call    DosSetMem
        add     esp, 0ch
        test    eax, eax
        jz      ok2
        int     3

ok2:
        ;
        ; Restore registers and return
        ;
        pop     edx
        pop     ecx
        pop     eax
        leave
        ret
FUNCTION_NAME endp


CODE32 ends


        end

