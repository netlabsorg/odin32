        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetRect:PROC
        PUBLIC  _WinSetRect
_WinSetRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinSetRect
    add    esp, 24
    pop    fs
    ret
_WinSetRect ENDP

CODE32          ENDS

                END
