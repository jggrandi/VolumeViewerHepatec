#include "mesh.h"

Mesh::Mesh()
{
  verts=0;
  colors=0;
  tris=0;
  uvs=0;

  nbrVerts=0;
  nbrTris=0;
}

Mesh::~Mesh()
{
  delete [] verts;
  delete [] uvs;
  delete [] colors;
  delete [] tris;
}

void Mesh::initVerts(int p_nbrVerts)
{
  if (verts) return;
  nbrVerts=p_nbrVerts;
  verts =new Vertex[nbrVerts];
  uvs   =new     UV[nbrVerts];
  colors=new Color[nbrVerts];
  vertNorm=new Normal[nbrVerts];  
  vertNeigh=new Neighbors[nbrVerts];

}

void Mesh::initTris(int p_nbrTris)
{
  nbrTris=p_nbrTris;
  tris=new Triangle[nbrTris];
  triNorm=new Normal[nbrTris];
}
void Mesh::initNeighbors()
{
  for (int t=0;t<nbrTris;t++)
  {
    for (int v=0;v<3;v++)
    {
      vertNeigh[tris[t].vert[v]].addNeighbor(t);
    }
  }
}

void Mesh::calcNormals()
{
  // First calculate the triangle normals
  for (int t=0;t<nbrTris;t++)
  {
    calcNormal(verts[tris[t].vert[0]],
               verts[tris[t].vert[1]],
               verts[tris[t].vert[2]],
               triNorm[t]);
  }
  // Now calculate the vertex normals
  for (int v=0;v<nbrVerts;v++)
  {
    vertNorm[v].x=vertNorm[v].y=vertNorm[v].z=0;
    for (int t=0;t<vertNeigh[v].nbr;t++)
    {
      vertNorm[v].x+=triNorm[vertNeigh[v].tri[t]].x;
      vertNorm[v].y+=triNorm[vertNeigh[v].tri[t]].y;
      vertNorm[v].z+=triNorm[vertNeigh[v].tri[t]].z;
    }
    normalize(vertNorm[v]);
  }
}

