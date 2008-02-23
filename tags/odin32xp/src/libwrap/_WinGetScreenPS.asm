        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinGetScreenPS:PROC
        PUBLIC  _WinGetScreenPS
_WinGetScreenPS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinGetScreenPS
    add    esp, 4
    pop    fs
    ret
_WinGetScreenPS ENDP

CODE32          ENDS

                END
