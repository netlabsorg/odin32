        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSaveMetaFile:PROC
        PUBLIC  _GpiSaveMetaFile
_GpiSaveMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSaveMetaFile
    add    esp, 8
    pop    fs
    ret
_GpiSaveMetaFile ENDP

CODE32          ENDS

                END
