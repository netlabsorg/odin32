        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetOwner:PROC
        PUBLIC  _WinSetOwner
_WinSetOwner PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetOwner
    add    esp, 8
    pop    fs
    ret
_WinSetOwner ENDP

CODE32          ENDS

                END
