        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPlayMetaFile:PROC
        PUBLIC  _GpiPlayMetaFile
_GpiPlayMetaFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   GpiPlayMetaFile
    add    esp, 28
    pop    fs
    ret
_GpiPlayMetaFile ENDP

CODE32          ENDS

                END
