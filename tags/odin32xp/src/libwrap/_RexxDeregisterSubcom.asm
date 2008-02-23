        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxDeregisterSubcom:PROC
        PUBLIC  _RexxDeregisterSubcom
_RexxDeregisterSubcom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   RexxDeregisterSubcom
    add    esp, 8
    pop    fs
    ret
_RexxDeregisterSubcom ENDP

CODE32          ENDS

                END
