        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLoadMenu:PROC
        PUBLIC  _WinLoadMenu
_WinLoadMenu PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinLoadMenu
    add    esp, 12
    pop    fs
    ret
_WinLoadMenu ENDP

CODE32          ENDS

                END
