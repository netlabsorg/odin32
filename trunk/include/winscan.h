#ifndef __WINSCAN_H__
#define __WINSCAN_H__

#define WIN_KEY_EXTENDED   0x01000000
#define WIN_KEY_DONTCARE   0x02000000
#define WIN_KEY_ALTHELD    0x20000000
#define WIN_KEY_PREVSTATE  0x40000000

#define WINSCAN_ESC             0x01  
#define WINSCAN_ONE             0x02  
#define WINSCAN_TWO             0x03  
#define WINSCAN_THREE           0x04  
#define WINSCAN_FOUR            0x05  
#define WINSCAN_FIVE            0x06  
#define WINSCAN_SIX             0x07  
#define WINSCAN_SEVEN           0x08  
#define WINSCAN_EIGHT           0x09  
#define WINSCAN_NINE            0x0a  
#define WINSCAN_ZERO            0x0b  
#define WINSCAN_HYPHEN          0x0c  
#define WINSCAN_EQUAL           0x0d  
#define WINSCAN_BACKSPACE       0x0e  
#define WINSCAN_TAB             0x0f  
#define WINSCAN_Q               0x10  
#define WINSCAN_W               0x11  
#define WINSCAN_E               0x12  
#define WINSCAN_R               0x13  
#define WINSCAN_T               0x14  
#define WINSCAN_Y               0x15  
#define WINSCAN_U               0x16  
#define WINSCAN_I               0x17  
#define WINSCAN_O               0x18  
#define WINSCAN_P               0x19  
#define WINSCAN_BRACKETLEFT     0x1a  
#define WINSCAN_BRACKETRIGHT    0x1b  
#define WINSCAN_ENTER           0x1c  
#define WINSCAN_PADENTER        0x1c
#define WINSCAN_CTRLLEFT        0x1d
#define WINSCAN_CTRLRIGHT       0x1d
#define WINSCAN_A               0x1e  
#define WINSCAN_S               0x1f  
#define WINSCAN_D               0x20  
#define WINSCAN_F               0x21  
#define WINSCAN_G               0x22  
#define WINSCAN_H               0x23  
#define WINSCAN_J               0x24  
#define WINSCAN_K               0x25  
#define WINSCAN_L               0x26  
#define WINSCAN_SEMICOLON       0x27  
#define WINSCAN_QUOTESINGLE     0x28  
#define WINSCAN_GRAVE           0x29  
#define WINSCAN_SHIFTLEFT       0x2a  
#define WINSCAN_BACKSLASH       0x2b  
#define WINSCAN_Z               0x2c  
#define WINSCAN_X               0x2d  
#define WINSCAN_C               0x2e  
#define WINSCAN_V               0x2f  
#define WINSCAN_B               0x30  
#define WINSCAN_N               0x31  
#define WINSCAN_M               0x32  
#define WINSCAN_COMMA           0x33  
#define WINSCAN_PERIOD          0x34  
#define WINSCAN_SLASH           0x35
#define WINSCAN_PADSLASH        0x35
#define WINSCAN_SHIFTRIGHT      0x36  
#define WINSCAN_PADASTERISK     0x37  
#define WINSCAN_PRINT           0x37
#define WINSCAN_ALTLEFT         0x38  
#define WINSCAN_ALTRIGHT        0x38
#define WINSCAN_SPACE           0x39  
#define WINSCAN_CAPSLOCK        0x3a  
#define WINSCAN_F1              0x3b  
#define WINSCAN_F2              0x3c  
#define WINSCAN_F3              0x3d  
#define WINSCAN_F4              0x3e  
#define WINSCAN_F5              0x3f  
#define WINSCAN_F6              0x40  
#define WINSCAN_F7              0x41  
#define WINSCAN_F8              0x42  
#define WINSCAN_F9              0x43  
#define WINSCAN_F10             0x44  
#define WINSCAN_NUMLOCK         0x45  
#define WINSCAN_PAUSE           0x45
#define WINSCAN_SCROLLLOCK      0x46  
#define WINSCAN_PAD7            0x47  
#define WINSCAN_HOME            0x47
#define WINSCAN_UP              0x48
#define WINSCAN_PAD8            0x48  
#define WINSCAN_PAD9            0x49  
#define WINSCAN_PAGEUP          0x49
#define WINSCAN_PADMINUS        0x4a  
#define WINSCAN_LEFT            0x4b
#define WINSCAN_PAD4            0x4b  
#define WINSCAN_PAD5            0x4c  
#define WINSCAN_RIGHT           0x4d
#define WINSCAN_PAD6            0x4d  
#define WINSCAN_PADPLUS         0x4e  
#define WINSCAN_END             0x4f
#define WINSCAN_PAD1            0x4f  
#define WINSCAN_DOWN            0x50
#define WINSCAN_PAD2            0x50  
#define WINSCAN_PAGEDOWN        0x51
#define WINSCAN_PAD3            0x51  
#define WINSCAN_INSERT          0x52
#define WINSCAN_PAD0            0x52  
#define WINSCAN_DELETE          0x53
#define WINSCAN_PADPERIOD       0x53  
#define WINSCAN_SYSREQ          0x54  
#define WINSCAN_RESET           0x55  
#define WINSCAN_EXTRA           0x56  // OEM102 < > | on UK/Germany keyboards
#define WINSCAN_F11             0x57  
#define WINSCAN_F12             0x58  
#define WINSCAN_BACKTAB         0x59  
#define WINSCAN_WINLEFT         0x5b
#define WINSCAN_WINRIGHT        0x5c
#define WINSCAN_APPLICATION     0x5d
#define WINSCAN_NECF13          0x64  // (NEC PC98)
#define WINSCAN_NECF14          0x65  // (NEC PC98)
#define WINSCAN_NECF15          0x66  // (NEC PC98)
#define WINSCAN_F23             0x6a  
#define WINSCAN_F24             0x6b  
#define WINSCAN_SYSMEM          0x6c  
#define WINSCAN_ERASEEOF        0x6d  
#define WINSCAN_BREAK           0x6e  
#define WINSCAN_MOVEWIN         0x6f  
#define WINSCAN_NLS3            0x70  // KANA (Japanese keyboard)
#define WINSCAN_HELP            0x71  
#define WINSCAN_TASKMAN         0x72  
#define WINSCAN_JEXTRA          0x73  
#define WINSCAN_B11             0x73  // / ? on Portugese (Brazilian) keyboards 
#define WINSCAN_JUMP            0x74  
#define WINSCAN_MINWIN          0x75  
#define WINSCAN_CLEAR           0x76  
#define WINSCAN_77              0x77  
#define WINSCAN_78              0x78  
#define WINSCAN_NLS2            0x79  // CONVERT (Japanese keyboard)
#define WINSCAN_SIZE            0x7a  
#define WINSCAN_NLS1            0x7b  // NOCONVERT (Japanese keyboard)
#define WINSCAN_YEN             0x7d  
#define WINSCAN_E13             0x7d
#define WINSCAN_ABNT_C2         0x7e  // Numpad . on Portugese (Brazilian) keyboards
#define WINSCAN_PA1             0x80  
#define WINSCAN_F13             0x81  
#define WINSCAN_F14             0x82  
#define WINSCAN_F15             0x83  
#define WINSCAN_PA2             0x84  
#define WINSCAN_PA3             0x85  
#define WINSCAN_SPACEBREAK      0x86  
#define WINSCAN_TABRIGHT        0x87  
#define WINSCAN_NOOP            0x88  
#define WINSCAN_F16             0x89  
#define WINSCAN_F17             0x8a  
#define WINSCAN_F18             0x8b  
#define WINSCAN_F19             0x8c  
#define WINSCAN_F20             0x8d  
#define WINSCAN_NUMPADEQUALS    0x8d    // = on numeric keypad (NEC PC98)
#define WINSCAN_F21             0x8e  
#define WINSCAN_F22             0x8f  

