
#include "main.hpp"
#include "Exception.hpp"
#include "Param.hpp"
#include <iostream.h>


int main(int argc, char *argv[])
{
  if (argc==2 && strcmp(argv[1], "-help") &&
      strcmp(argv[1], "--help") &&
      strcmp(argv[1], "-h") ) {  // recognize parameter file as argument
    ParamPushFile(argv[1]);
  } else if (argc==2) { // handle "-help", display parameters 
    GetAppParam(); // called only  to "register" parameters
    cerr << "Parameters for " <<  argv[0] << endl;
    ParamDisplay();
    exit(0);
  } 
  GetAppParam();

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
