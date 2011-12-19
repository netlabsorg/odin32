        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRestartWPDServer:PROC
        PUBLIC  _WinRestartWPDServer
_WinRestartWPDServer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinRestartWPDServer
    add    esp, 4
    pop    fs
    ret
_WinRestartWPDServer ENDP

CODE32          ENDS

                END
