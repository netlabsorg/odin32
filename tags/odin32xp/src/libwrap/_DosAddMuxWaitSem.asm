        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosAddMuxWaitSem:PROC
        PUBLIC  _DosAddMuxWaitSem
_DosAddMuxWaitSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosAddMuxWaitSem
    add    esp, 8
    pop    fs
    ret
_DosAddMuxWaitSem ENDP

CODE32          ENDS

                END
