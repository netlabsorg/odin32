        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDelete:PROC
        PUBLIC  _DosDelete
_DosDelete PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosDelete
    add    esp, 4
    pop    fs
    ret
_DosDelete ENDP

CODE32          ENDS

                END
