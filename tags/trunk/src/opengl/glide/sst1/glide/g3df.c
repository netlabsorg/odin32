/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
** $Header: /home/ktk/tmp/odin/odin32xp/src/opengl/glide/sst1/glide/g3df.c,v 1.1 2000-02-25 00:31:12 sandervl Exp $
** $Log: g3df.c,v $
** Revision 1.1  2000-02-25 00:31:12  sandervl
** Created new Voodoo 1 Glide dir
**
 * 
 * 6     3/09/97 10:31a Dow
 * Added GR_DIENTRY for di glide functions
**
*/
#include <stdio.h>
#include <string.h>

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>
#include "fxglide.h"
#ifdef __linux__
#include <ctype.h>
#endif

extern const int _grMipMapHostWH[GR_ASPECT_1x8+1][GR_LOD_1+1][2];
extern FxU32 _gr_aspect_index_table[];
extern FxU32 _grMipMapHostSize[4][16];

static FxU16 ReadDataShort( FILE * );
static FxU32 ReadDataLong( FILE * );
static void  Read8Bit( FxU8 *dst, FILE *image, int small_lod, int large_lod, GrAspectRatio_t aspect );
static void  Read16Bit( FxU16 *dst, FILE *image, int small_lod, int large_lod, GrAspectRatio_t aspect );

#if ( ( GLIDE_PLATFORM & ( GLIDE_OS_DOS32 | GLIDE_OS_WIN32 ) ) != 0 )
const char *openmode = "rb";
#else
const char *openmode = "r";
#endif

typedef struct
{
  const char        *name;
  GrTextureFormat_t  fmt;
  FxBool             valid;
} CfTableEntry;

#ifdef __linux__
static void strupr(char *str) {
  while (*str) {
    if (islower(*str)) *str=toupper(*str);
    str++;
  }
}
#endif

