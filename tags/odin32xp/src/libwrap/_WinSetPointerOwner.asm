        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetPointerOwner:PROC
        PUBLIC  _WinSetPointerOwner
_WinSetPointerOwner PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetPointerOwner
    add    esp, 12
    pop    fs
    ret
_WinSetPointerOwner ENDP

CODE32          ENDS

                END
