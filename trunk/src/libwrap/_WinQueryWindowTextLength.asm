        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowTextLength:PROC
        PUBLIC  _WinQueryWindowTextLength
_WinQueryWindowTextLength PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryWindowTextLength
    add    esp, 4
    pop    fs
    ret
_WinQueryWindowTextLength ENDP

CODE32          ENDS

                END
