        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetWindowText:PROC
        PUBLIC  _WinSetWindowText
_WinSetWindowText PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetWindowText
    add    esp, 8
    pop    fs
    ret
_WinSetWindowText ENDP

CODE32          ENDS

                END
