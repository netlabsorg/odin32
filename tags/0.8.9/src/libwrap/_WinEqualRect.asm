        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEqualRect:PROC
        PUBLIC  _WinEqualRect
_WinEqualRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinEqualRect
    add    esp, 12
    pop    fs
    ret
_WinEqualRect ENDP

CODE32          ENDS

                END
