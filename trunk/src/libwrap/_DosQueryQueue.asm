        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryQueue:PROC
        PUBLIC  _DosQueryQueue
_DosQueryQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosQueryQueue
    add    esp, 8
    pop    fs
    ret
_DosQueryQueue ENDP

CODE32          ENDS

                END
