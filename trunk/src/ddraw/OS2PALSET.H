#ifndef __OS2PALSET_H__
#define __OS2PALSET_H__

#ifndef __OS2PALETTE_H__
 typedef struct tagPALETTEENTRY {
    BYTE        peRed;
    BYTE        peGreen;
    BYTE        peBlue;
    BYTE        peFlags;
 } PALETTEENTRY;
#endif

//void OS2SetPhysPalette(PALETTEENTRY *pDirectXPal);
void OS2SetPhysPalette(void *pal);
void OS2ResetPhysPalette();

#endif
