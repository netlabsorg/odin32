/* $Id: glut32.cpp,v 1.1 2000-01-27 21:49:13 sandervl Exp $ */
/*****************************************************************************/
/*                                                                           */
/* GLUT32 API                                                                */
/*                                                                           */
/*****************************************************************************/
#define INCL_BASE
#include <os2.h>
#include <os2wrap.h>
#include <misc.h>
#include <malloc.h>
#include <memory.h>

#include "glos2.h"
#include "gluos2.h"
#include "glutos2.h"
#include "glucb32.h"

/*****************************************************************************/
/*                                                                           */
/* glut Functions                                                            */
/*                                                                           */
/*****************************************************************************/

void WIN32API glutInit(int *argcp, char **argv)
{
  dprintf(("glutInit argcp: %08X, argv %08X\n",argcp,argv));

  OS2glutInit(argcp,argv);

  dprintf(("glutInit complete\n"));
}

void WIN32API glutInitDisplayMode(unsigned long mode)
{
  dprintf(("glutInitDisplayMode - mode %08X\n",mode));

  OS2glutInitDisplayMode(mode);
}

void WIN32API glutInitWindowPosition(int x,int y)
{
  dprintf(("glutInitWindowPosition\n"));
  OS2glutInitWindowPosition(x,y);
}

void WIN32API glutInitWindowSize(int width,int height)
{
  dprintf(("glutInitWindowSize\n"));
  OS2glutInitWindowSize(width,height);
}

void WIN32API glutMainLoop(void)
{
  dprintf(("glutMainLoop\n"));
  OS2glutMainLoop();
}

int WIN32API glutCreateWindow(char *title)
{
  int rc;

  dprintf(("glutCreateWindow - title %s\n",title));

  rc=OS2glutCreateWindow(title);

  dprintf(("glutCreateWindow - Created - id %d\n",rc));

  return rc;
}

int WIN32API glutCreateSubWindow(int win,int x,int y,int width,int height)
{
  return OS2glutCreateSubWindow(win,x,y,width,height);
}

void WIN32API glutDestroyWindow(int win)
{
  OS2glutDestroyWindow(win);

  OS2glucbDestroyCallbackForObject(GLUT_OBJECT,(void *)win);
}

void WIN32API glutPostRedisplay(void)
{
  OS2glutPostRedisplay();
}

void WIN32API glutSwapBuffers(void)
{
  OS2glutSwapBuffers();
}

int WIN32API glutGetWindow(void)
{
  return OS2glutGetWindow();
}

void WIN32API glutSetWindow(int win)
{
  OS2glutSetWindow(win);
}

void WIN32API glutSetWindowTitle(char *title)
{
  OS2glutSetWindowTitle(title);
}

void WIN32API glutSetIconTitle(char *title)
{
  OS2glutSetIconTitle(title);
}

void WIN32API glutPositionWindow(int x,int y)
{
  OS2glutPositionWindow(x,y);
}

void WIN32API glutReshapeWindow(int width,int height)
{
  OS2glutReshapeWindow(width,height);
}

void WIN32API glutPopWindow(void)
{
  OS2glutPopWindow();
}

void WIN32API glutPushWindow(void)
{
  OS2glutPushWindow();
}

void WIN32API glutIconifyWindow(void)
{
  OS2glutIconifyWindow();
}

void WIN32API glutShowWindow(void)
{
  OS2glutShowWindow();
}

void WIN32API glutHideWindow(void)
{
  OS2glutHideWindow();
}

int WIN32API glutCreateMenu(void (* WIN32API fn)(int))
{
  /* NOTE: The registration-function should NOT deregister any others! */
  int            menu;
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_MENU,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  menu=OS2glutCreateMenu((void (* APIENTRY)(int))pfn);

  /* Now we know the menu set the 'object' of the correct glucb to this value */
  OS2glucbSetWhich(pfn,menu);

  return menu;
}

void WIN32API glutDestroyMenu(int menu)
{
  int win=OS2glutGetWindow();

  OS2glutDestroyMenu(menu);

  OS2glucbDestroyCallbackForObjectWhich(GLUTCB_MENU,(void *)win,menu);
}

int WIN32API glutGetMenu(void)
{
  return OS2glutGetMenu();
}

void WIN32API glutSetMenu(int menu)
{
  OS2glutSetMenu(menu);
}

void WIN32API glutAddMenuEntry(char *label,int value)
{
  OS2glutAddMenuEntry(label,value);
}

void WIN32API glutAddSubMenu(char *label,int submenu)
{
  OS2glutAddSubMenu(label,submenu);
}

void WIN32API glutChangeToMenuEntry(int item,char *label,int value)
{
  OS2glutChangeToMenuEntry(item,label,value);
}

