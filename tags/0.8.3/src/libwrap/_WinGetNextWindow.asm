        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinGetNextWindow:PROC
        PUBLIC  _WinGetNextWindow
_WinGetNextWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinGetNextWindow
    add    esp, 4
    pop    fs
    ret
_WinGetNextWindow ENDP

CODE32          ENDS

                END
