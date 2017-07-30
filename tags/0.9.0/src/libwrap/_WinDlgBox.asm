        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDlgBox:PROC
        PUBLIC  _WinDlgBox
_WinDlgBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinDlgBox
    add    esp, 24
    pop    fs
    ret
_WinDlgBox ENDP

CODE32          ENDS

                END
