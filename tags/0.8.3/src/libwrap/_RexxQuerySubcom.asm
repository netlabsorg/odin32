        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxQuerySubcom:PROC
        PUBLIC  _RexxQuerySubcom
_RexxQuerySubcom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   RexxQuerySubcom
    add    esp, 16
    pop    fs
    ret
_RexxQuerySubcom ENDP

CODE32          ENDS

                END
