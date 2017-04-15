#ifndef _READLINE_H
#define _READLINE_H

#include <Stream.h>

#define READLINE_MAX  128

class LineReader {
public:
  LineReader() : bpos(0), complete(false) {}
//  virtual ~LineReader();
  
  char *readln(Stream &stream);
  char *readln(char c);
private:
  char buffer[READLINE_MAX];
  int bpos;
  bool complete;
};

#endif

