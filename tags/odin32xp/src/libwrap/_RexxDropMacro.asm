        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxDropMacro:PROC
        PUBLIC  _RexxDropMacro
_RexxDropMacro PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   RexxDropMacro
    add    esp, 4
    pop    fs
    ret
_RexxDropMacro ENDP

CODE32          ENDS

                END
