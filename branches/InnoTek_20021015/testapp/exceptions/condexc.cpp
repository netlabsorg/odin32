// This example illustrates a conditional expresion
// used as a throw expression.

#include <iostream.h>
void main() {
      int doit = 1;
      int dont = 0;
      float f = 8.9;
      int i = 7;
      int j = 6;
      try { throw(doit ? i : f); }
      catch (int x)
      {
            cout << "Caught int " << x << endl;
      }
      catch (float x)
      {
            cout << "Caught float " << x << endl;
      }
      catch (double x)
      {
            cout << "Caught double " << x << endl;
      }
      catch (...)
      {
            cout << "Caught something " << endl;
      }
}

