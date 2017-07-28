        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosForceDelete:PROC
        PUBLIC  _DosForceDelete
_DosForceDelete PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosForceDelete
    add    esp, 4
    pop    fs
    ret
_DosForceDelete ENDP

CODE32          ENDS

                END
