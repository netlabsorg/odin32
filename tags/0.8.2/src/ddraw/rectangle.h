/* $Id: rectangle.h,v 1.7 2001-10-05 12:33:10 sandervl Exp $ */

/*
 * Rectangle class used to keep track of locked rects in surfaces
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __DDRectandle
  #define __DDRectandle

class DDRectangle {
public:
/*------------------------------ Related Types -------------------------------*/
 typedef long Coord;

/*------------------------------- Constructors -------------------------------*/
  DDRectangle ( );


  DDRectangle ( Coord point1X,
               Coord point1Y,
               Coord point2X,
               Coord point2Y );


/*------------------------------- Comparisons --------------------------------*/
BOOL
  operator == ( const DDRectangle &rectangle ) const,
  operator != ( const DDRectangle &rectangle ) const;

/*--------------------------------- Testing ----------------------------------*/
BOOL
  intersects ( const DDRectangle& rectangle ) const;

Coord
  width() const,
  height() const;

  Coord Top() const
  {
    return lTop;
  }

  Coord Left() const
  {
    return lLeft;
  }
  
  Coord Bottom() const
  {
    return lBottom;
  }
  
  Coord Right() const
  {
    return lRight;
  }

  void* GetMemPtr();
  void SetMemPtr(void* NewMemPtr);

private:
  void* pMemPtr; // Pointer to surface memory for this rectangle
  long lTop,lLeft;
  long lBottom,lRight;
}; // class DDRectangle

#endif
