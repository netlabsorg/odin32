        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinBeginPaint:PROC
        PUBLIC  _WinBeginPaint
_WinBeginPaint PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinBeginPaint
    add    esp, 12
    pop    fs
    ret
_WinBeginPaint ENDP

CODE32          ENDS

                END
