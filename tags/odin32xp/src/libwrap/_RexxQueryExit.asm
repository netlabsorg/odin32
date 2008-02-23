        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxQueryExit:PROC
        PUBLIC  _RexxQueryExit
_RexxQueryExit PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   RexxQueryExit
    add    esp, 16
    pop    fs
    ret
_RexxQueryExit ENDP

CODE32          ENDS

                END
