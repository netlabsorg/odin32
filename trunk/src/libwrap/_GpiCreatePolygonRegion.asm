        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreatePolygonRegion:PROC
        PUBLIC  _GpiCreatePolygonRegion
_GpiCreatePolygonRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiCreatePolygonRegion
    add    esp, 16
    pop    fs
    ret
_GpiCreatePolygonRegion ENDP

CODE32          ENDS

                END
