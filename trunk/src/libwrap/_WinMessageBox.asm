        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinMessageBox:PROC
        PUBLIC  _WinMessageBox
_WinMessageBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinMessageBox
    add    esp, 24
    pop    fs
    ret
_WinMessageBox ENDP

CODE32          ENDS

                END
