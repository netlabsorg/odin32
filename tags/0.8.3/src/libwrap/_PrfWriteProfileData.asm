        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   PrfWriteProfileData:PROC
        PUBLIC  _PrfWriteProfileData
_PrfWriteProfileData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   PrfWriteProfileData
    add    esp, 20
    pop    fs
    ret
_PrfWriteProfileData ENDP

CODE32          ENDS

                END
