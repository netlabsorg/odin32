        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxReorderMacro:PROC
        PUBLIC  _RexxReorderMacro
_RexxReorderMacro PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   RexxReorderMacro
    add    esp, 8
    pop    fs
    ret
_RexxReorderMacro ENDP

CODE32          ENDS

                END
