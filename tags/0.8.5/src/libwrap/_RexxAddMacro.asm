        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxAddMacro:PROC
        PUBLIC  _RexxAddMacro
_RexxAddMacro PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   RexxAddMacro
    add    esp, 12
    pop    fs
    ret
_RexxAddMacro ENDP

CODE32          ENDS

                END
