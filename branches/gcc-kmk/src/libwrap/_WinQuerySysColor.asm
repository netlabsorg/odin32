        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQuerySysColor:PROC
        PUBLIC  _WinQuerySysColor
_WinQuerySysColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQuerySysColor
    add    esp, 12
    pop    fs
    ret
_WinQuerySysColor ENDP

CODE32          ENDS

                END
