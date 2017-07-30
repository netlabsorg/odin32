        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetFSInfo:PROC
        PUBLIC  _DosSetFSInfo
_DosSetFSInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosSetFSInfo
    add    esp, 16
    pop    fs
    ret
_DosSetFSInfo ENDP

CODE32          ENDS

                END
