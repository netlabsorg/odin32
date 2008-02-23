        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinFlashWindow:PROC
        PUBLIC  _WinFlashWindow
_WinFlashWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinFlashWindow
    add    esp, 8
    pop    fs
    ret
_WinFlashWindow ENDP

CODE32          ENDS

                END
