        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxStart:PROC
        PUBLIC  _RexxStart
_RexxStart PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    call   RexxStart
    add    esp, 36
    pop    fs
    ret
_RexxStart ENDP

CODE32          ENDS

                END
