        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryAccelTable:PROC
        PUBLIC  _WinQueryAccelTable
_WinQueryAccelTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryAccelTable
    add    esp, 8
    pop    fs
    ret
_WinQueryAccelTable ENDP

CODE32          ENDS

                END
