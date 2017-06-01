#ifndef DCIMAGE_H
#define DCIMAGE_H

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// Loads square, square of two
// raw files with rgb rgb rgb ... aligmnent
// with 0 header size
// and 8 bits per color channel

class DCImage
{
public:
  DCImage();
  ~DCImage();
  bool load(char* file);

  char* data;
  int   width;
};

#endif