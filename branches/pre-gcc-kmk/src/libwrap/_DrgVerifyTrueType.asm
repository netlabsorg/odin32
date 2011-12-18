        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgVerifyTrueType:PROC
        PUBLIC  _DrgVerifyTrueType
_DrgVerifyTrueType PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DrgVerifyTrueType
    add    esp, 8
    pop    fs
    ret
_DrgVerifyTrueType ENDP

CODE32          ENDS

                END
