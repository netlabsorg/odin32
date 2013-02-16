        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinPtInRect:PROC
        PUBLIC  _WinPtInRect
_WinPtInRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinPtInRect
    add    esp, 12
    pop    fs
    ret
_WinPtInRect ENDP

CODE32          ENDS

                END
