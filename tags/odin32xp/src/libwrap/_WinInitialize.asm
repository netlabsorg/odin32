        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinInitialize:PROC
        PUBLIC  _WinInitialize
_WinInitialize PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinInitialize
    add    esp, 4
    pop    fs
    ret
_WinInitialize ENDP

CODE32          ENDS

                END
