        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQuerySysModalWindow:PROC
        PUBLIC  _WinQuerySysModalWindow
_WinQuerySysModalWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQuerySysModalWindow
    add    esp, 4
    pop    fs
    ret
_WinQuerySysModalWindow ENDP

CODE32          ENDS

                END
