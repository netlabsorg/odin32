        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosScanEnv:PROC
        PUBLIC  _DosScanEnv
_DosScanEnv PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosScanEnv
    add    esp, 8
    pop    fs
    ret
_DosScanEnv ENDP

CODE32          ENDS

                END
