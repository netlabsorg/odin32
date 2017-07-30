        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEnableWindow:PROC
        PUBLIC  _WinEnableWindow
_WinEnableWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinEnableWindow
    add    esp, 8
    pop    fs
    ret
_WinEnableWindow ENDP

CODE32          ENDS

                END
