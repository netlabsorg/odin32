        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinFreeErrorInfo:PROC
        PUBLIC  _WinFreeErrorInfo
_WinFreeErrorInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinFreeErrorInfo
    add    esp, 4
    pop    fs
    ret
_WinFreeErrorInfo ENDP

CODE32          ENDS

                END
