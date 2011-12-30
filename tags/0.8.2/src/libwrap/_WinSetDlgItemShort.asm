        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetDlgItemShort:PROC
        PUBLIC  _WinSetDlgItemShort
_WinSetDlgItemShort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinSetDlgItemShort
    add    esp, 16
    pop    fs
    ret
_WinSetDlgItemShort ENDP

CODE32          ENDS

                END
