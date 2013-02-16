        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowThunkProc:PROC
        PUBLIC  _WinQueryWindowThunkProc
_WinQueryWindowThunkProc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryWindowThunkProc
    add    esp, 4
    pop    fs
    ret
_WinQueryWindowThunkProc ENDP

CODE32          ENDS

                END
