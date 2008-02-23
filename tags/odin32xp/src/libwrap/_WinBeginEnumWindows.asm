        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinBeginEnumWindows:PROC
        PUBLIC  _WinBeginEnumWindows
_WinBeginEnumWindows PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinBeginEnumWindows
    add    esp, 4
    pop    fs
    ret
_WinBeginEnumWindows ENDP

CODE32          ENDS

                END
