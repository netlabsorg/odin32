        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CloseMetaFile:PROC
        PUBLIC  O32_CloseMetaFile
O32_CloseMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CloseMetaFile
    add    esp, 4
    pop    fs
    ret
O32_CloseMetaFile ENDP

CODE32          ENDS

                END
