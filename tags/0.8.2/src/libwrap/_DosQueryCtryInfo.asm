        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryCtryInfo:PROC
        PUBLIC  _DosQueryCtryInfo
_DosQueryCtryInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryCtryInfo
    add    esp, 16
    pop    fs
    ret
_DosQueryCtryInfo ENDP

CODE32          ENDS

                END
