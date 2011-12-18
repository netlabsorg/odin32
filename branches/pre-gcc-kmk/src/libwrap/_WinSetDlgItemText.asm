        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetDlgItemText:PROC
        PUBLIC  _WinSetDlgItemText
_WinSetDlgItemText PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetDlgItemText
    add    esp, 12
    pop    fs
    ret
_WinSetDlgItemText ENDP

CODE32          ENDS

                END
