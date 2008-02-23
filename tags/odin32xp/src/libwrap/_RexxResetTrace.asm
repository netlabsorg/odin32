        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxResetTrace:PROC
        PUBLIC  _RexxResetTrace
_RexxResetTrace PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   RexxResetTrace
    add    esp, 8
    pop    fs
    ret
_RexxResetTrace ENDP

CODE32          ENDS

                END
