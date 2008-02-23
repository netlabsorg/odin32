        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryTaskTitle:PROC
        PUBLIC  _WinQueryTaskTitle
_WinQueryTaskTitle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQueryTaskTitle
    add    esp, 12
    pop    fs
    ret
_WinQueryTaskTitle ENDP

CODE32          ENDS

                END
