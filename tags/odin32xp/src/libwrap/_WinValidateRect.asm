        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinValidateRect:PROC
        PUBLIC  _WinValidateRect
_WinValidateRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinValidateRect
    add    esp, 12
    pop    fs
    ret
_WinValidateRect ENDP

CODE32          ENDS

                END
