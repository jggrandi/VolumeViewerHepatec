
#include <iostream>
#include "objloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>
#include "objloader.h"

Mesh* ObjLoader::ReadMesh(char *fileName)
{
  Mesh *m=new Mesh;

  int i; //Index variable
	FILE *l_file; //File pointer
	unsigned short l_chunk_id; //Chunk identifier
	unsigned int l_chunk_lenght; //Chunk lenght
	unsigned short l_qty; //Number of elements in each chunk
  unsigned short temp;
  char l_char;

	if ((l_file=fopen (fileName, "rb"))== NULL) return 0; //Open the file

	while (ftell (l_file) < filelength (fileno (l_file))) //Loop to scan the whole file 
	{
		//getche(); //Insert this command for debug (to wait for keypress for each chuck reading)

		fread (&l_chunk_id, 2, 1, l_file); //Read the chunk header
		fread (&l_chunk_lenght, 4, 1, l_file); //Read the lenght of the chunk

		switch (l_chunk_id)
        {
			//----------------- MAIN3DS -----------------
			// Description: Main chunk, contains all the other chunks
			// Chunk ID: 4d4d 
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case 0x4d4d: 
			break;    

			//----------------- EDIT3DS -----------------
			// Description: 3D Editor chunk, objects layout info 
			// Chunk ID: 3d3d (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case 0x3d3d:
			break;
			
			//--------------- EDIT_OBJECT ---------------
			// Description: Object block, info for each object
			// Chunk ID: 4000 (hex)
			// Chunk Lenght: len(object name) + sub chunks
			//-------------------------------------------
			case 0x4000: 
	      i=0;
				do
				{
					fread (&l_char, 1, 1, l_file);
					i++;
                }while(l_char != '\0' && i<20);

			break;

			//--------------- OBJ_TRIMESH ---------------
			// Description: Triangular mesh, contains chunks for 3d mesh info
			// Chunk ID: 4100 (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case 0x4100:
			break;
			
			//--------------- TRI_VERTEXL ---------------
			// Description: Vertices list
			// Chunk ID: 4110 (hex)
			// Chunk Lenght: 1 x unsigned short (number of vertices) 
			//             + 3 x float (vertex coordinates) x (number of vertices)
			//             + sub chunks
			//-------------------------------------------
			case 0x4110: 
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
        m->initVerts(l_qty);
        fread ((float*)m->verts, sizeof(float),m->nbrVerts*3, l_file);
		  break;

			//--------------- TRI_FACEL1 ----------------
			// Description: Polygons (faces) list
			// Chunk ID: 4120 (hex)
			// Chunk Lenght: 1 x unsigned short (number of polygons) 
			//             + 3 x unsigned short (polygon points) x (number of polygons)
			//             + sub chunks
			//-------------------------------------------
			case 0x4120:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
        m->initTris(l_qty);
			  for (i=0; i<l_qty; i++)
        {
          
          fread (&temp,sizeof (unsigned short), 1, l_file);
          m->tris[i].vert[0]=temp;
          fread (&temp,sizeof (unsigned short), 1, l_file);
          m->tris[i].vert[1]=temp;
          fread (&temp,sizeof (unsigned short), 1, l_file);
          m->tris[i].vert[2]=temp;
          fread (&temp, sizeof (unsigned short), 1, l_file);
  			}
      break;

			//------------- TRI_MAPPINGCOORS ------------
			// Description: Vertices list
			// Chunk ID: 4140 (hex)
			// Chunk Lenght: 1 x unsigned short (number of mapping points) 
			//             + 2 x float (mapping coordinates) x (number of mapping points)
			//             + sub chunks
			//-------------------------------------------
			case 0x4140:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
        m->initVerts(l_qty);
				fread (m->uvs,sizeof(float),l_qty*2,l_file);
       break;

			//----------- Skip unknow chunks ------------
			//We need to skip all the chunks that currently we don't use
			//We use the chunk lenght information to set the file pointer
			//to the same level next chunk
			//-------------------------------------------
			default:
				 fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
        } 
	}
	fclose (l_file); // Closes the file stream

 	return m; // Returns ok
}