        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetFocus:PROC
        PUBLIC  _WinSetFocus
_WinSetFocus PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetFocus
    add    esp, 8
    pop    fs
    ret
_WinSetFocus ENDP

CODE32          ENDS

                END
