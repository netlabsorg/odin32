        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEndPaint:PROC
        PUBLIC  _WinEndPaint
_WinEndPaint PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinEndPaint
    add    esp, 4
    pop    fs
    ret
_WinEndPaint ENDP

CODE32          ENDS

                END
