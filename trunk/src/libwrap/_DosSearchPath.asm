        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSearchPath:PROC
        PUBLIC  _DosSearchPath
_DosSearchPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosSearchPath
    add    esp, 20
    pop    fs
    ret
_DosSearchPath ENDP

CODE32          ENDS

                END
