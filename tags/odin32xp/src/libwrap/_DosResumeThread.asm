        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosResumeThread:PROC
        PUBLIC  _DosResumeThread
_DosResumeThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosResumeThread
    add    esp, 4
    pop    fs
    ret
_DosResumeThread ENDP

CODE32          ENDS

                END
