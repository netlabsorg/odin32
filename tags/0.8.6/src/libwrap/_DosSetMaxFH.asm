        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetMaxFH:PROC
        PUBLIC  _DosSetMaxFH
_DosSetMaxFH PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosSetMaxFH
    add    esp, 4
    pop    fs
    ret
_DosSetMaxFH ENDP

CODE32          ENDS

                END
