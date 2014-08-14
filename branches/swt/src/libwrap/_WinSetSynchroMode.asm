        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetSynchroMode:PROC
        PUBLIC  _WinSetSynchroMode
_WinSetSynchroMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetSynchroMode
    add    esp, 8
    pop    fs
    ret
_WinSetSynchroMode ENDP

CODE32          ENDS

                END
