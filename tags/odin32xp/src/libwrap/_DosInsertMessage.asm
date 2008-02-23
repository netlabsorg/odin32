        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosInsertMessage:PROC
        PUBLIC  _DosInsertMessage
_DosInsertMessage PROC NEAR
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
    call   DosInsertMessage
    add    esp, 28
    pop    fs
    ret
_DosInsertMessage ENDP

CODE32          ENDS

                END
