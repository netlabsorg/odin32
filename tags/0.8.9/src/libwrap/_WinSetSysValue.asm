        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetSysValue:PROC
        PUBLIC  _WinSetSysValue
_WinSetSysValue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinSetSysValue
    add    esp, 12
    pop    fs
    ret
_WinSetSysValue ENDP

CODE32          ENDS

                END
