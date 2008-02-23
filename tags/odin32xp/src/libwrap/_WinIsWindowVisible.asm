        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsWindowVisible:PROC
        PUBLIC  _WinIsWindowVisible
_WinIsWindowVisible PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinIsWindowVisible
    add    esp, 4
    pop    fs
    ret
_WinIsWindowVisible ENDP

CODE32          ENDS

                END