void WIN32API glutChangeToSubMenu(int item,char *label,int submenu)
{
  OS2glutChangeToSubMenu(item,label,submenu);
}

void WIN32API glutRemoveMenuItem(int item)
{
  OS2glutRemoveMenuItem(item);
}

void WIN32API glutAttachMenu(int button)
{
  OS2glutAttachMenu(button);
}

void WIN32API glutDetachMenu(int button)
{
  OS2glutDetachMenu(button);
}

void WIN32API glutDisplayFunc(void (* WIN32API fn)(void))
{
  GLUOS2CALLBACK pfn;

  dprintf(("glutDisplayFunc\n"));

  pfn=OS2glucbRegisterCallback(GLUTCB_DISPLAY,
                               (void *)OS2glutGetWindow(),
                               0,
                               fn);

  OS2glutDisplayFunc(pfn);
}

void WIN32API glutReshapeFunc(void (* WIN32API fn)(int width, int height))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_RESHAPE,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutReshapeFunc((void (* APIENTRY)(int, int))pfn);
}

void WIN32API glutKeyboardFunc(void (* WIN32API fn)(unsigned char key,int x,int y))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_KEYBOARD,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutKeyboardFunc((void (* APIENTRY)(unsigned char, int, int))pfn);
}

void WIN32API glutMouseFunc(void (* WIN32API fn)(int button,int state,int x,int y))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_MOUSE,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutMouseFunc((void (* APIENTRY)(int, int, int, int))pfn);
}

void WIN32API glutMotionFunc(void (* WIN32API fn)(int x,int y))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_MOTION,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutMotionFunc((void (* APIENTRY)(int, int))pfn);
}

void WIN32API glutPassiveMotionFunc(void (* WIN32API fn)(int x, int y))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_PASSIVEMOTION,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutPassiveMotionFunc((void (* APIENTRY)(int, int))pfn);
}

void WIN32API glutEntryFunc(void (* WIN32API fn)(int state))
{
  dprintf(("GLUT32: glutEntryFunc - not implemented\n"));
}

void WIN32API glutVisibilityFunc(void (* WIN32API fn)(int state))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_VISIBILITY,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutVisibilityFunc((void (* APIENTRY)(int))pfn);
}

void WIN32API glutIdleFunc(void (* WIN32API fn)(void))
{
  /* NOTE: There is only a single IDLE function at any time */
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_IDLE,
                                              0,
                                              0,
                                              fn);

  OS2glutIdleFunc(pfn);
}

void WIN32API glutTimerFunc(unsigned long millis, void (* WIN32API fn)(int value),int value)
{
  /* NOTE: Timers can NEVER be deregistered - make sure of this in the GLUCB */
  /* code!                                                                   */
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_TIMER,
                                              0,
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutTimerFunc(millis,
                   (void (* APIENTRY)(int))pfn,
                   value);
}

void WIN32API glutMenuStateFunc(void (* WIN32API fn)(int state))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_MENUSTATE,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutMenuStateFunc((void (* APIENTRY)(int))pfn);
}

void WIN32API glutSpecialFunc(void (* WIN32API fn)(int key,int x,int y))
{
  GLUOS2CALLBACK pfn=OS2glucbRegisterCallback(GLUTCB_SPECIAL,
                                              (void *)OS2glutGetWindow(),
                                              0,
                                              (GLUWINCALLBACK)fn);

  OS2glutSpecialFunc((void (* APIENTRY)(int, int, int))pfn);
}

void WIN32API glutSpaceballMotionFunc(void (* WIN32API fn)(int x,int y,int z))
{
  dprintf(("GLUT32: glutSpaceballMotionFunc - not implemented\n"));
}

void WIN32API glutSpaceballRotateFunc(void (* WIN32API fn)(int x,int y,int z))
{
  dprintf(("GLUT32: glutSpaceballRotateFunc - not implemented\n"));
}

void WIN32API glutSpaceballButtonFunc(void (* WIN32API fn)(int button,int state))
{
  dprintf(("GLUT32: glutSpaceballButtonFunc - not implemented\n"));
}

void WIN32API glutButtonBoxFunc(void (* WIN32API fn)(int button,int state))
{
  dprintf(("GLUT32: glutButtonBoxFunc - not implemented\n"));
}

void WIN32API glutDialsFunc(void (* WIN32API fn)(int dial,int value))
{
  dprintf(("GLUT32: glutDialsFunc - not implemented\n"));
}

void WIN32API glutTabletMotionFunc(void (* WIN32API fn)(int x,int y))
{
  dprintf(("GLUT32: glutTabletMotionFunc - not implemented\n"));
}

void WIN32API glutTabletButtonFunc(void (* WIN32API fn)(int button,int state,int x,int y))
{
  dprintf(("GLUT32: glutTabletButtonFunc - not implemented\n"));
}

