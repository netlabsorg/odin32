        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryDOSProperty:PROC
        PUBLIC  _DosQueryDOSProperty
_DosQueryDOSProperty PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryDOSProperty
    add    esp, 16
    pop    fs
    ret
_DosQueryDOSProperty ENDP

CODE32          ENDS

                END
