        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateShadow:PROC
        PUBLIC  _WinCreateShadow
_WinCreateShadow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinCreateShadow
    add    esp, 12
    pop    fs
    ret
_WinCreateShadow ENDP

CODE32          ENDS

                END
