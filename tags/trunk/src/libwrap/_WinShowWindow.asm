        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinShowWindow:PROC
        PUBLIC  _WinShowWindow
_WinShowWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinShowWindow
    add    esp, 8
    pop    fs
    ret
_WinShowWindow ENDP

CODE32          ENDS

                END
