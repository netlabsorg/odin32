        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinReplyMsg:PROC
        PUBLIC  _WinReplyMsg
_WinReplyMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinReplyMsg
    add    esp, 16
    pop    fs
    ret
_WinReplyMsg ENDP

CODE32          ENDS

                END
