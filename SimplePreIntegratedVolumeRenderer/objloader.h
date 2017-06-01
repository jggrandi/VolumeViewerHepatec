#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "mesh.h"

namespace ObjLoader
{
  Mesh *ReadMesh(char* file);
};

#endif