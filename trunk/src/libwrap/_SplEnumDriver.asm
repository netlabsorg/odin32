        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplEnumDriver:PROC
        PUBLIC  _SplEnumDriver
_SplEnumDriver PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   SplEnumDriver
    add    esp, 32
    pop    fs
    ret
_SplEnumDriver ENDP

CODE32          ENDS

                END
