        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDrawBorder:PROC
        PUBLIC  _WinDrawBorder
_WinDrawBorder PROC NEAR
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
    call   WinDrawBorder
    add    esp, 28
    pop    fs
    ret
_WinDrawBorder ENDP

CODE32          ENDS

                END
