        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryClassName:PROC
        PUBLIC  _WinQueryClassName
_WinQueryClassName PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinQueryClassName
    add    esp, 12
    pop    fs
    ret
_WinQueryClassName ENDP

CODE32          ENDS

                END
