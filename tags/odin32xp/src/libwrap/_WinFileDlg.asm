        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinFileDlg:PROC
        PUBLIC  _WinFileDlg
_WinFileDlg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinFileDlg
    add    esp, 12
    pop    fs
    ret
_WinFileDlg ENDP

CODE32          ENDS

                END
