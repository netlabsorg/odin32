        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetMetaFile:PROC
        PUBLIC  O32_GetMetaFile
O32_GetMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetMetaFile
    add    esp, 4
    pop    fs
    ret
O32_GetMetaFile ENDP

CODE32          ENDS

                END
