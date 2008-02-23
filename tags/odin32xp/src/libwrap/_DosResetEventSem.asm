        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosResetEventSem:PROC
        PUBLIC  _DosResetEventSem
_DosResetEventSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosResetEventSem
    add    esp, 8
    pop    fs
    ret
_DosResetEventSem ENDP

CODE32          ENDS

                END
