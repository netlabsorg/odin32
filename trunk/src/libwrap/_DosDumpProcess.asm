        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDumpProcess:PROC
        PUBLIC  _DosDumpProcess
_DosDumpProcess PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosDumpProcess
    add    esp, 12
    pop    fs
    ret
_DosDumpProcess ENDP

CODE32          ENDS

                END
