        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinInvalidateRegion:PROC
        PUBLIC  _WinInvalidateRegion
_WinInvalidateRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinInvalidateRegion
    add    esp, 12
    pop    fs
    ret
_WinInvalidateRegion ENDP

CODE32          ENDS

                END
