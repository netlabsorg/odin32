        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateDlg:PROC
        PUBLIC  _WinCreateDlg
_WinCreateDlg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinCreateDlg
    add    esp, 20
    pop    fs
    ret
_WinCreateDlg ENDP

CODE32          ENDS

                END
