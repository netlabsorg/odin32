        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCloseQueue:PROC
        PUBLIC  _DosCloseQueue
_DosCloseQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosCloseQueue
    add    esp, 4
    pop    fs
    ret
_DosCloseQueue ENDP

CODE32          ENDS

                END
