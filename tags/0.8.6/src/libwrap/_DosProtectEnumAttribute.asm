        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosProtectEnumAttribute:PROC
        PUBLIC  _DosProtectEnumAttribute
_DosProtectEnumAttribute PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   DosProtectEnumAttribute
    add    esp, 32
    pop    fs
    ret
_DosProtectEnumAttribute ENDP

CODE32          ENDS

                END
