        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowProcess:PROC
        PUBLIC  _WinQueryWindowProcess
_WinQueryWindowProcess PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQueryWindowProcess
    add    esp, 12
    pop    fs
    ret
_WinQueryWindowProcess ENDP

CODE32          ENDS

                END
