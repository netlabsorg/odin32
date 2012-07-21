        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQuerySessionTitle:PROC
        PUBLIC  _WinQuerySessionTitle
_WinQuerySessionTitle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinQuerySessionTitle
    add    esp, 16
    pop    fs
    ret
_WinQuerySessionTitle ENDP

CODE32          ENDS

                END
