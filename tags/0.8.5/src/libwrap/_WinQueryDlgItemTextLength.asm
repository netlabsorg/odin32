        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryDlgItemTextLength:PROC
        PUBLIC  _WinQueryDlgItemTextLength
_WinQueryDlgItemTextLength PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryDlgItemTextLength
    add    esp, 8
    pop    fs
    ret
_WinQueryDlgItemTextLength ENDP

CODE32          ENDS

                END
