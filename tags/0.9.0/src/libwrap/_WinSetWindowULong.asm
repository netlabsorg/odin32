        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetWindowULong:PROC
        PUBLIC  _WinSetWindowULong
_WinSetWindowULong PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetWindowULong
    add    esp, 12
    pop    fs
    ret
_WinSetWindowULong ENDP

CODE32          ENDS

                END
