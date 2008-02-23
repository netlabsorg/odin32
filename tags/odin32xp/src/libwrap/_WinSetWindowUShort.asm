        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetWindowUShort:PROC
        PUBLIC  _WinSetWindowUShort
_WinSetWindowUShort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetWindowUShort
    add    esp, 12
    pop    fs
    ret
_WinSetWindowUShort ENDP

CODE32          ENDS

                END
