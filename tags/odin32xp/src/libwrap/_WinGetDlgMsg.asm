        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinGetDlgMsg:PROC
        PUBLIC  _WinGetDlgMsg
_WinGetDlgMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinGetDlgMsg
    add    esp, 8
    pop    fs
    ret
_WinGetDlgMsg ENDP

CODE32          ENDS

                END
