        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DeleteEnhMetaFile:PROC
        PUBLIC  O32_DeleteEnhMetaFile
O32_DeleteEnhMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DeleteEnhMetaFile
    add    esp, 4
    pop    fs
    ret
O32_DeleteEnhMetaFile ENDP

CODE32          ENDS

                END
