        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinChangeSwitchEntry:PROC
        PUBLIC  _WinChangeSwitchEntry
_WinChangeSwitchEntry PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinChangeSwitchEntry
    add    esp, 8
    pop    fs
    ret
_WinChangeSwitchEntry ENDP

CODE32          ENDS

                END
