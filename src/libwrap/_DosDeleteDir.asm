        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDeleteDir:PROC
        PUBLIC  _DosDeleteDir
_DosDeleteDir PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosDeleteDir
    add    esp, 4
    pop    fs
    ret
_DosDeleteDir ENDP

CODE32          ENDS

                END
