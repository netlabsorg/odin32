        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryUpdateRect:PROC
        PUBLIC  _WinQueryUpdateRect
_WinQueryUpdateRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryUpdateRect
    add    esp, 8
    pop    fs
    ret
_WinQueryUpdateRect ENDP

CODE32          ENDS

                END
