        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryDesktopWindow:PROC
        PUBLIC  _WinQueryDesktopWindow
_WinQueryDesktopWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryDesktopWindow
    add    esp, 8
    pop    fs
    ret
_WinQueryDesktopWindow ENDP

CODE32          ENDS

                END
