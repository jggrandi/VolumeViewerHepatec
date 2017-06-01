#include "dcimage.h"

DCImage::DCImage()
{
  width=0;
  data=0;
}

DCImage::~DCImage()
{
  delete []data;
}

bool DCImage::load(char *file)
{
  struct stat st;
  stat(file,&st);
  width=sqrtf(st.st_size/3);
  data=new char[st.st_size];

  FILE *pFile=fopen(file,"rb");
  if (!pFile) return false;
  fread(data,1,st.st_size,pFile);
  int result = ferror(pFile);
  if (result) return false;
  fclose(pFile);
  return true;
}