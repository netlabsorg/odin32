        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _OpenClipbrd:PROC
        PUBLIC  __OpenClipbrd
__OpenClipbrd PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   _OpenClipbrd
    add    esp, 8
    pop    fs
    ret
__OpenClipbrd ENDP

CODE32          ENDS

                END
