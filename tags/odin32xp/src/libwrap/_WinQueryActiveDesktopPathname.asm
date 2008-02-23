        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryActiveDesktopPathname:PROC
        PUBLIC  _WinQueryActiveDesktopPathname
_WinQueryActiveDesktopPathname PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryActiveDesktopPathname
    add    esp, 8
    pop    fs
    ret
_WinQueryActiveDesktopPathname ENDP

CODE32          ENDS

                END
