        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSwitchToProgram:PROC
        PUBLIC  _WinSwitchToProgram
_WinSwitchToProgram PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinSwitchToProgram
    add    esp, 4
    pop    fs
    ret
_WinSwitchToProgram ENDP

CODE32          ENDS

                END
