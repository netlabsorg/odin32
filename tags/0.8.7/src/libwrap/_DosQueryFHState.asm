        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryFHState:PROC
        PUBLIC  _DosQueryFHState
_DosQueryFHState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosQueryFHState
    add    esp, 8
    pop    fs
    ret
_DosQueryFHState ENDP

CODE32          ENDS

                END
