        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetPMHandleFromGDIHandle:PROC
        PUBLIC  O32_GetPMHandleFromGDIHandle
O32_GetPMHandleFromGDIHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetPMHandleFromGDIHandle
    add    esp, 4
    pop    fs
    ret
O32_GetPMHandleFromGDIHandle ENDP

CODE32          ENDS

                END
