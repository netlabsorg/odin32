        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiLoadMetaFile:PROC
        PUBLIC  _GpiLoadMetaFile
_GpiLoadMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiLoadMetaFile
    add    esp, 8
    pop    fs
    ret
_GpiLoadMetaFile ENDP

CODE32          ENDS

                END
