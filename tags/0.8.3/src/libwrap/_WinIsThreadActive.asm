        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsThreadActive:PROC
        PUBLIC  _WinIsThreadActive
_WinIsThreadActive PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinIsThreadActive
    add    esp, 4
    pop    fs
    ret
_WinIsThreadActive ENDP

CODE32          ENDS

                END
