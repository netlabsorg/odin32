        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowText:PROC
        PUBLIC  _WinQueryWindowText
_WinQueryWindowText PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQueryWindowText
    add    esp, 12
    pop    fs
    ret
_WinQueryWindowText ENDP

CODE32          ENDS

                END
