        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosTransactNPipe:PROC
        PUBLIC  _DosTransactNPipe
_DosTransactNPipe PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   DosTransactNPipe
    add    esp, 24
    pop    fs
    ret
_DosTransactNPipe ENDP

CODE32          ENDS

                END
