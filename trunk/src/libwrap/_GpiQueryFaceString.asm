        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryFaceString:PROC
        PUBLIC  _GpiQueryFaceString
_GpiQueryFaceString PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryFaceString
    add    esp, 20
    pop    fs
    ret
_GpiQueryFaceString ENDP

CODE32          ENDS

                END
