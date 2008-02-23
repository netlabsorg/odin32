        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEnumClipbrdFmts:PROC
        PUBLIC  _WinEnumClipbrdFmts
_WinEnumClipbrdFmts PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinEnumClipbrdFmts
    add    esp, 8
    pop    fs
    ret
_WinEnumClipbrdFmts ENDP

CODE32          ENDS

                END