void WIN32API glutSetColor(int index,GLfloat red,GLfloat green,GLfloat blue)
{
  OS2glutSetColor(index,red,green,blue);
}

GLfloat WIN32API glutGetColor(int index,int component)
{
  return OS2glutGetColor(index,component);
}

void WIN32API glutCopyColormap(int win)
{
  OS2glutCopyColormap(win);
}

int WIN32API glutGet(GLenum type)
{
  return OS2glutGet(type);
}

int WIN32API glutDeviceGet(GLenum type)
{
  return OS2glutDeviceGet(type);
}

void WIN32API glutStrokeCharacter(void *font,int character)
{
  OS2glutStrokeCharacter(font,character);
}

void WIN32API glutBitmapCharacter(void *font,int character)
{
  OS2glutBitmapCharacter(font,character);
}

void WIN32API glutWireSphere(GLdouble radius,GLint slices,GLint stacks)
{
  OS2glutWireSphere(radius,slices,stacks);
}

void WIN32API glutSolidSphere(GLdouble radius,GLint slices,GLint stacks)
{
  OS2glutSolidSphere(radius,slices,stacks);
}

void WIN32API glutWireCone(GLdouble base,GLdouble height,GLint slices,GLint stacks)
{
  OS2glutWireCone(base,height,slices,stacks);
}

void WIN32API glutSolidCone(GLdouble base,GLdouble height,GLint slices,GLint stacks)
{
  OS2glutSolidCone(base,height,slices,stacks);
}

void WIN32API glutWireCube(GLdouble size)
{
  OS2glutWireCube(size);
}

void WIN32API glutSolidCube(GLdouble size)
{
  OS2glutSolidCube(size);
}

void WIN32API glutWireTorus(GLdouble innerRadius,GLdouble outerRadius,GLint sides,GLint rings)
{
  OS2glutWireTorus(innerRadius,outerRadius,sides,rings);
}

void WIN32API glutSolidTorus(GLdouble innerRadius,GLdouble outerRadius,GLint sides,GLint rings)
{
  OS2glutSolidTorus(innerRadius,outerRadius,sides,rings);
}

void WIN32API glutWireDodecahedron(void)
{
  OS2glutWireDodecahedron();
}
void WIN32API glutSolidDodecahedron(void)
{
  OS2glutSolidDodecahedron();
}

void WIN32API glutWireTeapot(GLdouble size)
{
  OS2glutWireTeapot(size);
}

void WIN32API glutSolidTeapot(GLdouble size)
{
  OS2glutSolidTeapot(size);
}

void WIN32API glutWireOctahedron(void)
{
  OS2glutWireOctahedron();
}

void WIN32API glutSolidOctahedron(void)
{
  OS2glutSolidOctahedron();
}

void WIN32API glutWireTetrahedron(void)
{
  OS2glutWireTetrahedron();
}

void WIN32API glutSolidTetrahedron(void)
{
  OS2glutSolidTetrahedron();
}

void WIN32API glutWireIcosahedron(void)
{
  OS2glutWireIcosahedron();
}

void WIN32API glutSolidIcosahedron(void)
{
  OS2glutSolidIcosahedron();
}

int WIN32API glutExtensionSupported(char *name)
{
  return OS2glutExtensionSupported(name);
}

/*****************************************************************************/
/*                                                                           */
/* Unsupported calls                                                         */
/*                                                                           */
/*****************************************************************************/

int WIN32API glutBitmapLength(void *font,const unsigned char *string)
{
  dprintf(("GLUT32: glutBitmapLength - not implemented\n"));
  return 0;
}

int WIN32API glutBitmapWidth(void *font, int character)
{
  dprintf(("GLUT32: glutBitmapWidth - not implemented\n"));
  return 0;
}

int WIN32API glutEnterGameMode(void)
{
  dprintf(("GLUT32: glutEnterGameMode - not implemented\n"));
  return 0;
}

void WIN32API glutEstablishOverlay(void)
{
  dprintf(("GLUT32: glutEstablishOverlay - not implemented\n"));
}

void WIN32API glutForceJoystickFunc(void)
{
  dprintf(("GLUT32: glutForceJoystickFunc - not implemented\n"));
}

void WIN32API glutFullScreen(void)
{
  dprintf(("GLUT32: glutFullScreen - not implemented\n"));
}

int WIN32API glutGameModeGet(GLenum mode)
{
  dprintf(("GLUT32: glutGameModeGet - not implemented\n"));
  return 0;
}

void WIN32API glutGameModeString(const char *string)
{
  dprintf(("GLUT32: glutGameModeString - not implemented\n"));
}

int WIN32API glutGetModifiers(void)
{
  dprintf(("GLUT32: glutGetModifiers - not implemented\n"));
  return 0;
}

