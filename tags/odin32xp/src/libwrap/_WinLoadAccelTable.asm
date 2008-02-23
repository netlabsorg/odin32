        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLoadAccelTable:PROC
        PUBLIC  _WinLoadAccelTable
_WinLoadAccelTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinLoadAccelTable
    add    esp, 12
    pop    fs
    ret
_WinLoadAccelTable ENDP

CODE32          ENDS

                END
