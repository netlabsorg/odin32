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
  height() const,
  Top() const,
  Left() const,
  Bottom() const,
  Right() const;

  void* GetMemPtr();
  void SetMemPtr(void* NewMemPtr);

private:
  void* pMemPtr; // Pointer to surface memory for this rectangle
  long lTop,lLeft;
  long lBottom,lRight;
}; // class DDRectangle

#endif
