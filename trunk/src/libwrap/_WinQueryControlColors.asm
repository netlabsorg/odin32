        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryControlColors:PROC
        PUBLIC  _WinQueryControlColors
_WinQueryControlColors PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinQueryControlColors
    add    esp, 20
    pop    fs
    ret
_WinQueryControlColors ENDP

CODE32          ENDS

                END
