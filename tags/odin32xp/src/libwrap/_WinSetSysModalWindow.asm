        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetSysModalWindow:PROC
        PUBLIC  _WinSetSysModalWindow
_WinSetSysModalWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetSysModalWindow
    add    esp, 8
    pop    fs
    ret
_WinSetSysModalWindow ENDP

CODE32          ENDS

                END
