        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEndEnumWindows:PROC
        PUBLIC  _WinEndEnumWindows
_WinEndEnumWindows PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinEndEnumWindows
    add    esp, 4
    pop    fs
    ret
_WinEndEnumWindows ENDP

CODE32          ENDS

                END
