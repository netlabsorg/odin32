; asmutil.asm Get Correct MS counter
;
; Copyright 1999 Markus Montkowski

                NAME    asmutil
.586p
.MMX

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

PUBLIC _GetMsCount
;----------------------------------------------------------------------------
; ulong GetMSCount (CPU_largeInteger *t1,ULONG freq)
;----------------------------------------------------------------------------
; Get the MS value from the High Tmr values
;----------------------------------------------------------------------------
_GetMsCount PROC NEAR

    t1 EQU  EBP+8
    freq  EQU  EBP+12

    push  ebp
    mov   ebp, esp
    push  ebx
    push  edi
    push  esi

    mov   ecx,[t1]
    mov   eax,[ecx]   ; EAX := t1.low
    mov   edx,[ecx+4]   ; ECX := t1.high
    shld  edx,eax,20
    shl   eax,20      ; t1 * 0x100000
    div   DWORD[freq] ; (t1 * 0x100000) / freq
    mov   ecx,1000000
    xor   edx,edx
    mul   ecx       ; ((t1 * 0x100000) / freq) * 1000000)
    shrd  eax,edx,20    ; ((t1 * 0x100000) / freq) * 1000000) / 0x100000

    pop  edi
    pop  esi
    pop  ebx
    cld
    mov  esp,ebp
    pop  ebp
    ret

_GetMsCount endp




CODE32          ENDS

                END
