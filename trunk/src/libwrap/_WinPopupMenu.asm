        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinPopupMenu:PROC
        PUBLIC  _WinPopupMenu
_WinPopupMenu PROC NEAR
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
    call   WinPopupMenu
    add    esp, 28
    pop    fs
    ret
_WinPopupMenu ENDP

CODE32          ENDS

                END
