#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>

int parseVertexCount(const char *input);
int isEmptyLine(const char *input);

#endif

#ifdef _WIN32
#  ifdef BUILD_DLL
#    define API __declspec(dllexport)
#  else
#    define API __declspec(dllimport)
#  endif
#else
#  define API
#endif