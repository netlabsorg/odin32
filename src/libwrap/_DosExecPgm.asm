        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosExecPgm:PROC
        PUBLIC  _DosExecPgm
_DosExecPgm PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   DosExecPgm
    add    esp, 28
    pop    fs
    ret
_DosExecPgm ENDP

CODE32          ENDS

                END
