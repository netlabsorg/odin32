        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryClassThunkProc:PROC
        PUBLIC  _WinQueryClassThunkProc
_WinQueryClassThunkProc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryClassThunkProc
    add    esp, 4
    pop    fs
    ret
_WinQueryClassThunkProc ENDP

CODE32          ENDS

                END
