        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCloseMuxWaitSem:PROC
        PUBLIC  _DosCloseMuxWaitSem
_DosCloseMuxWaitSem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosCloseMuxWaitSem
    add    esp, 4
    pop    fs
    ret
_DosCloseMuxWaitSem ENDP

CODE32          ENDS

                END
