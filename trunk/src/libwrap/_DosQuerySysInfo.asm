        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQuerySysInfo:PROC
        PUBLIC  _DosQuerySysInfo
_DosQuerySysInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQuerySysInfo
    add    esp, 16
    pop    fs
    ret
_DosQuerySysInfo ENDP

CODE32          ENDS

                END
