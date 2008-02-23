        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDefFileDlgProc:PROC
        PUBLIC  _WinDefFileDlgProc
_WinDefFileDlgProc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinDefFileDlgProc
    add    esp, 16
    pop    fs
    ret
_WinDefFileDlgProc ENDP

CODE32          ENDS

                END
