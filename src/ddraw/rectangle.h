#ifndef __DDRectandle
  #define __DDRectandle

#include <ibase.hpp>

class DDRectangle : public IBase {
typedef IBase
  Inherited;
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
Boolean
  operator == ( const DDRectangle& rectangle ) const,
  operator != ( const DDRectangle& rectangle ) const;

/*--------------------------------- Testing ----------------------------------*/
Boolean
  intersects ( const DDRectangle& rectangle ) const;

Coord
  width() const,
  height() const;

/*--------------------------------- Synonyms ---------------------------------

Coord
  bottom ( ) const,
  left   ( ) const,
  right  ( ) const,
  top    ( ) const;
*/

private:
  long lTop,lLeft;
  long lBottom,lRight;
}; // class DDRectangle

DDRectangle :: DDRectangle ( Coord y1,
                    Coord x1,
                    Coord y2,
                    Coord x2 )
{
  lTop    = y1;
  lLeft   = x1;
  lBottom = y2;
  lRight  = x2;
}

IBase::Boolean DDRectangle :: operator == ( const DDRectangle &aRect ) const
  {
  return ( lTop == aRect.lTop && lLeft == aRect.lLeft
           &&
           lBottom == aRect.lBottom && lRight == aRect.lRight);
  }
IBase::Boolean DDRectangle :: operator != ( const DDRectangle& aRect ) const
  {
  return !( *this == aRect );
  }
IBase::Boolean DDRectangle :: intersects ( const DDRectangle &aRect ) const
{
  return ( lTop < aRect.lBottom && lLeft<aRect.lRight
           &&
           aRect.lTop < lBottom && aRect.lLeft < lRight );
}

DDRectangle::Coord DDRectangle :: width (  ) const
{
  return ( lRight - lLeft);
}

DDRectangle::Coord DDRectangle :: height (  ) const
{
  return ( lBottom - lTop);
}

#endif
