        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinGetErrorInfo:PROC
        PUBLIC  _WinGetErrorInfo
_WinGetErrorInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinGetErrorInfo
    add    esp, 4
    pop    fs
    ret
_WinGetErrorInfo ENDP

CODE32          ENDS

                END
