        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinGetLastError:PROC
        PUBLIC  _WinGetLastError
_WinGetLastError PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinGetLastError
    add    esp, 4
    pop    fs
    ret
_WinGetLastError ENDP

CODE32          ENDS

                END
