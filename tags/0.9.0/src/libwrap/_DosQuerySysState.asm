        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQuerySysState:PROC
        PUBLIC  _DosQuerySysState
_DosQuerySysState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   DosQuerySysState
    add    esp, 24
    pop    fs
    ret
_DosQuerySysState ENDP

CODE32          ENDS

                END
