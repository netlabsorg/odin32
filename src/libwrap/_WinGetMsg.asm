        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinGetMsg:PROC
        PUBLIC  _WinGetMsg
_WinGetMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinGetMsg
    add    esp, 20
    pop    fs
    ret
_WinGetMsg ENDP

CODE32          ENDS

                END
