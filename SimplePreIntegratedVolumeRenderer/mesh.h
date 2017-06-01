#ifndef MESH_H
#define MESH_H


struct Normal
{
float x,y,z;
};

struct Neighbors
{
Neighbors(){tri=0;nbr=0;}
~Neighbors(){delete [] tri;}

void addNeighbor(int triangle)
{
for (int i=0;i<nbr;i++) if (tri[i]==triangle) return;
int *temp=new int[nbr+1];
for (int i=0;i<nbr;i++) temp[i]=tri[i];
temp[nbr]=triangle;
delete [] tri;
tri=temp;
nbr++;
}

int *tri;
int nbr;
};

struct UV
{
  float u,v;
};

struct Color
{
  float r,g,b;
};

struct Vertex
{
  float x,y,z;
};

struct Triangle
{
  unsigned int vert[3];
};


class Mesh
{
public:
  Mesh();
  virtual ~Mesh();

  void initVerts(int nbrVerts);
  void initTris(int nbrTris);

  Vertex   *verts;
  int       nbrVerts;
  UV         *uvs;
  Color    *colors;
  Triangle *tris;
  int       nbrTris;

	void initNeighbors();
	void calcNormals();
	float invSqrt (float x);
	void calcNormal(Vertex& p1,Vertex& p2,Vertex& p3,Normal& n);
	void normalize(Normal& n);
	Neighbors *vertNeigh; // triangles that are adjacent to a vertex
	Normal *vertNorm;
	Normal *triNorm;



};

inline float Mesh::invSqrt (float x)
{
float xhalf = 0.5f*x;
int i = *(int*)&x;
i = 0x5f3759df - (i >> 1); // This line hides a LOT of math!
x = *(float*)&i;
x = x*(1.5f - xhalf*x*x); // repeat this statement for a better approximation
return x;
}

inline void Mesh::calcNormal(Vertex& p1,Vertex& p2,Vertex& p3,Normal& n)
{
	Vertex v1;
	Vertex v2;
	v1.x = p2.x-p1.x;
	v1.y = p2.y-p1.y;
	v1.z = p2.z-p1.z;

	v2.x = p3.x-p2.x;
	v2.y = p3.y-p2.y;
	v2.z = p3.z-p2.z;

	n.x = v1.y*v2.z - v1.z*v2.y;
	n.y = v1.z*v2.x - v1.x*v2.z;
	n.z = v1.x*v2.y - v1.y*v2.x;

	// normalization step
	float l=invSqrt(n.x*n.x+n.y*n.y+n.z*n.z);

	// Possibly unnecessary
	if (l==0.0) l=1.0;

	n.x*=l;
	n.y*=l;
	n.z*=l;
}

inline void Mesh::normalize(Normal& n)
{
	float length=invSqrt(n.x*n.x+n.y*n.y+n.z*n.z);

	// Possibly unnecessary
	if(length == 0.0) length = 1.0;

	n.x *= length;
	n.y *= length;
	n.z *= length;
}

#endif