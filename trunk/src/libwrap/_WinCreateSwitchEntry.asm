        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateSwitchEntry:PROC
        PUBLIC  _WinCreateSwitchEntry
_WinCreateSwitchEntry PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinCreateSwitchEntry
    add    esp, 8
    pop    fs
    ret
_WinCreateSwitchEntry ENDP

CODE32          ENDS

                END
