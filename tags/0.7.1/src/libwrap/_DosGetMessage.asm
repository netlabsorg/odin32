        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosGetMessage:PROC
        PUBLIC  _DosGetMessage
_DosGetMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   DosGetMessage
    add    esp, 28
    pop    fs
    ret
_DosGetMessage ENDP

CODE32          ENDS

                END