/*---------------------------------------------------------------------------
** gu3dfGetInfo
*/
GR_DIENTRY(gu3dfGetInfo, FxBool,
           ( const char *FileName, Gu3dfInfo *Info ))
{
  FILE *image_file;
  FxU32 index, i;
  FxU32 newlines = 0;
  char  version[5];
  char  color_format[10];
  int   aspect_width, aspect_height;
  char  buffer[100];
  int   small_lod, large_lod;
  FxBool ratio_found = FXFALSE;
  FxBool format_found = FXFALSE;
  GrAspectRatio_t wh_aspect_table[] =
  {
    GR_ASPECT_1x1,
    GR_ASPECT_1x2,
    GR_ASPECT_1x4,
    GR_ASPECT_1x8
  };
  GrAspectRatio_t hw_aspect_table[] =
  {
    GR_ASPECT_1x1,
    GR_ASPECT_2x1,
    GR_ASPECT_4x1,
    GR_ASPECT_8x1
  };
  CfTableEntry cftable[] = 
  {
      { "I8",       GR_TEXFMT_INTENSITY_8,        FXTRUE },
      { "A8",       GR_TEXFMT_ALPHA_8,            FXTRUE },
      { "AI44",     GR_TEXFMT_ALPHA_INTENSITY_44, FXTRUE },
      { "YIQ",      GR_TEXFMT_YIQ_422,            FXTRUE },
      { "RGB332",   GR_TEXFMT_RGB_332,            FXTRUE },
      { "RGB565",   GR_TEXFMT_RGB_565,            FXTRUE },
      { "ARGB8332", GR_TEXFMT_ARGB_8332,          FXTRUE },
      { "ARGB1555", GR_TEXFMT_ARGB_1555,          FXTRUE },
      { "AYIQ8422", GR_TEXFMT_AYIQ_8422,          FXTRUE },
      { "ARGB4444", GR_TEXFMT_ARGB_4444,          FXTRUE },
      { "AI88",     GR_TEXFMT_ALPHA_INTENSITY_88, FXTRUE },
      { "P8",       GR_TEXFMT_P_8,                FXTRUE },
      { "AP88",     GR_TEXFMT_AP_88,              FXTRUE },
      { 0, 0, FXFALSE }
  };

  GDBG_INFO((81,"gu3dfGetInfo(%s,0x%x)\n",FileName,Info));
  /*
  ** open the filen
  */
  if( ( image_file = fopen( FileName, openmode ) ) == NULL )
    return FXFALSE;

  /*
  ** check header
  */
  index=0;
  while(newlines<4)  /* Read the header into a buffer. */
    if((buffer[index++]=getc(image_file))=='\n')
      {
    newlines++;
    buffer[index-1] = ' ';
      }
  buffer[--index] = 0;

  /*
  ** grab statistics out of the header
  */
  if( sscanf(buffer,"3df v%s %s lod range: %i %i aspect ratio: %i %i\n",
         &version,
          color_format,
         &small_lod, &large_lod,
         &aspect_width, &aspect_height ) == 0 )
    return FXFALSE;

  /*
  ** determine aspect ratio, height, and width
  */
  i = 0;
  ratio_found = FXFALSE;
  while ( ( i < 4 ) && ( !ratio_found ) )
  {
    if ( ( aspect_width << i ) == aspect_height )
    {
      Info->header.aspect_ratio = wh_aspect_table[i];
      ratio_found = FXTRUE;
    }
    i++;
  }
  i = 0;
  while ( ( i < 4 ) && ( !ratio_found ) )
  {
    if ( ( aspect_height << i ) == aspect_width )
    {
      Info->header.aspect_ratio = hw_aspect_table[i];
      ratio_found = FXTRUE;
    }
    i++;
  }
  if ( !ratio_found )
    return FXFALSE;

  /*
  ** determine height and width of the mip map
  */
  if ( aspect_width >= aspect_height )
  {
    Info->header.width  = large_lod;
    Info->header.height = large_lod / aspect_width;
  }
  else
  {
    Info->header.height = large_lod;
    Info->header.width  = large_lod / aspect_height;
  }


  /*
  ** calculate proper LOD values
  */
  switch ( small_lod )
  {
     case 1:
       Info->header.small_lod = GR_LOD_1;
       break;
     case 2:
       Info->header.small_lod = GR_LOD_2;
       break;
     case 4:
       Info->header.small_lod = GR_LOD_4;
       break;
     case 8:
       Info->header.small_lod = GR_LOD_8;
       break;
     case 16:
       Info->header.small_lod = GR_LOD_16;
       break;
     case 32:
       Info->header.small_lod = GR_LOD_32;
       break;
     case 64:
       Info->header.small_lod = GR_LOD_64;
       break;
     case 128:
       Info->header.small_lod = GR_LOD_128;
       break;
     case 256:
       Info->header.small_lod = GR_LOD_256;
       break;
  }

  switch ( large_lod )
  {
     case 1:
       Info->header.large_lod = GR_LOD_1;
       break;
     case 2:
       Info->header.large_lod = GR_LOD_2;
       break;
     case 4:
       Info->header.large_lod = GR_LOD_4;
       break;
     case 8:
       Info->header.large_lod = GR_LOD_8;
       break;
     case 16:
       Info->header.large_lod = GR_LOD_16;
       break;
     case 32:
       Info->header.large_lod = GR_LOD_32;
       break;
     case 64:
       Info->header.large_lod = GR_LOD_64;
       break;
     case 128:
       Info->header.large_lod = GR_LOD_128;
       break;
     case 256:
       Info->header.large_lod = GR_LOD_256;
       break;
  }

  /*
  ** determine the color format of the input image
  */
  strupr( color_format );

  i = 0;
  format_found = FXFALSE;
  while ( ( cftable[i].name != 0 ) && ( !format_found ) )
  {
    if ( strcmp( color_format, cftable[i].name ) == 0 )
    {
      Info->header.format = cftable[i].fmt;
      format_found = FXTRUE;
    }
    i++;
  }

  /*
  ** close the input file
  */
  fclose( image_file );

  if ( format_found ) {
      FxI32 lod;
      Info->mem_required = 0;
      for( lod = Info->header.large_lod; lod <= Info->header.small_lod; lod++ ) {
          Info->mem_required +=
          _grMipMapHostSize[_gr_aspect_index_table[Info->header.aspect_ratio]]
                           [lod] << (Info->header.format>=GR_TEXFMT_16BIT);
      }
  }

  return format_found;
}

