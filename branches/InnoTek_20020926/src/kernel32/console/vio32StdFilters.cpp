#define INCL_DOS
#define INCL_PM
#define INCL_WIN
#define INCL_VIO
#define INCL_SUB
#define INCL_BSE
#define INCL_DOSERRORS
#define INCL_DOSDEVICES
#include <os2.h>
#include <bsedev.h>
#include <stdio.h>
#include <stdlib.h>

#include "vio32Client.h"
#include "vio32Private.h"
#include "vio32Utils.h"
#include "vio32SharedData.h"
#include "vio32Errors.h"


// scan to vkey  it seems that vkey is a sole function of the kbd (NUMLOCK OFF)
char vKeyMap1[256]= {
    0,  27,  49,  50,  51,  52,  53,  54,  55,  56,  57,  48, 189, 187,   8,   9, //0
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 219, 221,  13,  17,   0,   0, //16
    0,   0,   0,   0,   0,   0,   0, 186, 192, 223,  16, 222,   0,   0,   0,   0, //32
    0,   0,   0, 188, 190, 191,  16, 106,  18,  32,  20, 112, 113, 114, 115, 116, //48
  117, 118, 119, 120, 121, 144, 145,  36,  38,  33, 109,  37, 101,  39, 107,  35, //64
   40,  34,  45,  46,   0,   0, 220, 122, 123,   0,   0,  91,  92,  93,   0,   0, //80
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// scan to vkey  it seems that vkey is a sole function of the kbd (NUMLOCK ON)
char vKeyMap2[256]= {
    0,  27,  49,  50,  51,  52,  53,  54,  55,  56,  57,  48, 189, 187,   8,   9, //0
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 219, 221,  13,  17,   0,   0, //16
    0,   0,   0,   0,   0,   0,   0, 186, 192, 223,  16, 222,   0,   0,   0,   0, //32
    0,   0,   0, 188, 190, 191,  16, 106,  18,  32,  20, 112, 113, 114, 115, 116, //48
  117, 118, 119, 120, 121, 144, 145, 103, 104, 105, 109, 100, 101, 102, 107,  97, //64
   98,  99,  96, 110,   0,   0, 220, 122, 123,   0,   0,  91,  92,  93,   0,   0, //80
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// maps PM msg scan to win32 scan
char PMScanMapper[256]={
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
   20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
   30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
   40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
   50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
   60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
   70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
   80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
   28,  29,  53,   0,  56,  95,  71,  72,  73,  75,
   77,  79, 102,  81,  82,  83, 106, 107, 108, 109,
  110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
  120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
  130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
  150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
  180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
  190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
  200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
  210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
  220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
  230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
  250, 251, 252, 253, 254, 255
};

// finds extended keys amon pm scan codes
char PMExtKeys[256]={
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //10
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //20
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //30
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //40
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //50
    0,   0,   0,   0,   0,   0,   0,   0,   0,   1, //60
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //70
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //80
    1,   1,   1,   0,   1,   0,   1,   1,   1,   1, //90
    1,   1,   0,   1,   1,   1,   0,   0,   0,   0, //100
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0
};

// ALT-x combinations are not treated correctly: we would need to know scan->vkey translation !!
CHAR calcVkey(KBD32KEYINFO &key){
    int c = key.chr;
    if ((c >= 1) && (c <=26))  return c + 64;
    if ((c >=65) && (c <=90))  return c;
    if ((c >=97) && (c <=122)) return c - 32;
    int scan = key.scancode;
    CHAR vk =0;
    ULONG status = key.status;
    if (status & 0x20)  vk = vKeyMap2[scan];
    else                vk = vKeyMap1[scan];
    if ((status & 0x100) && (scan == 53)) vk = 111;
    return vk;
}

#define keyStatus kbd32ClientInstance.KeyStatus()


VOID updateKeyStatus(char scanCode,BOOL enhancedKey,BOOL keyUp){
    kbd32ClientInstance.LocalLock();

    // MODIFY status normal bits
    ULONG extScan = scanCode;
    if (enhancedKey){
        if (extScan == KBD32_ALTL_SCAN)  extScan = KBD32_ALTR_SCAN;
        if (extScan == KBD32_CTRLL_SCAN) extScan = KBD32_CTRLR_SCAN;
    }
    if (keyUp)  KBD32_UNSETKEY(keyStatus,extScan);
    else        KBD32_SETKEY(keyStatus,extScan);

    // MODIFY status toggling bit CAPS/LOCK/NUM/INS
    if (keyUp)  {
        ULONG specialScan = 0;
        switch (extScan){
            case 58: specialScan = KBD32_CAPSLOCK_SCAN;     break;
            case 69: specialScan = KBD32_NUMLOCK_SCAN;      break;
            case 70: specialScan = KBD32_SCROLLLOCK_SCAN;   break;
            case 82: specialScan = KBD32_INSERT_SCAN;       break;
        }
        if (specialScan!=0) KBD32_TOGGLEKEY(keyStatus,specialScan);
    }
    kbd32ClientInstance.LocalUnlock();
}

// without the knowledge of up/down but with the shift status info from kbd VIO call
VOID updateKeyStatusNoKeys(ULONG shiftStatus){
    kbd32ClientInstance.LocalLock();
    if (shiftStatus & 0x0040) KBD32_SETKEY(keyStatus,KBD32_CAPSLOCK_SCAN);   else KBD32_UNSETKEY(keyStatus,KBD32_CAPSLOCK_SCAN);
    if (shiftStatus & 0x0020) KBD32_SETKEY(keyStatus,KBD32_NUMLOCK_SCAN);    else KBD32_UNSETKEY(keyStatus,KBD32_NUMLOCK_SCAN);
    if (shiftStatus & 0x0010) KBD32_SETKEY(keyStatus,KBD32_SCROLLLOCK_SCAN); else KBD32_UNSETKEY(keyStatus,KBD32_SCROLLLOCK_SCAN);
    if (shiftStatus & 0x0002) KBD32_SETKEY(keyStatus,KBD32_SHIFTL_SCAN);     else KBD32_UNSETKEY(keyStatus,KBD32_SHIFTL_SCAN);
    if (shiftStatus & 0x0001) KBD32_SETKEY(keyStatus,KBD32_SHIFTR_SCAN);     else KBD32_UNSETKEY(keyStatus,KBD32_SHIFTR_SCAN);
    if (shiftStatus & 0x0200) KBD32_SETKEY(keyStatus,KBD32_ALTL_SCAN);       else KBD32_UNSETKEY(keyStatus,KBD32_ALTL_SCAN);
    if (shiftStatus & 0x0800) KBD32_SETKEY(keyStatus,KBD32_ALTR_SCAN);       else KBD32_UNSETKEY(keyStatus,KBD32_ALTR_SCAN);
    if (shiftStatus & 0x0100) KBD32_SETKEY(keyStatus,KBD32_CTRLL_SCAN);      else KBD32_UNSETKEY(keyStatus,KBD32_CTRLL_SCAN);
    if (shiftStatus & 0x0400) KBD32_SETKEY(keyStatus,KBD32_CTRLR_SCAN);      else KBD32_UNSETKEY(keyStatus,KBD32_CTRLR_SCAN);
    kbd32ClientInstance.LocalUnlock();
}

// use the knowledge of the status to set special keys but also update geberic key
VOID updateKeyStatus2(char scanCode,ULONG shiftStatus,BOOL keyUp){
    kbd32ClientInstance.LocalLock();
    // MODIFY status normal bits
    if (keyUp)  KBD32_UNSETKEY(keyStatus,scanCode);
    else        KBD32_SETKEY(keyStatus,scanCode);

    if (shiftStatus & 0x0040) KBD32_SETKEY(keyStatus,KBD32_CAPSLOCK_SCAN);   else KBD32_UNSETKEY(keyStatus,KBD32_CAPSLOCK_SCAN);
    if (shiftStatus & 0x0020) KBD32_SETKEY(keyStatus,KBD32_NUMLOCK_SCAN);    else KBD32_UNSETKEY(keyStatus,KBD32_NUMLOCK_SCAN);
    if (shiftStatus & 0x0010) KBD32_SETKEY(keyStatus,KBD32_SCROLLLOCK_SCAN); else KBD32_UNSETKEY(keyStatus,KBD32_SCROLLLOCK_SCAN);
    if (shiftStatus & 0x0002) KBD32_SETKEY(keyStatus,KBD32_SHIFTL_SCAN);     else KBD32_UNSETKEY(keyStatus,KBD32_SHIFTL_SCAN);
    if (shiftStatus & 0x0001) KBD32_SETKEY(keyStatus,KBD32_SHIFTR_SCAN);     else KBD32_UNSETKEY(keyStatus,KBD32_SHIFTR_SCAN);
    if (shiftStatus & 0x0200) KBD32_SETKEY(keyStatus,KBD32_ALTL_SCAN);       else KBD32_UNSETKEY(keyStatus,KBD32_ALTL_SCAN);
    if (shiftStatus & 0x0800) KBD32_SETKEY(keyStatus,KBD32_ALTR_SCAN);       else KBD32_UNSETKEY(keyStatus,KBD32_ALTR_SCAN);
    if (shiftStatus & 0x0100) KBD32_SETKEY(keyStatus,KBD32_CTRLL_SCAN);      else KBD32_UNSETKEY(keyStatus,KBD32_CTRLL_SCAN);
    if (shiftStatus & 0x0400) KBD32_SETKEY(keyStatus,KBD32_CTRLR_SCAN);      else KBD32_UNSETKEY(keyStatus,KBD32_CTRLR_SCAN);
    kbd32ClientInstance.LocalUnlock();
}

//
BOOL kbdMapperF(ULONG *eventType,VIO32EVENTINFO *event,VOID *filterData){
    VIO32EVENTINFO ev = *event;
    switch (*eventType){
        case WM_CHAR: {
//CHRMSG *data = &ev.wmchar;
//printf(">>fs=0x%x repeat=%i scan=%i char=%i,%i vkey=%i\n",data->fs,data->cRepeat,data->scancode,data->chr,data->chr & 0xFF,data->vkey );
            // STANDARD COPY
            event->vio32Kbd.chr      = ev.wmchar.chr & 0xFF;
            event->vio32Kbd.scancode = PMScanMapper[ev.wmchar.scancode];
            event->vio32Kbd.repeat   = ev.wmchar.cRepeat;
            event->vio32Kbd.keyUp    = ((ev.wmchar.fs & KC_KEYUP) != 0);

            // CHANGE event type
            *eventType = (ULONG)KBD32_CHAR;

            // work out extended keys
            BOOL enhancedKey = (PMExtKeys[ev.wmchar.scancode] == 1);

            // update the key status bits
            updateKeyStatus(event->vio32Kbd.scancode,enhancedKey,event->vio32Kbd.keyUp);

            // CALCULATE SHIFT status from global key-status
            event->vio32Kbd.status   = 0;
            if (KBD32_CHECKKEY(keyStatus,KBD32_CAPSLOCK_SCAN))      event->vio32Kbd.status |= KBD32_CAPSLOCK;
            if (KBD32_CHECKKEY(keyStatus,KBD32_NUMLOCK_SCAN))       event->vio32Kbd.status |= KBD32_NUMLOCK;
            if (KBD32_CHECKKEY(keyStatus,KBD32_SCROLLLOCK_SCAN))    event->vio32Kbd.status |= KBD32_SCROLLLOCK;
            if (KBD32_CHECKKEY(keyStatus,KBD32_SHIFTL_SCAN))        event->vio32Kbd.status |= KBD32_SHIFT;
            if (KBD32_CHECKKEY(keyStatus,KBD32_SHIFTR_SCAN))        event->vio32Kbd.status |= KBD32_SHIFT;
            if (KBD32_CHECKKEY(keyStatus,KBD32_ALTL_SCAN))          event->vio32Kbd.status |= KBD32_ALT_L;
            if (KBD32_CHECKKEY(keyStatus,KBD32_ALTR_SCAN))          event->vio32Kbd.status |= KBD32_ALT_R;
            if (KBD32_CHECKKEY(keyStatus,KBD32_CTRLL_SCAN))         event->vio32Kbd.status |= KBD32_CTRL_L;
            if (KBD32_CHECKKEY(keyStatus,KBD32_CTRLR_SCAN))         event->vio32Kbd.status |= KBD32_CTRL_R;
            if (enhancedKey ){
                event->vio32Kbd.chr = 0;
                event->vio32Kbd.status |= KBD32_ENHANCED_KEY;
            }

            // compute vKey
            event->vio32Kbd.vKey     = calcVkey(event->vio32Kbd);

            // on special intermediate keys. Discard them
            if (event->vio32Kbd.scancode == 0)  return TRUE;

        } break;
        case VIO_CHAR:{
//KBDKEYINFO *k = &ev.vioKbd;
//printf(">>chr=%i scan=%i fbst=0x%x nlss=0x%x fsst=0x%x %i \n",k->chChar,k->chScan,k->fbStatus,k->bNlsShift,k->fsState,k->time);

            // STANDARD COPY
            event->vio32Kbd.chr      = ev.vioKbd.chChar & 0xFF;
            event->vio32Kbd.scancode = ev.vioKbd.chScan;
            event->vio32Kbd.repeat   = 1;
            event->vio32Kbd.keyUp    = FALSE;
            event->vio32Kbd.status   = 0;

            // CHANGE event type
            *eventType = (ULONG)KBD32_CHAR;

            // PATCH-UP SOME SCAN CODE
            if (event->vio32Kbd.scancode == 133) event->vio32Kbd.scancode = 87;
            if (event->vio32Kbd.scancode == 134) event->vio32Kbd.scancode = 88;
            if (event->vio32Kbd.scancode == 224) event->vio32Kbd.scancode = 28;

            // work out extended keys
            BOOL enhancedKey = (event->vio32Kbd.chr == 224);

            // update the key status bits
            updateKeyStatusNoKeys(ev.vioKbd.fsState);

            // SHIFT STATUS TRANSLATION
            if (ev.vioKbd.fsState & 0x0040) event->vio32Kbd.status |= KBD32_CAPSLOCK;
            if (ev.vioKbd.fsState & 0x0020) event->vio32Kbd.status |= KBD32_NUMLOCK;
            if (ev.vioKbd.fsState & 0x0010) event->vio32Kbd.status |= KBD32_SCROLLLOCK;
            if (ev.vioKbd.fsState & 0x0002) event->vio32Kbd.status |= KBD32_SHIFT;
            if (ev.vioKbd.fsState & 0x0001) event->vio32Kbd.status |= KBD32_SHIFT;
            if (ev.vioKbd.fsState & 0x0100) event->vio32Kbd.status |= KBD32_CTRL_L;
            if (ev.vioKbd.fsState & 0x0400) event->vio32Kbd.status |= KBD32_CTRL_R;
            if (ev.vioKbd.fsState & 0x0200) event->vio32Kbd.status |= KBD32_ALT_L;
            if (ev.vioKbd.fsState & 0x0800) event->vio32Kbd.status |= KBD32_ALT_R;
            if (enhancedKey ){
                event->vio32Kbd.chr = 0;
                event->vio32Kbd.status |= KBD32_ENHANCED_KEY;
            }

            // compute vKey
            event->vio32Kbd.vKey     = calcVkey(event->vio32Kbd);

        }break;
        case MON_CHAR:{

//MonRecord *m = &ev.monKbd;
//printf(">>flag=0x%x chr=%i scan=%i dbst=0x%x dbs=0x%x ss=0x%x %i 0x%x\n",m->MonFlagWord,m->XlatedChar,m->XlatedScan,m->DBCS_Status,m->DBCS_Shift,m->Shift_State,m->Milliseconds,m->KbdDDFlagWord);

            // STANDARD COPY
            event->vio32Kbd.chr      = ev.monKbd.XlatedChar;
            event->vio32Kbd.scancode = ev.monKbd.XlatedScan;
            event->vio32Kbd.repeat   = 1;
            event->vio32Kbd.keyUp    = ((ev.monKbd.KbdDDFlagWord & 0x40) != 0);
            event->vio32Kbd.status   = 0;

            // CHANGE event type
            *eventType = (ULONG)KBD32_CHAR;

            // PATCH-UP SOME SCAN CODE
            if (event->vio32Kbd.scancode == 133) event->vio32Kbd.scancode = 87;
            if (event->vio32Kbd.scancode == 134) event->vio32Kbd.scancode = 88;
            if (event->vio32Kbd.scancode == 224) event->vio32Kbd.scancode = 28;

            // work out extended keys
            BOOL enhancedKey = ((ev.monKbd.KbdDDFlagWord & 0x80) != 0);

            // SHIFT STATUS TRANSLATION
            if (ev.monKbd.Shift_State & 0x0040) event->vio32Kbd.status |= KBD32_CAPSLOCK;
            if (ev.monKbd.Shift_State & 0x0020) event->vio32Kbd.status |= KBD32_NUMLOCK;
            if (ev.monKbd.Shift_State & 0x0010) event->vio32Kbd.status |= KBD32_SCROLLLOCK;
            if (ev.monKbd.Shift_State & 0x0002) event->vio32Kbd.status |= KBD32_SHIFT;
            if (ev.monKbd.Shift_State & 0x0001) event->vio32Kbd.status |= KBD32_SHIFT;
            if (ev.monKbd.Shift_State & 0x0100) event->vio32Kbd.status |= KBD32_CTRL_L;
            if (ev.monKbd.Shift_State & 0x0200) event->vio32Kbd.status |= KBD32_ALT_L;
            if (ev.monKbd.Shift_State & 0x0800) event->vio32Kbd.status |= KBD32_ALT_R;
            if (ev.monKbd.Shift_State & 0x0400) event->vio32Kbd.status |= KBD32_CTRL_R;
            if (enhancedKey ){
                event->vio32Kbd.chr = 0;
                event->vio32Kbd.status |= KBD32_ENHANCED_KEY;
            }

            // TREAT char 224 -> 0
            if (event->vio32Kbd.chr == 224)     event->vio32Kbd.chr = 0;

            // DETECT SHIFT STATUS CHANGES EVENTS
            if ((ev.monKbd.KbdDDFlagWord & 0x3F)==7){

                // CALCULATE SHIFT status from global key-status
                ULONG lastShiftStatus = 0;
                if (KBD32_CHECKKEY(keyStatus,58))                       lastShiftStatus |= 0x4000;
                if (KBD32_CHECKKEY(keyStatus,69))                       lastShiftStatus |= 0x2000;
                if (KBD32_CHECKKEY(keyStatus,70))                       lastShiftStatus |= 0x1000;
                if (KBD32_CHECKKEY(keyStatus,KBD32_SHIFTL_SCAN))        lastShiftStatus |= 0x0002;
                if (KBD32_CHECKKEY(keyStatus,KBD32_SHIFTR_SCAN))        lastShiftStatus |= 0x0001;
                if (KBD32_CHECKKEY(keyStatus,KBD32_ALTL_SCAN))          lastShiftStatus |= 0x0200;
                if (KBD32_CHECKKEY(keyStatus,KBD32_ALTR_SCAN))          lastShiftStatus |= 0x0800;
                if (KBD32_CHECKKEY(keyStatus,KBD32_CTRLL_SCAN))         lastShiftStatus |= 0x0100;
                if (KBD32_CHECKKEY(keyStatus,KBD32_CTRLR_SCAN))         lastShiftStatus |= 0x0400;

//printf("old:%08x - %08x:new\n", lastShiftStatus,ev.monKbd.Shift_State);

                ULONG changes = lastShiftStatus ^ ev.monKbd.Shift_State;
                if (changes & 0x1)      event->vio32Kbd.scancode = 54;
                else
                if (changes & 0x2)      event->vio32Kbd.scancode = 42;
                else
                if (changes & 0x4000)   event->vio32Kbd.scancode = 58;
                else
                if (changes & 0x100)    event->vio32Kbd.scancode = 29;
                else
                if (changes & 0x200)    event->vio32Kbd.scancode = 56;
                else
                if (changes & 0x800)    event->vio32Kbd.scancode = 56;
                else
                if (changes & 0x400)    event->vio32Kbd.scancode = 29;
                else
                if (changes & 0x1000)   event->vio32Kbd.scancode = 70;
                else
                if (changes & 0x2000)   event->vio32Kbd.scancode = 69;
            }

            // update the key status bits
            updateKeyStatus2(event->vio32Kbd.scancode,ev.monKbd.Shift_State,event->vio32Kbd.keyUp);

            // compute vKey
            event->vio32Kbd.vKey     = calcVkey(event->vio32Kbd);

            // on special intermediate keys. Discard them
            if (event->vio32Kbd.scancode == 0){
                return TRUE;
            }
        }break;
        case VIO_MOU:{
            static ULONG lastClickTime = 0;
            static ULONG lastClickBits = 0;

            // STANDARD COPY
            event->vio32Mou.x               = ev.vioMou.row;
            event->vio32Mou.y               = ev.vioMou.col;
            event->vio32Mou.buttonStatus    = 0;
            event->vio32Mou.status          = 0;
            event->vio32Mou.eventType       = 0;

            // CHANGE event type
            *eventType = (ULONG)VIO32_MOU;

            // check for movement in the bit set
            BOOL hasMoved    = FALSE;
            if (ev.vioMou.fs & 0x2B) hasMoved = TRUE;

            // check the new status
            if (ev.vioMou.fs & 0x06) event->vio32Mou.buttonStatus |= VIO32_MB1;
            if (ev.vioMou.fs & 0x18) event->vio32Mou.buttonStatus |= VIO32_MB2;
            if (ev.vioMou.fs & 0x60) event->vio32Mou.buttonStatus |= VIO32_MB3;

            // work out the old status
            ULONG lastButtonStatus = 0;
            if (KBD32_CHECKKEY(keyStatus,VIO32_MB1_SCAN))   lastButtonStatus |= VIO32_MB1;
            if (KBD32_CHECKKEY(keyStatus,VIO32_MB2_SCAN))   lastButtonStatus |= VIO32_MB2;
            if (KBD32_CHECKKEY(keyStatus,VIO32_MB3_SCAN))   lastButtonStatus |= VIO32_MB3;

            // work out the clicking bits : only going up
            ULONG pressed;
            pressed = ~lastButtonStatus & event->vio32Mou.buttonStatus;

            // a new pressed bit? the same as before? not too far in time?
            BOOL doubleClick = FALSE;
            if (pressed != 0){
                if (pressed == lastClickBits){
                    if ((ev.vioMou.time - lastClickTime) < 500) {
                        doubleClick = TRUE;
                        pressed = 0;
                    }
                }
                lastClickBits = pressed;
                lastClickTime = ev.vioMou.time;
            }

            // work out right event
            if (doubleClick) event->vio32Mou.eventType = VIO32_DOUBLECLICK;
            else
            if (hasMoved)    event->vio32Mou.eventType = VIO32_MOUSEMOVE;

            // fill in key status
            if (KBD32_CHECKKEY(keyStatus,KBD32_CAPSLOCK_SCAN))      event->vio32Mou.status |= KBD32_CAPSLOCK;
            if (KBD32_CHECKKEY(keyStatus,KBD32_NUMLOCK_SCAN))       event->vio32Mou.status |= KBD32_NUMLOCK;
            if (KBD32_CHECKKEY(keyStatus,KBD32_SCROLLLOCK_SCAN))    event->vio32Mou.status |= KBD32_SCROLLLOCK;
            if (KBD32_CHECKKEY(keyStatus,KBD32_SHIFTL_SCAN))        event->vio32Mou.status |= KBD32_SHIFT;
            if (KBD32_CHECKKEY(keyStatus,KBD32_SHIFTR_SCAN))        event->vio32Mou.status |= KBD32_SHIFT;
            if (KBD32_CHECKKEY(keyStatus,KBD32_ALTL_SCAN))          event->vio32Mou.status |= KBD32_ALT_L;
            if (KBD32_CHECKKEY(keyStatus,KBD32_ALTR_SCAN))          event->vio32Mou.status |= KBD32_ALT_R;
            if (KBD32_CHECKKEY(keyStatus,KBD32_CTRLL_SCAN))         event->vio32Mou.status |= KBD32_CTRL_L;
            if (KBD32_CHECKKEY(keyStatus,KBD32_CTRLR_SCAN))         event->vio32Mou.status |= KBD32_CTRL_R;

            // save new mouse status
            if (ev.vioMou.fs & 0x06) KBD32_SETKEY(keyStatus,VIO32_MB1_SCAN); else KBD32_UNSETKEY(keyStatus,VIO32_MB1_SCAN);
            if (ev.vioMou.fs & 0x18) KBD32_SETKEY(keyStatus,VIO32_MB2_SCAN); else KBD32_UNSETKEY(keyStatus,VIO32_MB2_SCAN);
            if (ev.vioMou.fs & 0x60) KBD32_SETKEY(keyStatus,VIO32_MB3_SCAN); else KBD32_UNSETKEY(keyStatus,VIO32_MB3_SCAN);

//printf("x=%i y=%i bs=%x ks=%x ev=%x\n",event->vio32Mou.x,event->vio32Mou.y,event->vio32Mou.buttonStatus,event->vio32Mou.status,event->vio32Mou.eventType);

        } break;
        case WM_FOCUSCHANGE:{

            HFILE   DevHandle  = NULLHANDLE;   /* Handle for device */
            ULONG   action;
            APIRET  rc = DosOpen("KBD$",&DevHandle, &action, 0, 0, FILE_OPEN, OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL);

            SHIFTSTATE shiftState;
            if (rc == 0){
                ULONG parmLen = 0;
                ULONG dataLen = 0;
                rc = DosDevIOCtl(DevHandle,IOCTL_KEYBOARD,KBD_GETSHIFTSTATE,0,0,&parmLen,&shiftState,sizeof(SHIFTSTATE),&dataLen);
            }

            // update the key status bits
            updateKeyStatusNoKeys(shiftState.fsState);

            DosClose(DevHandle);

        } break;
        default:
        break;
    }
    return FALSE;
}

vio32Filter kbdMapper={NULL,(VOID *)KBD32_REMAPPER_NAME,kbdMapperF};

