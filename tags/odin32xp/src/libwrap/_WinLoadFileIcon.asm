        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinLoadFileIcon:PROC
        PUBLIC  _WinLoadFileIcon
_WinLoadFileIcon PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinLoadFileIcon
    add    esp, 8
    pop    fs
    ret
_WinLoadFileIcon ENDP

CODE32          ENDS

                END