/*---------------------------------------------------------------------------
** gu3dfLoad
*/
GR_DIENTRY(gu3dfLoad, FxBool, ( const char *filename, Gu3dfInfo *info ))
{
  FILE *image_file  = 0;
  FxU32 index       = 0;
  FxU32 newlines    = 0;
  char  buffer[100] = "";

  GDBG_INFO((81,"gu3dfLoad(%s,0x%x)\n",filename,info));
  /*
  ** open the file
  */
  if ( ( image_file = fopen( filename, openmode ) ) == NULL )
    return FXFALSE;

  /*
  ** examine the header
  */
  index = 0;
  while ( newlines < 4 )
  {
    if ( ( buffer[index++] = getc( image_file ) ) == '\n')
    {
      newlines++;
      buffer[index-1] = ' ';
    }
  }
  buffer[--index] = 0;

  /*
  ** If necessary, read in the YIQ decompression table
  */
  if ( ( info->header.format == GR_TEXFMT_YIQ_422 ) ||
       ( info->header.format == GR_TEXFMT_AYIQ_8422 ) )
  {
    /*
    ** read in Y
    */
    for ( index = 0; index < 16; index++ )
      info->table.nccTable.yRGB[index] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0xFF;

    /*
    ** read in I
    */
    for ( index = 0; index < 4; index++ )
    {
      info->table.nccTable.iRGB[index][0] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0x1FF;
      info->table.nccTable.iRGB[index][1] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0x1FF;
      info->table.nccTable.iRGB[index][2] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0x1FF;
    }

    /*
    ** read in Q
    */
    for ( index = 0; index < 4; index++ )
    {
      info->table.nccTable.qRGB[index][0] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0x1FF;
      info->table.nccTable.qRGB[index][1] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0x1FF;
      info->table.nccTable.qRGB[index][2] = ( ( FxI16 ) ReadDataShort( image_file ) ) & 0x1FF;
    }

    /*
    ** pack the table Y entries
    */
    for ( index = 0; index < 4; index++ )
    {
       FxU32 packedvalue;

       packedvalue  = ( ( FxU32 ) info->table.nccTable.yRGB[index*4+0] );
       packedvalue |= ( ( FxU32 ) info->table.nccTable.yRGB[index*4+1] ) << 8;
       packedvalue |= ( ( FxU32 ) info->table.nccTable.yRGB[index*4+2] ) << 16;
       packedvalue |= ( ( FxU32 ) info->table.nccTable.yRGB[index*4+3] ) << 24;
                                           
       info->table.nccTable.packed_data[index] = packedvalue;
    }

    /*
    ** pack the table I entries
    */
    for ( index = 0; index < 4; index++ )
    {
       FxU32 packedvalue;

       packedvalue  = ( ( FxU32 ) info->table.nccTable.iRGB[index][0] ) << 18;
       packedvalue |= ( ( FxU32 ) info->table.nccTable.iRGB[index][1] ) << 9;
       packedvalue |= ( ( FxU32 ) info->table.nccTable.iRGB[index][2] ) << 0;

       info->table.nccTable.packed_data[index+4] = packedvalue;
    }

    /*
    ** pack the table Q entries
    */
    for ( index = 0; index < 4; index++ )
    {
       FxU32 packedvalue;

       packedvalue  = ( ( FxU32 ) info->table.nccTable.qRGB[index][0] ) << 18;
       packedvalue |= ( ( FxU32 ) info->table.nccTable.qRGB[index][1] ) << 9;;
       packedvalue |= ( ( FxU32 ) info->table.nccTable.qRGB[index][2] ) << 0;

       info->table.nccTable.packed_data[index+8] = packedvalue;
    }
  }

  /*
  ** If necessary, read in the Palette
  */
  if ( ( info->header.format == GR_TEXFMT_P_8 ) ||
       ( info->header.format == GR_TEXFMT_AP_88 ) ) {
      FxU32 i;
      for( i = 0; i < 256; i++ )
          info->table.palette.data[i] = ReadDataLong( image_file );
  }

  /*
  ** Read in the image
  */
  switch ( info->header.format )
  {
  case GR_TEXFMT_INTENSITY_8:
  case GR_TEXFMT_ALPHA_8:
  case GR_TEXFMT_ALPHA_INTENSITY_44:
  case GR_TEXFMT_YIQ_422:
  case GR_TEXFMT_RGB_332:
  case GR_TEXFMT_P_8:
    Read8Bit( (FxU8 *)info->data, image_file, info->header.small_lod, info->header.large_lod, info->header.aspect_ratio );
    break;
  case GR_TEXFMT_RGB_565:
  case GR_TEXFMT_ARGB_8332:
  case GR_TEXFMT_ARGB_1555:
  case GR_TEXFMT_AYIQ_8422:
  case GR_TEXFMT_ARGB_4444:
  case GR_TEXFMT_ALPHA_INTENSITY_88:
  case GR_TEXFMT_AP_88:
    Read16Bit( (FxU16 *)info->data, image_file, info->header.small_lod, info->header.large_lod, info->header.aspect_ratio );
    break;

  default:
    return FXFALSE;
  }

  /*
  ** close the file
  */
  fclose( image_file );

  return FXTRUE;
}

