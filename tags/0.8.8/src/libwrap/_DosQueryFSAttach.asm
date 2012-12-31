        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryFSAttach:PROC
        PUBLIC  _DosQueryFSAttach
_DosQueryFSAttach PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosQueryFSAttach
    add    esp, 20
    pop    fs
    ret
_DosQueryFSAttach ENDP

CODE32          ENDS

                END
