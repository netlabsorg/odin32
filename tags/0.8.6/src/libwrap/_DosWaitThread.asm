        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosWaitThread:PROC
        PUBLIC  _DosWaitThread
_DosWaitThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosWaitThread
    add    esp, 8
    pop    fs
    ret
_DosWaitThread ENDP

CODE32          ENDS

                END
