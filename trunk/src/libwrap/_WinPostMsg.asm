        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinPostMsg:PROC
        PUBLIC  _WinPostMsg
_WinPostMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinPostMsg
    add    esp, 16
    pop    fs
    ret
_WinPostMsg ENDP

CODE32          ENDS

                END
