        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinWindowFromPoint:PROC
        PUBLIC  _WinWindowFromPoint
_WinWindowFromPoint PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinWindowFromPoint
    add    esp, 12
    pop    fs
    ret
_WinWindowFromPoint ENDP

CODE32          ENDS

                END
