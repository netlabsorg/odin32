        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryHType:PROC
        PUBLIC  _DosQueryHType
_DosQueryHType PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosQueryHType
    add    esp, 12
    pop    fs
    ret
_DosQueryHType ENDP

CODE32          ENDS

                END
