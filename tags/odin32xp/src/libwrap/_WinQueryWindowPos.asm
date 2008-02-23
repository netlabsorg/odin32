        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowPos:PROC
        PUBLIC  _WinQueryWindowPos
_WinQueryWindowPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryWindowPos
    add    esp, 8
    pop    fs
    ret
_WinQueryWindowPos ENDP

CODE32          ENDS

                END
