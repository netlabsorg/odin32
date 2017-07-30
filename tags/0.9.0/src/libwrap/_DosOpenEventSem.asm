        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosOpenEventSem:PROC
        PUBLIC  _DosOpenEventSem
_DosOpenEventSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosOpenEventSem
    add    esp, 8
    pop    fs
    ret
_DosOpenEventSem ENDP

CODE32          ENDS

                END
