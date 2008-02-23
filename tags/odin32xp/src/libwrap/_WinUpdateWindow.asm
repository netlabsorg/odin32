        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinUpdateWindow:PROC
        PUBLIC  _WinUpdateWindow
_WinUpdateWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinUpdateWindow
    add    esp, 4
    pop    fs
    ret
_WinUpdateWindow ENDP

CODE32          ENDS

                END
