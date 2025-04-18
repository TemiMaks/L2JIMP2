#ifndef CSRRG_H
#define CSRRG_H

int processCsrrgFile(const char *fileName);

#endif //CSRRG_H

#ifdef _WIN32
#  ifdef BUILD_DLL
#    define API __declspec(dllexport)
#  else
#    define API __declspec(dllimport)
#  endif
#else
#  define API
#endif