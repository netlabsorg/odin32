        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryNPHState:PROC
        PUBLIC  _DosQueryNPHState
_DosQueryNPHState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosQueryNPHState
    add    esp, 8
    pop    fs
    ret
_DosQueryNPHState ENDP

CODE32          ENDS

                END
