        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinMultWindowFromIDs:PROC
        PUBLIC  _WinMultWindowFromIDs
_WinMultWindowFromIDs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinMultWindowFromIDs
    add    esp, 16
    pop    fs
    ret
_WinMultWindowFromIDs ENDP

CODE32          ENDS

                END
