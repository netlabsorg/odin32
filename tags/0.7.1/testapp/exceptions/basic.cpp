// This example illustrates the basic use of
// try, catch, and throw.

#include <iostream.h>
#include <stdlib.h>
class IsZero { /* ... */ };
void ZeroCheck( int i )
{
         if (i==0)
                throw IsZero();
}
void main()
{
         double a;

         cout << "Enter a number: ";
         cin >> a;
         try
         {
              ZeroCheck( a );
              cout << "Reciprocal is " << 1.0/a << endl;
         }
         catch ( IsZero )
         {
              cout << "Zero input is not valid" << endl;
         }
         cout << "Finished." << endl;
}
