        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinMessageBox2:PROC
        PUBLIC  _WinMessageBox2
_WinMessageBox2 PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinMessageBox2
    add    esp, 24
    pop    fs
    ret
_WinMessageBox2 ENDP

CODE32          ENDS

                END
