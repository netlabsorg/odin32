/* $Id: stereo.h,v 1.2 2000-05-23 20:34:57 jeroen Exp $ */
/* File name stereov.h
   header file for stereo display driver
***************************************************************
*                     WMesa                                   *
*                     version 2.3                             *
*                                                             *
*                        By                                   *
*                      Li Wei                                 *
*       Institute of Artificial Intelligence & Robotics       *
*       Xi'an Jiaotong University                             *
*       Email: liwei@aiar.xjtu.edu.cn                         *
*       Web page: http://sun.aiar.xjtu.edu.cn                 *
*                                                             *
*              July 7th, 1997                                                 *
***************************************************************

*/

typedef enum VIEW_INDICATOR { FIRST, SECOND};

#define MAXIMUM_DISPLAY_LIST 99

extern GLenum stereoBuffer;

extern GLint displayList;

extern GLint stereo_flag ;

extern GLfloat viewDistance;

extern GLuint viewTag;

extern GLuint displayListBase;

extern GLuint numOfLists;

extern GLenum stereoCompile;

extern GLenum stereoShowing;

extern void glShowStereo(GLuint list);

extern void toggleStereoMode();

