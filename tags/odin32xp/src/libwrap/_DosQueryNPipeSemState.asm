        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryNPipeSemState:PROC
        PUBLIC  _DosQueryNPipeSemState
_DosQueryNPipeSemState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosQueryNPipeSemState
    add    esp, 12
    pop    fs
    ret
_DosQueryNPipeSemState ENDP

CODE32          ENDS

                END
