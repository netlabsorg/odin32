        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDeleteMuxWaitSem:PROC
        PUBLIC  _DosDeleteMuxWaitSem
_DosDeleteMuxWaitSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosDeleteMuxWaitSem
    add    esp, 8
    pop    fs
    ret
_DosDeleteMuxWaitSem ENDP

CODE32          ENDS

                END
