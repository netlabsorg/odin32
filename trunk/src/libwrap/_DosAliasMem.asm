        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosAliasMem:PROC
        PUBLIC  _DosAliasMem
_DosAliasMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosAliasMem
    add    esp, 16
    pop    fs
    ret
_DosAliasMem ENDP

CODE32          ENDS

                END
