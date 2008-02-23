        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetDesktopBkgnd:PROC
        PUBLIC  _WinSetDesktopBkgnd
_WinSetDesktopBkgnd PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetDesktopBkgnd
    add    esp, 8
    pop    fs
    ret
_WinSetDesktopBkgnd ENDP

CODE32          ENDS

                END
