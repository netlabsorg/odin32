        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryMessageCP:PROC
        PUBLIC  _DosQueryMessageCP
_DosQueryMessageCP PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosQueryMessageCP
    add    esp, 16
    pop    fs
    ret
_DosQueryMessageCP ENDP

CODE32          ENDS

                END
