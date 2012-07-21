        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryCursorInfo:PROC
        PUBLIC  _WinQueryCursorInfo
_WinQueryCursorInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryCursorInfo
    add    esp, 8
    pop    fs
    ret
_WinQueryCursorInfo ENDP

CODE32          ENDS

                END
