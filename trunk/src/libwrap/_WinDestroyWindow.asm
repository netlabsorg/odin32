        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDestroyWindow:PROC
        PUBLIC  _WinDestroyWindow
_WinDestroyWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinDestroyWindow
    add    esp, 4
    pop    fs
    ret
_WinDestroyWindow ENDP

CODE32          ENDS

                END
