        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryMuxWaitSem:PROC
        PUBLIC  _DosQueryMuxWaitSem
_DosQueryMuxWaitSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryMuxWaitSem
    add    esp, 16
    pop    fs
    ret
_DosQueryMuxWaitSem ENDP

CODE32          ENDS

                END
