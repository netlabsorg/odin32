        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EnumEnhMetaFile:PROC
        PUBLIC  O32_EnumEnhMetaFile
O32_EnumEnhMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   _O32_EnumEnhMetaFile
    add    esp, 20
    pop    fs
    ret
O32_EnumEnhMetaFile ENDP

CODE32          ENDS

                END
