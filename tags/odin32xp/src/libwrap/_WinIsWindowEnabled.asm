        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsWindowEnabled:PROC
        PUBLIC  _WinIsWindowEnabled
_WinIsWindowEnabled PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinIsWindowEnabled
    add    esp, 4
    pop    fs
    ret
_WinIsWindowEnabled ENDP

CODE32          ENDS

                END
