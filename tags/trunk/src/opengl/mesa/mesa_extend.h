/* $Id: mesa_extend.h,v 1.1 2000-02-29 00:48:33 sandervl Exp $ */
/* mesa_extend.h
 * for wmesa-2.3
 *  Written by Li Wei (liwei@aiar.xjtu.edu.cn)
 */

/* Log 6/14, 1997
 * revision 1.01
 * struct DisplayOptions defined for tk_ddmesa.c to read the initial file
 */

#include "gl.h"
#include <stdlib.h>
#ifdef __WIN32OS2__
#include <os2win.h>
#else
#include <windows.h>
#include <winbase.h>
#endif

typedef enum SHIFT{ SHIFT_NONE, SHIFT_LEFT,SHIFT_RIGHT,SHIFT_UP,SHIFT_DOWN,SHIFT_FAR,SHIFT_NEAR};

extern GLfloat deltaView ;

extern GLuint viewShift;

extern GLenum glImageRendered();

extern GLenum imageRendered ;

extern GLfloat deltaView ;

extern GLfloat deltaShift;

void shiftView( void );

struct DISPLAY_OPTIONS {
        int  stereo;
        int  fullScreen;
        int      mode;
        int      bpp;
};

extern struct DISPLAY_OPTIONS displayOptions;
extern void getDisplayOptions( void);

GLenum defaultKeyProc(int, GLenum);
extern GLenum (*userKeyProc) (int, GLenum);
