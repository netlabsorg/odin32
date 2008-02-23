        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosStopSession:PROC
        PUBLIC  _DosStopSession
_DosStopSession PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosStopSession
    add    esp, 8
    pop    fs
    ret
_DosStopSession ENDP

CODE32          ENDS

                END
