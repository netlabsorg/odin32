        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateMenu:PROC
        PUBLIC  _WinCreateMenu
_WinCreateMenu PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinCreateMenu
    add    esp, 8
    pop    fs
    ret
_WinCreateMenu ENDP

CODE32          ENDS

                END
