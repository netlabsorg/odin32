// This example illustrates rethrowing an exception.

#include <stdlib.h>
#include <iostream.h>
class FileIO
{
public:
      int reason1;
      int reason2;
      FileIO() {reason1 = reason2 = 0;}; // initialize data members

      // the following member function throws an exception
      void foo(int arg) {
          FileIO fio;

          // throw different exception depending on arg
          switch (arg)
          {
              case 1:
                  fio.reason1 = 1;
                  break;
              case 2:
                  fio.reason2 = 1;
                  break;                           
          }
          throw fio;
      };
};
//      .
//      .
//      .
void f(int arg)
{
      FileIO fio;
      try
      {
            // call member functions of FileIO class
            fio.foo(arg);
      }

      catch(FileIO fexc)
      {
            if (fexc.reason1)
                  cout << "Exception #1" << endl;
            else if (fexc.reason2)
                  cout << "Exception #2" << endl;
            else
                  throw;            // rethrow to outer handler
      }
      catch(...) { /* ... */ }      // catch other exceptions
}

void main(int argc, char **argv)
{
      int  arg = 0;

      if (argc == 2)
      {
          arg = atoi(argv[1]);
      }

      try
      {
            f(arg);
      }
      catch(FileIO)
      {
            cout << "Outer Handler" << endl;
      }

      cout << "Finished." << endl;
}
