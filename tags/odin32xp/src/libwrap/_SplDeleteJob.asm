        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplDeleteJob:PROC
        PUBLIC  _SplDeleteJob
_SplDeleteJob PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   SplDeleteJob
    add    esp, 12
    pop    fs
    ret
_SplDeleteJob ENDP

CODE32          ENDS

                END
