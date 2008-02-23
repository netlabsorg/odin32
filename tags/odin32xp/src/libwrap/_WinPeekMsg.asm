        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinPeekMsg:PROC
        PUBLIC  _WinPeekMsg
_WinPeekMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinPeekMsg
    add    esp, 24
    pop    fs
    ret
_WinPeekMsg ENDP

CODE32          ENDS

                END
