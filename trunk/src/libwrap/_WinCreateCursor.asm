        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateCursor:PROC
        PUBLIC  _WinCreateCursor
_WinCreateCursor PROC NEAR
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
    call   WinCreateCursor
    add    esp, 28
    pop    fs
    ret
_WinCreateCursor ENDP

CODE32          ENDS

                END
