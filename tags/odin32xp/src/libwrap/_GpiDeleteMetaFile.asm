        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDeleteMetaFile:PROC
        PUBLIC  _GpiDeleteMetaFile
_GpiDeleteMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiDeleteMetaFile
    add    esp, 4
    pop    fs
    ret
_GpiDeleteMetaFile ENDP

CODE32          ENDS

                END
