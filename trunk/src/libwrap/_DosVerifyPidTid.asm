        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosVerifyPidTid:PROC
        PUBLIC  _DosVerifyPidTid
_DosVerifyPidTid PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosVerifyPidTid
    add    esp, 8
    pop    fs
    ret
_DosVerifyPidTid ENDP

CODE32          ENDS

                END