void WIN32API glutHideOverlay(void)
{
  dprintf(("GLUT32: glutHideOverlay - not implemented\n"));
}

void WIN32API glutIgnoreKeyRepeat(int ignore)
{
  dprintf(("GLUT32: glutIgnoreKeyRepeat - not implemented\n"));
}

void WIN32API glutInitDisplayString(const char *string)
{
  dprintf(("GLUT32: glutDisplayString - not implemented\n"));
}

void WIN32API glutJoystickFunc(void (* WIN32API fn)(int, int, int, int),int pollinterval)
{
  dprintf(("GLUT32: glutJoystickFunc  - not implemented\n"));
}

void WIN32API glutKeyboardUpFunc(void (* WIN32API fn)(unsigned char, int, int))
{
  dprintf(("GLUT32: glutKeyboardUpFunc - not implemented\n"));
}

int WIN32API glutLayerGet(GLenum type)
{
  dprintf(("GLUT32: glutLayerGet - not implemented\n"));
  return 0;
}

void WIN32API glutLeaveGameMode(void)
{
  dprintf(("GLUT32: glutLeaveGameMode - not implemented\n"));
}

void WIN32API glutMenuStatusFunc(void (* WIN32API fn)(int, int))
{
  dprintf(("GLUT32: glutMenuStatusFunc - not implemented\n"));
}

void WIN32API glutOverlayDisplayFunc(void (* WIN32API fn)(void))
{
  dprintf(("GLUT32: glutOverlayDisplayFunc - not implemented\n"));
}

void WIN32API glutPostOverlayRedisplay(void)
{
  dprintf(("GLUT32: glutPostOverlayRedislay - not implemented\n"));
}

void WIN32API glutPostWindowOverlayRedisplay(int win)
{
  dprintf(("GLUT32: glutPostWindowOverlayRedislay - not implemented\n"));
}

void WIN32API glutPostWindowRedisplay(int win)
{
  dprintf(("GLUT32: glutPostWindowRedislay - not implemented\n"));
}

void WIN32API glutRemoveOverlay(void)
{
  dprintf(("GLUT32: glutRemoveOverlay - not implemented\n"));
}

void WIN32API glutReportErrors(void)
{
  dprintf(("GLUT32: glutReportErrors - not implemented\n"));
}

void WIN32API glutSetCursor(int cursor)
{
  dprintf(("GLUT32: glutSetCursor - not implemented\n"));
}

void WIN32API glutSetKeyRepeat(int repeatMode)
{
  dprintf(("GLUT32: glutSetKeyRepeat - not implemented\n"));
}

void WIN32API glutSetupVideoResizing(void)
{
  dprintf(("GLUT32: glutSetupVideoResizing - not implemented\n"));
}

void WIN32API glutShowOverlay(void)
{
  dprintf(("GLUT32: glutShowOverlay - not implemented\n"));
}

void WIN32API glutSpecialUpFunc(void (* WIN32API fn)(int, int, int))
{
  dprintf(("GLUT32: glutSpecialUpFunc - not implemented\n"));
}

void WIN32API glutStopVideoResizing(void)
{
  dprintf(("GLUT32: glutStopVideoResizing - not implemented\n"));
}

int WIN32API glutStrokeLength(void *font,const unsigned char *string)
{
  dprintf(("GLUT32: glutStrokeLength - not implemented\n"));
  return 0;
}

int WIN32API glutStrokeWidth(void *font, int character)
{
  dprintf(("GLUT32: glutStrokeWidth - not implemented\n"));
  return 0;
}

void WIN32API glutUseLayer(GLenum layer)
{
  dprintf(("GLUT32: glutUseLayer - not implemented\n"));
}

void WIN32API glutVideoPan(int x, int y, int width, int height)
{
  dprintf(("GLUT32: glutVideoPan - not implemented\n"));
}

void WIN32API glutVideoResize(int x, int y, int width, int height)
{
  dprintf(("GLUT32: glutVideoResize - not implemented\n"));
}

int WIN32API glutVideoResizeGet(GLenum param)
{
  dprintf(("GLUT32: glutVideoResizeGet - not implemented\n"));
  return 0;
}

void WIN32API glutWarpPointer(int x,int y)
{
  dprintf(("GLUT32: glutWarpPointer - not implemented\n"));
}

int WIN32API glutWindowStatusFunc(void (* WIN32API fn)(int))
{
  dprintf(("GLUT32: glutWindowStatusFunc - not implemented\n"));
  return 0;
}

/*****************************************************************************/
/*                                                                           */
/* Module Internal init and cleanup - called by initterm                     */
/*                                                                           */
/*****************************************************************************/

void mod_init(void)
{
  dprintf(("GLUT32 INIT\n"));
}

void mod_cleanup(void)
{
  dprintf(("GLUT32 TERM\n"));
}
