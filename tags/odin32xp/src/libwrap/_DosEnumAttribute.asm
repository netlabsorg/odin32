        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosEnumAttribute:PROC
        PUBLIC  _DosEnumAttribute
_DosEnumAttribute PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   DosEnumAttribute
    add    esp, 28
    pop    fs
    ret
_DosEnumAttribute ENDP

CODE32          ENDS

                END
