        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCopyRect:PROC
        PUBLIC  _WinCopyRect
_WinCopyRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinCopyRect
    add    esp, 12
    pop    fs
    ret
_WinCopyRect ENDP

CODE32          ENDS

                END
