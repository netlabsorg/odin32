        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCopyAccelTable:PROC
        PUBLIC  _WinCopyAccelTable
_WinCopyAccelTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinCopyAccelTable
    add    esp, 12
    pop    fs
    ret
_WinCopyAccelTable ENDP

CODE32          ENDS

                END
