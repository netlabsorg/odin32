        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosWaitNPipe:PROC
        PUBLIC  _DosWaitNPipe
_DosWaitNPipe PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosWaitNPipe
    add    esp, 8
    pop    fs
    ret
_DosWaitNPipe ENDP

CODE32          ENDS

                END
