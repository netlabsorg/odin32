        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSendMsg:PROC
        PUBLIC  _WinSendMsg
_WinSendMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinSendMsg
    add    esp, 16
    pop    fs
    ret
_WinSendMsg ENDP

CODE32          ENDS

                END
