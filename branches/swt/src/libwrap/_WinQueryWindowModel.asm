        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryWindowModel:PROC
        PUBLIC  _WinQueryWindowModel
_WinQueryWindowModel PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryWindowModel
    add    esp, 4
    pop    fs
    ret
_WinQueryWindowModel ENDP

CODE32          ENDS

                END
