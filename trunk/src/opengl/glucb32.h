/* $Id: glucb32.h,v 1.1 2000-01-27 21:49:12 sandervl Exp $ */
/*****************************************************************************/
/*                                                                           */
/* Glu/Glut Callback logic - Header Files                                    */
/*                                                                           */
/*****************************************************************************/

typedef void (* APIENTRY GLUOS2CALLBACK)();
typedef void (* WIN32API GLUWINCALLBACK)();

typedef struct tagGLUCB                /* GLU Callback Wrapper structure   */
  {
    GLUOS2CALLBACK   entryAddress;
    GLbyte           objtype;
    void            *object;
    GLenum           which;
    GLint            parms_on_stack;
    struct tagGLUCB *Next;
  }GLUCB;

#define GLUCB_NURBS    1
#define GLUCB_QUADRIC  2
#define GLUCB_TESS     3

#define GLUTCB_DISPLAY          100
#define GLUTCB_IDLE             101
#define GLUTCB_KEYBOARD         102
#define GLUTCB_MENU             103
#define GLUTCB_MENUSTATE        104
#define GLUTCB_MOUSE            105
#define GLUTCB_MOTION           106
#define GLUTCB_PASSIVEMOTION    107
#define GLUTCB_RESHAPE          108
#define GLUTCB_SPECIAL          109
#define GLUTCB_TIMER            110
#define GLUTCB_VISIBILITY       111

#define GLUT_OBJECT             0x80000000

extern GLUOS2CALLBACK OS2glucbRegisterCallback(GLbyte  objtype,
                                                void   *object,
                                                GLenum  which,
                                                GLUWINCALLBACK CallBackFunc);
                                   /* GLvoid  (* WIN32API CallBackFunc)());*/
extern void           OS2glucbDestroyGlucb(GLUCB *glucb);
extern void           OS2glucbDestroyCallbackForObject(GLbyte objtype,void *object);
extern void           OS2glucbDestroyCallbackForObjectWhich(GLbyte objtype,void *object,GLenum which);
extern void           OS2glucbSetWhich(GLUOS2CALLBACK pfn,GLenum which);

/*****************************************************************************/
/*                                                                           */
/* Externals                                                                 */
/*                                                                           */
/*****************************************************************************/

extern GLUCB *Glucbs;
extern HMTX   hmtxGlucb;
