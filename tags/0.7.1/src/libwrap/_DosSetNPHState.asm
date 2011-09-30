        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetNPHState:PROC
        PUBLIC  _DosSetNPHState
_DosSetNPHState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosSetNPHState
    add    esp, 8
    pop    fs
    ret
_DosSetNPHState ENDP

CODE32          ENDS

                END
