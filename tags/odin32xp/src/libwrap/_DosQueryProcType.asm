        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryProcType:PROC
        PUBLIC  _DosQueryProcType
_DosQueryProcType PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryProcType
    add    esp, 16
    pop    fs
    ret
_DosQueryProcType ENDP

CODE32          ENDS

                END
