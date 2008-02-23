        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosConnectNPipe:PROC
        PUBLIC  _DosConnectNPipe
_DosConnectNPipe PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosConnectNPipe
    add    esp, 4
    pop    fs
    ret
_DosConnectNPipe ENDP

CODE32          ENDS

                END
