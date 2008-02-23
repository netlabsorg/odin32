/* $Id: rectangle.cpp,v 1.1 2000-04-07 18:21:14 mike Exp $ */

/*
 * Rectangle class Implementaion
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

typedef long BOOL;

#include "rectangle.h"

DDRectangle::DDRectangle ( Coord y1,
                    Coord x1,
                    Coord y2,
                    Coord x2 )
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

DDRectangle::Coord DDRectangle::Top() const
{
  return lTop;
}

DDRectangle::Coord DDRectangle::Left() const
{
  return lLeft;
}

DDRectangle::Coord DDRectangle::Bottom() const
{
  return lBottom;
}

DDRectangle::Coord DDRectangle::Right() const
{
  return lRight;
}

void DDRectangle::SetMemPtr(void* NewMemPtr)
{
  pMemPtr = NewMemPtr;
}

void* DDRectangle::GetMemPtr()
{
  return pMemPtr;
}
