        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRemoveSwitchEntry:PROC
        PUBLIC  _WinRemoveSwitchEntry
_WinRemoveSwitchEntry PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinRemoveSwitchEntry
    add    esp, 4
    pop    fs
    ret
_WinRemoveSwitchEntry ENDP

CODE32          ENDS

                END
