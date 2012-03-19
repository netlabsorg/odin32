        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowUShort:PROC
        PUBLIC  _WinQueryWindowUShort
_WinQueryWindowUShort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryWindowUShort
    add    esp, 8
    pop    fs
    ret
_WinQueryWindowUShort ENDP

CODE32          ENDS

                END
