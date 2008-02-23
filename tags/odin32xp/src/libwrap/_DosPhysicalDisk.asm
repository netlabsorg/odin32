        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosPhysicalDisk:PROC
        PUBLIC  _DosPhysicalDisk
_DosPhysicalDisk PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosPhysicalDisk
    add    esp, 20
    pop    fs
    ret
_DosPhysicalDisk ENDP

CODE32          ENDS

                END