#define WINSCAN_PREVTRACK       0x90    // Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) 
#define WINSCAN_AT              0x91    //                     (NEC PC98)
#define WINSCAN_COLON           0x92    //                     (NEC PC98)
#define WINSCAN_UNDERLINE       0x93    //                     (NEC PC98)
#define WINSCAN_KANJI           0x94    // (Japanese keyboard)           
#define WINSCAN_STOP            0x95    //                     (NEC PC98)
#define WINSCAN_AX              0x96    //                     (Japan AX)
#define WINSCAN_UNLABELED       0x97    //                        (J3100)

#define WINSCAN_NEXTTRACK       0x99    // Next Track 


#define WINSCAN_MUTE            0xA0    // Mute 
#define WINSCAN_CALCULATOR      0xA1    // Calculator 
#define WINSCAN_PLAYPAUSE       0xA2    // Play / Pause 
#define WINSCAN_MEDIASTOP       0xA4    // Media Stop 

#define WINSCAN_VOLUMEDOWN      0xAE    // Volume - 
#define WINSCAN_VOLUMEUP        0xB0    // Volume + 
#define WINSCAN_WEBHOME         0xB2    // Web home 
#define WINSCAN_NUMPADCOMMA     0xB3    // , on numeric keypad (NEC PC98) 

#define WINSCAN_POWER           0xDE    // System Power 
#define WINSCAN_SLEEP           0xDF    // System Sleep 
#define WINSCAN_WAKE            0xE3    // System Wake 
#define WINSCAN_WEBSEARCH       0xE5    // Web Search 
#define WINSCAN_WEBFAVORITES    0xE6    // Web Favorites 
#define WINSCAN_WEBREFRESH      0xE7    // Web Refresh 
#define WINSCAN_WEBSTOP         0xE8    // Web Stop 
#define WINSCAN_WEBFORWARD      0xE9    // Web Forward 
#define WINSCAN_WEBBACK         0xEA    // Web Back 
#define WINSCAN_MYCOMPUTER      0xEB    // My Computer 
#define WINSCAN_MAIL            0xEC    // Mail 
#define WINSCAN_MEDIASELECT     0xED    // Media Select 

#endif
