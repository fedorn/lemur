
#include "main.hpp"
#include "Exception.hpp"


int main(int argc, char *argv[])
{
  int result = 1;
  try {
    result = AppMain(argc,argv);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    cerr << "Program aborted due to exception" << endl;;
  } 
  return result;
}
