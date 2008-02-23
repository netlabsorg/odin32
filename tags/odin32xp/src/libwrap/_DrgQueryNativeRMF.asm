        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryNativeRMF:PROC
        PUBLIC  _DrgQueryNativeRMF
_DrgQueryNativeRMF PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DrgQueryNativeRMF
    add    esp, 12
    pop    fs
    ret
_DrgQueryNativeRMF ENDP

CODE32          ENDS

                END
