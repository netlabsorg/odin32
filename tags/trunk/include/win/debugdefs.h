/* $Id: debugdefs.h,v 1.2 1999-08-22 22:47:17 sandervl Exp $ */
/* Do not modify this file -- it is automatically generated! */

#include "debugtools.h"

#define DEBUG_CLASS_COUNT __DBCL_COUNT

#ifdef DEBUG_RUNTIME

extern const char * const debug_cl_name[] = { "fixme", "err", "warn", "trace" };

extern const int dbch_accel = 0;
extern const int dbch_advapi = 1;
extern const int dbch_animate = 2;
extern const int dbch_aspi = 3;
extern const int dbch_atom = 4;
extern const int dbch_avifile = 5;
extern const int dbch_bitblt = 6;
extern const int dbch_bitmap = 7;
extern const int dbch_caret = 8;
extern const int dbch_cdaudio = 9;
extern const int dbch_class = 10;
extern const int dbch_clipboard = 11;
extern const int dbch_clipping = 12;
extern const int dbch_combo = 13;
extern const int dbch_comboex = 14;
extern const int dbch_comm = 15;
extern const int dbch_commctrl = 16;
extern const int dbch_commdlg = 17;
extern const int dbch_console = 18;
extern const int dbch_crtdll = 19;
extern const int dbch_cursor = 20;
extern const int dbch_datetime = 21;
extern const int dbch_dc = 22;
extern const int dbch_ddeml = 23;
extern const int dbch_ddraw = 24;
extern const int dbch_debug = 25;
extern const int dbch_delayhlp = 26;
extern const int dbch_dialog = 27;
extern const int dbch_dinput = 28;
extern const int dbch_dll = 29;
extern const int dbch_dosfs = 30;
extern const int dbch_dosmem = 31;
extern const int dbch_dplay = 32;
extern const int dbch_driver = 33;
extern const int dbch_dsound = 34;
extern const int dbch_edit = 35;
extern const int dbch_elfdll = 36;
extern const int dbch_enhmetafile = 37;
extern const int dbch_event = 38;
extern const int dbch_exec = 39;
extern const int dbch_file = 40;
extern const int dbch_fixup = 41;
extern const int dbch_font = 42;
extern const int dbch_gdi = 43;
extern const int dbch_global = 44;
extern const int dbch_graphics = 45;
extern const int dbch_header = 46;
extern const int dbch_heap = 47;
extern const int dbch_hook = 48;
extern const int dbch_hotkey = 49;
extern const int dbch_icon = 50;
extern const int dbch_imagehlp = 51;
extern const int dbch_imagelist = 52;
extern const int dbch_imm = 53;
extern const int dbch_int = 54;
extern const int dbch_int10 = 55;
extern const int dbch_int16 = 56;
extern const int dbch_int17 = 57;
extern const int dbch_int19 = 58;
extern const int dbch_int21 = 59;
extern const int dbch_int31 = 60;
extern const int dbch_io = 61;
extern const int dbch_ipaddress = 62;
extern const int dbch_key = 63;
extern const int dbch_keyboard = 64;
extern const int dbch_ldt = 65;
extern const int dbch_listbox = 66;
extern const int dbch_listview = 67;
extern const int dbch_local = 68;
extern const int dbch_mci = 69;
extern const int dbch_mcianim = 70;
extern const int dbch_mciavi = 71;
extern const int dbch_mcimidi = 72;
extern const int dbch_mciwave = 73;
extern const int dbch_mdi = 74;
extern const int dbch_menu = 75;
extern const int dbch_message = 76;
extern const int dbch_metafile = 77;
extern const int dbch_midi = 78;
extern const int dbch_mmaux = 79;
extern const int dbch_mmio = 80;
extern const int dbch_mmsys = 81;
extern const int dbch_mmtime = 82;
extern const int dbch_module = 83;
extern const int dbch_monthcal = 84;
extern const int dbch_mpr = 85;
extern const int dbch_msacm = 86;
extern const int dbch_msg = 87;
extern const int dbch_msvideo = 88;
extern const int dbch_nativefont = 89;
extern const int dbch_nonclient = 90;
extern const int dbch_ntdll = 91;
extern const int dbch_ole = 92;
extern const int dbch_pager = 93;
extern const int dbch_palette = 94;
extern const int dbch_pidl = 95;
extern const int dbch_print = 96;
extern const int dbch_process = 97;
extern const int dbch_profile = 98;
extern const int dbch_progress = 99;
extern const int dbch_prop = 100;
extern const int dbch_propsheet = 101;
extern const int dbch_psapi = 102;
extern const int dbch_psdrv = 103;
extern const int dbch_ras = 104;
extern const int dbch_rebar = 105;
extern const int dbch_reg = 106;
extern const int dbch_region = 107;
extern const int dbch_relay = 108;
extern const int dbch_resource = 109;
extern const int dbch_scroll = 110;
extern const int dbch_security = 111;
extern const int dbch_segment = 112;
extern const int dbch_seh = 113;
extern const int dbch_selector = 114;
extern const int dbch_sendmsg = 115;
extern const int dbch_server = 116;
extern const int dbch_shell = 117;
extern const int dbch_snoop = 118;
extern const int dbch_sound = 119;
extern const int dbch_static = 120;
extern const int dbch_statusbar = 121;
extern const int dbch_storage = 122;
extern const int dbch_stress = 123;
extern const int dbch_string = 124;
extern const int dbch_syscolor = 125;
extern const int dbch_system = 126;
extern const int dbch_tab = 127;
extern const int dbch_tapi = 128;
extern const int dbch_task = 129;
extern const int dbch_text = 130;
extern const int dbch_thread = 131;
extern const int dbch_thunk = 132;
extern const int dbch_timer = 133;
extern const int dbch_toolbar = 134;
extern const int dbch_toolhelp = 135;
extern const int dbch_tooltips = 136;
extern const int dbch_trackbar = 137;
extern const int dbch_treeview = 138;
extern const int dbch_ttydrv = 139;
extern const int dbch_tweak = 140;
extern const int dbch_typelib = 141;
extern const int dbch_updown = 142;
extern const int dbch_ver = 143;
extern const int dbch_virtual = 144;
extern const int dbch_vxd = 145;
extern const int dbch_wave = 146;
extern const int dbch_win = 147;
extern const int dbch_win16drv = 148;
extern const int dbch_win32 = 149;
extern const int dbch_wing = 150;
extern const int dbch_winsock = 151;
extern const int dbch_winspool = 152;
extern const int dbch_wnet = 153;
extern const int dbch_x11 = 154;
extern const int dbch_x11drv = 155;

