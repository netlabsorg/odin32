        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryObjectWindow:PROC
        PUBLIC  _WinQueryObjectWindow
_WinQueryObjectWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryObjectWindow
    add    esp, 4
    pop    fs
    ret
_WinQueryObjectWindow ENDP

CODE32          ENDS

                END
