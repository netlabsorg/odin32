        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxSetTrace:PROC
        PUBLIC  _RexxSetTrace
_RexxSetTrace PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   RexxSetTrace
    add    esp, 8
    pop    fs
    ret
_RexxSetTrace ENDP

CODE32          ENDS

                END
