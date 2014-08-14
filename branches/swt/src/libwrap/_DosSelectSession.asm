        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSelectSession:PROC
        PUBLIC  _DosSelectSession
_DosSelectSession PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosSelectSession
    add    esp, 4
    pop    fs
    ret
_DosSelectSession ENDP

CODE32          ENDS

                END
