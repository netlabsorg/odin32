        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEnumDlgItem:PROC
        PUBLIC  _WinEnumDlgItem
_WinEnumDlgItem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinEnumDlgItem
    add    esp, 12
    pop    fs
    ret
_WinEnumDlgItem ENDP

CODE32          ENDS

                END
