        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetWorldTransform:PROC
        PUBLIC  O32_GetWorldTransform
O32_GetWorldTransform PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   _O32_GetWorldTransform
    add    esp, 8
    pop    fs
    ret
O32_GetWorldTransform ENDP

CODE32          ENDS

                END
