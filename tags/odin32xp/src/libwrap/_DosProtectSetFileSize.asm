        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectSetFileSize:PROC
        PUBLIC  _DosProtectSetFileSize
_DosProtectSetFileSize PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosProtectSetFileSize
    add    esp, 12
    pop    fs
    ret
_DosProtectSetFileSize ENDP

CODE32          ENDS

                END
