        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCopyMetaFile:PROC
        PUBLIC  _GpiCopyMetaFile
_GpiCopyMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiCopyMetaFile
    add    esp, 4
    pop    fs
    ret
_GpiCopyMetaFile ENDP

CODE32          ENDS

                END
