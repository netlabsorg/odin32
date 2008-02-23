        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinShowCursor:PROC
        PUBLIC  _WinShowCursor
_WinShowCursor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinShowCursor
    add    esp, 8
    pop    fs
    ret
_WinShowCursor ENDP

CODE32          ENDS

                END
