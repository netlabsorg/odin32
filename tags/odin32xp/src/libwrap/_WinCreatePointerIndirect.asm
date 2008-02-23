        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreatePointerIndirect:PROC
        PUBLIC  _WinCreatePointerIndirect
_WinCreatePointerIndirect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinCreatePointerIndirect
    add    esp, 8
    pop    fs
    ret
_WinCreatePointerIndirect ENDP

CODE32          ENDS

                END