/*
** Read8Bit
**
** Read in an 8-bit texture map, unpacked.
*/
static void Read8Bit( FxU8 *data, FILE *image_file, int small_lod, int large_lod, GrAspectRatio_t aspect_ratio )
{
  int lod;
  int width, height;

  for ( lod = large_lod; lod <= small_lod; lod++ )
  {
    width  = _grMipMapHostWH[aspect_ratio][lod][0];
    height = _grMipMapHostWH[aspect_ratio][lod][1];

    fread( data, sizeof( char ), width*height, image_file );
    data += width*height;
  }
}

/*
** Read16Bit
**
** Read in a 16-bit texture map, unpacked.
*/
static void Read16Bit( FxU16 *data, FILE *image_file, int small_lod, int large_lod, GrAspectRatio_t aspect_ratio )
{
  int index;
  int lod;
  int width, height;

  for ( lod = large_lod; lod <= small_lod; lod++ )
  {
    width  = _grMipMapHostWH[aspect_ratio][lod][0];
    height = _grMipMapHostWH[aspect_ratio][lod][1];

    for ( index = 0; index < ( width * height ); index++ )
    {
      *data = ReadDataShort( image_file );
      data++;
    }
  }
}

/*
** FxU16 ReadDataShort
*/
static FxU16 ReadDataShort( FILE *fp )
{
  FxU16 value;

  /*
  ** read in the MSB
  */
  value = ( ( FxU16 ) getc( fp ) ) << 8;

  /*
  ** read in the LSB
  */
  value |= ( getc( fp ) & 0x00FF );

  return value;
}

/*
** ReadDataLong
*/
static FxU32 ReadDataLong( FILE *fp )
{
    FxU32 data;
    FxU8 byte[4];

    fread(byte, 4, 1, fp);
    data = (((FxU32) byte[0]) << 24) |
           (((FxU32) byte[1]) << 16) |
           (((FxU32) byte[2]) <<  8) |
            ((FxU32) byte[3]);

    return data;
}