#define DEBUG_CHANNEL_COUNT 156

char __debug_msg_enabled[DEBUG_CHANNEL_COUNT][DEBUG_CLASS_COUNT] = {
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0},
{1, 1, 0, 0}
};

extern const char * const debug_ch_name[DEBUG_CHANNEL_COUNT] = {
"accel",
"advapi",
"animate",
"aspi",
"atom",
"avifile",
"bitblt",
"bitmap",
"caret",
"cdaudio",
"class",
"clipboard",
"clipping",
"combo",
"comboex",
"comm",
"commctrl",
"commdlg",
"console",
"crtdll",
"cursor",
"datetime",
"dc",
"ddeml",
"ddraw",
"debug",
"delayhlp",
"dialog",
"dinput",
"dll",
"dosfs",
"dosmem",
"dplay",
"driver",
"dsound",
"edit",
"elfdll",
"enhmetafile",
"event",
"exec",
"file",
"fixup",
"font",
"gdi",
"global",
"graphics",
"header",
"heap",
"hook",
"hotkey",
"icon",
"imagehlp",
"imagelist",
"imm",
"int",
"int10",
"int16",
"int17",
"int19",
"int21",
"int31",
"io",
"ipaddress",
"key",
"keyboard",
"ldt",
"listbox",
"listview",
"local",
"mci",
"mcianim",
"mciavi",
"mcimidi",
"mciwave",
"mdi",
"menu",
"message",
"metafile",
"midi",
"mmaux",
"mmio",
"mmsys",
"mmtime",
"module",
"monthcal",
"mpr",
"msacm",
"msg",
"msvideo",
"nativefont",
"nonclient",
"ntdll",
"ole",
"pager",
"palette",
"pidl",
"print",
"process",
"profile",
"progress",
"prop",
"propsheet",
"psapi",
"psdrv",
"ras",
"rebar",
"reg",
"region",
"relay",
"resource",
"scroll",
"security",
"segment",
"seh",
"selector",
"sendmsg",
"server",
"shell",
"snoop",
"sound",
"static",
"statusbar",
"storage",
"stress",
"string",
"syscolor",
"system",
"tab",
"tapi",
"task",
"text",
"thread",
"thunk",
"timer",
"toolbar",
"toolhelp",
"tooltips",
"trackbar",
"treeview",
"ttydrv",
"tweak",
"typelib",
"updown",
"ver",
"virtual",
"vxd",
"wave",
"win",
"win16drv",
"win32",
"wing",
"winsock",
"winspool",
"wnet",
"x11",
"x11drv"
};

#endif /*DEBUG_RUNTIME*/

/* end of automatically generated debug.h */
