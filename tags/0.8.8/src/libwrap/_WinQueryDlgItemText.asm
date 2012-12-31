        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryDlgItemText:PROC
        PUBLIC  _WinQueryDlgItemText
_WinQueryDlgItemText PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinQueryDlgItemText
    add    esp, 16
    pop    fs
    ret
_WinQueryDlgItemText ENDP

CODE32          ENDS

                END
