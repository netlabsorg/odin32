        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsWindowShowing:PROC
        PUBLIC  _WinIsWindowShowing
_WinIsWindowShowing PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinIsWindowShowing
    add    esp, 4
    pop    fs
    ret
_WinIsWindowShowing ENDP

CODE32          ENDS

                END
