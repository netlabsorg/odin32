        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCreateThread:PROC
        PUBLIC  _DosCreateThread
_DosCreateThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosCreateThread
    add    esp, 20
    pop    fs
    ret
_DosCreateThread ENDP

CODE32          ENDS

                END
