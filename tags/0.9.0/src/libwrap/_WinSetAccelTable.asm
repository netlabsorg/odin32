        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetAccelTable:PROC
        PUBLIC  _WinSetAccelTable
_WinSetAccelTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetAccelTable
    add    esp, 12
    pop    fs
    ret
_WinSetAccelTable ENDP

CODE32          ENDS

                END
