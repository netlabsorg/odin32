        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryEventSem:PROC
        PUBLIC  _DosQueryEventSem
_DosQueryEventSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosQueryEventSem
    add    esp, 8
    pop    fs
    ret
_DosQueryEventSem ENDP

CODE32          ENDS

                END
