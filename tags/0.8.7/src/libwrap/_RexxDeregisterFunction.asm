        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxDeregisterFunction:PROC
        PUBLIC  _RexxDeregisterFunction
_RexxDeregisterFunction PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   RexxDeregisterFunction
    add    esp, 4
    pop    fs
    ret
_RexxDeregisterFunction ENDP

CODE32          ENDS

                END
