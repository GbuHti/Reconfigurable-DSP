#ifndef REPORT_H
#define REPORT_H

#include <sstream>                                      ///< string streams
#include <iostream>
#include <iomanip>                                      ///< I/O manipulation
#include <string>
#include <stdio.h>                                      ///< standard I/O


using namespace std;

#define REPORT_INFO(source, routine, text) \
{ ostringstream os; \
  string routine_string (routine); \
  int colon_location; \
  if ((colon_location = routine_string.find("::")) != -1) \
  { \
    routine_string.erase(0, colon_location + 2); \
  } \
  os << sc_core::sc_time_stamp() << " -yzb " << routine_string << endl << "      " << text; \
  if (1) \
  { \
    SC_REPORT_INFO(source, os.str().c_str()); \
  } \
}

#endif
