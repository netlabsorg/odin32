        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetRectEmpty:PROC
        PUBLIC  _WinSetRectEmpty
_WinSetRectEmpty PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetRectEmpty
    add    esp, 8
    pop    fs
    ret
_WinSetRectEmpty ENDP

CODE32          ENDS

                END
