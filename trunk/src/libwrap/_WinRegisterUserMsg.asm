        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRegisterUserMsg:PROC
        PUBLIC  _WinRegisterUserMsg
_WinRegisterUserMsg PROC NEAR
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
    call   WinRegisterUserMsg
    add    esp, 28
    pop    fs
    ret
_WinRegisterUserMsg ENDP

CODE32          ENDS

                END
