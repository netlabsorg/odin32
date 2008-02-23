        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryMsgPos:PROC
        PUBLIC  _WinQueryMsgPos
_WinQueryMsgPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryMsgPos
    add    esp, 8
    pop    fs
    ret
_WinQueryMsgPos ENDP

CODE32          ENDS

                END
