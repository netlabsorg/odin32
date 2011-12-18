        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryPointerInfo:PROC
        PUBLIC  _WinQueryPointerInfo
_WinQueryPointerInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryPointerInfo
    add    esp, 8
    pop    fs
    ret
_WinQueryPointerInfo ENDP

CODE32          ENDS

                END
