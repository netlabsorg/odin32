        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryStrNameLen:PROC
        PUBLIC  _DrgQueryStrNameLen
_DrgQueryStrNameLen PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgQueryStrNameLen
    add    esp, 4
    pop    fs
    ret
_DrgQueryStrNameLen ENDP

CODE32          ENDS

                END
