#include <terminat.h>
#include <unexpect.h>
#include <iostream.h>
class X { /* ... */ };
class Y { /* ... */ };
class A { /* ... */ };
// pfv type is pointer to function returning void
typedef void (*pfv)();
void my_terminate()
{      cout << "Call to my terminate" << endl; }
void my_unexpected()
{      cout << "Call to my unexpected" << endl; }
void f() throw(X,Y)      // f() is permitted to throw objects of class
                         // types X and Y only
{
      A aobj;
      throw(aobj); // error, f() throws a class A object
}
main()
{
      pfv old_term = set_terminate(my_terminate);
      pfv old_unex = set_unexpected(my_unexpected);
      try{ f(); }
      catch(X)       { /* ... */ }
      catch(Y)       { /* ... */ }
      catch (...)    { /* ... */ }

      set_unexpected(old_unex);
      try { f();}
      catch(X)       { /* ... */ }
      catch(Y)       { /* ... */ }
      catch (...)    { /* ... */ }
}

