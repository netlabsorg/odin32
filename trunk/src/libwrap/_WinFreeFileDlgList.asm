        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinFreeFileDlgList:PROC
        PUBLIC  _WinFreeFileDlgList
_WinFreeFileDlgList PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinFreeFileDlgList
    add    esp, 4
    pop    fs
    ret
_WinFreeFileDlgList ENDP

CODE32          ENDS

                END
