        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetVolumeInformation:PROC
        PUBLIC  O32_GetVolumeInformation
O32_GetVolumeInformation PROC NEAR
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
    call   _O32_GetVolumeInformation
    add    esp, 32
    pop    fs
    ret
O32_GetVolumeInformation ENDP

CODE32          ENDS

                END
