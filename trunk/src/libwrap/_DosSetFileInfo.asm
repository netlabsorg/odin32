        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetFileInfo:PROC
        PUBLIC  _DosSetFileInfo
_DosSetFileInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosSetFileInfo
    add    esp, 16
    pop    fs
    ret
_DosSetFileInfo ENDP

CODE32          ENDS

                END
