/* $Id: rectangle.cpp,v 1.7 2001-10-05 12:33:10 sandervl Exp $ */

/*
 * Rectangle class Implementaion
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

typedef unsigned long BOOL;

#include "rectangle.h"

DDRectangle::DDRectangle ( Coord x1,
                    Coord y1,
                    Coord x2,
                    Coord y2 )
{
  lTop    = y1;
  lLeft   = x1;
  lBottom = y2;
  lRight  = x2;
  pMemPtr = 0;
}

BOOL DDRectangle::operator == ( const DDRectangle &aRect ) const
  {
  return ( lTop == aRect.lTop && lLeft == aRect.lLeft
           &&
           lBottom == aRect.lBottom && lRight == aRect.lRight);
  }
BOOL DDRectangle::operator != ( const DDRectangle& aRect ) const
  {
  return !( *this == aRect );
  }
BOOL DDRectangle::intersects ( const DDRectangle &aRect ) const
{
  return ( lTop < aRect.lBottom && lLeft<aRect.lRight
           &&
           aRect.lTop < lBottom && aRect.lLeft < lRight );
}

DDRectangle::Coord DDRectangle::width (  ) const
{
  return ( lRight - lLeft);
}

DDRectangle::Coord DDRectangle::height (  ) const
{
  return ( lBottom - lTop);
}

void DDRectangle::SetMemPtr(void* NewMemPtr)
{
  pMemPtr = NewMemPtr;
}

void* DDRectangle::GetMemPtr()
{
  return pMemPtr;
}
