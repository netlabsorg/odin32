        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetMsgMode:PROC
        PUBLIC  _WinSetMsgMode
_WinSetMsgMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetMsgMode
    add    esp, 12
    pop    fs
    ret
_WinSetMsgMode ENDP

CODE32          ENDS

                END
