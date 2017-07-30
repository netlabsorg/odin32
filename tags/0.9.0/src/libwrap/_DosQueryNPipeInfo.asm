        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryNPipeInfo:PROC
        PUBLIC  _DosQueryNPipeInfo
_DosQueryNPipeInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryNPipeInfo
    add    esp, 16
    pop    fs
    ret
_DosQueryNPipeInfo ENDP

CODE32          ENDS

                END
