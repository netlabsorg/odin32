        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgVerifyTypeSet:PROC
        PUBLIC  _DrgVerifyTypeSet
_DrgVerifyTypeSet PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DrgVerifyTypeSet
    add    esp, 16
    pop    fs
    ret
_DrgVerifyTypeSet ENDP

CODE32          ENDS

                END
