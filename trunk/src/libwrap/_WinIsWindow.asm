        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsWindow:PROC
        PUBLIC  _WinIsWindow
_WinIsWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinIsWindow
    add    esp, 8
    pop    fs
    ret
_WinIsWindow ENDP

CODE32          ENDS

                END
