        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryCurrentDisk:PROC
        PUBLIC  _DosQueryCurrentDisk
_DosQueryCurrentDisk PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosQueryCurrentDisk
    add    esp, 8
    pop    fs
    ret
_DosQueryCurrentDisk ENDP

CODE32          ENDS

                END
