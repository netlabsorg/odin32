// This example illustrates constructors and
// destructors in exception handling.

#include <string.h>            // needed for strcpy
#include <iostream.h>
class Matherr { public: char errname[30]; };
class DivideByZero : public Matherr
{
public:
      DivideByZero() {strcpy (errname, "Division by zero");}
};
double divide(double a, double b)
{
      if (b == 0) throw DivideByZero();
      return a/b;
}

void main()
{
      double a=7,b=0;
      try {divide (a,b);}
      catch (Matherr xx)
      {
            cout << xx.errname << endl;
      }
}

