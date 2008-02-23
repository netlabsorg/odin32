        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinWindowFromID:PROC
        PUBLIC  _WinWindowFromID
_WinWindowFromID PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinWindowFromID
    add    esp, 8
    pop    fs
    ret
_WinWindowFromID ENDP

CODE32          ENDS

                END
