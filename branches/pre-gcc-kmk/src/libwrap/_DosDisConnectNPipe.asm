        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDisConnectNPipe:PROC
        PUBLIC  _DosDisConnectNPipe
_DosDisConnectNPipe PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosDisConnectNPipe
    add    esp, 4
    pop    fs
    ret
_DosDisConnectNPipe ENDP

CODE32          ENDS

                END
