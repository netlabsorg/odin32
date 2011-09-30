        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxDeregisterExit:PROC
        PUBLIC  _RexxDeregisterExit
_RexxDeregisterExit PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   RexxDeregisterExit
    add    esp, 8
    pop    fs
    ret
_RexxDeregisterExit ENDP

CODE32          ENDS

                END
