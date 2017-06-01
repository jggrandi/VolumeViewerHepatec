// Body01.
#define MINIMUM_VALUE 0//(-1024)
#define MAXIMUM_VALUE 3071

// fengine.
//#define MINIMUM_VALUE 0
//#define MAXIMUM_VALUE 255

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "mesh.h"
#include "objloader.h"

#include <GL/glui.h>
#include <PinholeCamera.h>
#include <ErrorManager.h>
#include <GageAdaptor.h>
#include <VertexPool.h>
#include <BoundingBox.h>
#include <Texture.h>
#include <Shader.h>
#include <MyGLH.h>
#include <MyMath.h>
#include <Timer.h>

#include "wsg.h"

#include "VolumeArray.h"

#include "SimpleMeasure.h"

//#include "CoordinateSystemHelper.h"


using namespace std;

// Image.
string m_imageName;
boost::shared_ptr<CGageAdaptor> m_image;

	int radioObj = 0;
int m_voxelsDoVolume =0;
int m_voxelsLoboEsquerdo =0;
int m_voxelsLoboDireito =0;
float m_volumeDeUmVoxel =0.0;
int showBody = 0;
int showVeinsMesh = 0;
int showLiverMesh = 0;
int meshAlpha =0;

enum ApplicationModeEnum {
	// In this mode, the left button of the mouse displaces the image in the XY 
	// axis of the camera and the right button of the mouse changes camera's 
	// near plane size.
	MOVINGBODYMODE,
	// In this mode, the left button of the mouse adds a new point to the current 
	// volume
	ADDINGPOINTSMODE,
	// In this mode, the left button of the mouse changes the window (transfer 
	// function) of the current slice (m_currentSlice).
	LBUTTON2WINDOW,

	SELECTINGVOXEL
};

#define MOVING_BODY_ID			1010
//#define ADDING_POINTS_ID		1020
//#define NEW_LAYER_ID			1030
//#define LAYER_FORWARD_ID		1040
//#define LAYER_BACKWARD_ID		1050
#define VOLUME_FORWARD_ID		1060
#define VOLUME_BACKWARD_ID		1070
#define VOLUME_BODY_ID			1075
#define REGULAGEM_JANELA_ID		1080
//#define SELECT_VOXEL_ID			1090
//#define FLOOD_FILL_ID			1100
#define VORONOI_ALPHA_ID		1110
//#define SHOW_DATASET_ID			1000
//#define SHOW_MOLDE_ID			1001
//#define SHOW_SEGMENTADO_ID		1002
//#define SAVE_SEGMENTS_ID		1003
//#define SAVE_DENSITY_ID			1005
//#define SAVE_SEGMENTS_RAW_ID	1006
//#define LOAD_SEGMENTS_ID		1007
#define VORONOI_DRENAGEM_ID		1
#define VORONOI_ESQ_DIR_ID		2
#define VORONOI_TUMOR_ID		3
#define VORONOI_DESLIGADO_ID	0
#define RADIO_ID				1011

	std::string m_fileName;

	Volume m_volumeData;
//	Volume m_volumeMolde;
boost::shared_ptr<Volume> m_voronoiVolume;

boost::shared_ptr<Volume> m_currentVolume;
boost::shared_ptr<VolumeArray> m_volumeArray;
	//vector<short> m_volumeTextureDataLabel;
int m_currentID = 0;

	int m_selectedVoxel[3];

	float	m_floodFillRange;

GLUI_RadioGroup *radio;

ApplicationModeEnum               m_applicationMode = MOVINGBODYMODE;

// .
boost::shared_ptr<CTexture> m_volumeTexture;
boost::shared_ptr<CTexture> m_voronoiVolumeTexture;
int m_voronoiAlpha = 10;
//CTexture m_preIntegrationTableTexture;
CTexture m_preIntegrationTableTexture;
CTexture m_voronoiTags;
CBoundingBox m_meshBoundingBox;
int m_width = 1;
int m_height = 1;
int m_depth = 1;
GLubyte m_lookUpTable[4*256];
int m_windowCenter = 400;
int m_windowWidth = 50;
CShader m_preIntegrationShader;
// Camera.
boost::shared_ptr<CPinholeCamera> m_camera;
int m_mouseButton = -1;
int m_mouseX = -1;
int m_mouseY = -1;
// Adding points.
boost::shared_ptr<CSimpleMeasure>   m_currentMeasureLayer;
vector <boost::shared_ptr<CSimpleMeasure>> m_simpleMeasureVector;
float m_alphaThreshold = 0.025f;


ApplicationModeEnum GetApplicationMode(void);
//bool measureLayerForward();
//bool measureLayerBackward();
//bool AddMeasureLayer(void);
void DrawSlices(void);
void GetColorMapping(int normalizedValue, unsigned char *r, unsigned char *g, unsigned char *b);
void InitializeLookUpTable(int windowCenter, int windowWidth);
GLubyte clamp(int value, int minValue, int maxValue);
bool createPreintegrationTable(GLubyte* Table);
bool InitializeShaders(void);
void OnDestroy(void);
void OnPaint(void);
void OnSize(GLsizei w, GLsizei h);
void OnIdle(void);
void OnKeyDown(unsigned char key, int x, int y);
//float getClosest(int pos[3]);
//void calcEsqDir();
//bool calcVoronoi();
void OnMouse(int button, int state, int x, int y);
void OnMouseMove(int x, int y);
void OnPassiveMouseMove(int x, int y);
bool SetApplicationMode(ApplicationModeEnum applicationMode);
void setMode( int control );
bool OnCreate(int argc, char** argv);
//void onSave(std::string name2save, std::string tipoDeArquivo);
bool LoadVolume(char* fileName = NULL);
bool LoadVoronoiVolume(char* fileName = NULL);
//void onLoad(std::string name2save);

int volumeFigado=0;



//*******************************************

Mesh *fromFile;
Mesh *fromFile2;

void drawMesh(Mesh& m)
{
	//if (smooth)
	{
		glVertexPointer(3,GL_FLOAT,0,m.verts);
		glNormalPointer(GL_FLOAT,0,m.vertNorm);
		glDrawElements (GL_TRIANGLES,m.nbrTris*3,GL_UNSIGNED_INT,m.tris);
	}
	/*else
	{
		glBegin(GL_TRIANGLES);
		for (int i=0;i<m.nbrTris;i++)
		{
			glNormal3f(m.triNorm[i].x,m.triNorm[i].y,m.triNorm[i].z);
			for (int v=0;v<3;v++)
			{
				glVertex3f(m.verts[m.tris[i].vert[v]].x,
				m.verts[m.tris[i].vert[v]].y,
				m.verts[m.tris[i].vert[v]].z);
			}
		}
		glEnd();
	}*/
}

void setupRC(){

	glDepthFunc(GL_LESS);		// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
	glFrontFace(GL_CCW);	    // Counterclockwise polygons face out

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);    // Clear the color and depth buffer

	glEnable( GL_LIGHTING );

    GLfloat diffuseLight[] = { 0.7, 0.7, 0.7, 1.0 };	/// RGBA, vary the 3 first paramaters to change the light color and intensity.   
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );	/// Create the light source
    glEnable( GL_LIGHT0 );					/// Light it!


	GLfloat specularLight[] = { 0.7, 0.7, 0.7, 0.2 };
GLfloat spectre[] = { 1.0, 1.0, 1.0, 1.0 };

glLightfv( GL_LIGHT0, GL_SPECULAR, specularLight );
glMaterialfv( GL_FRONT, GL_SPECULAR, spectre );
glMateriali( GL_FRONT, GL_SHININESS, 128 );

	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	// NEW to initialize the vertex array drawing
// used by glDrawElements,glVertexPointer and glColorPointer
glEnableClientState(GL_VERTEX_ARRAY);
// glEnableClientState(GL_COLOR_ARRAY);

	glEnable( GL_NORMALIZE );
	glEnable(GL_CULL_FACE);
		//glEnable( GL_NORMALIZE );
glEnableClientState(GL_NORMAL_ARRAY);

} 

//*******************************************/

/*

bool measureLayerForward()
{
	for (int i = 0 ; i < m_simpleMeasureVector.size() ; i++){
		if (m_simpleMeasureVector[i] == m_currentMeasureLayer)
		{
			if (i < m_simpleMeasureVector.size()-1)
				i++;
			//m_currentLayer = i;
			m_currentMeasureLayer = (m_simpleMeasureVector[i]);
			return true;
		}
	}
	return false;
}

bool measureLayerBackward()
{
	for (int i = 0 ; i < m_simpleMeasureVector.size() ; i++)
	{
		if (m_simpleMeasureVector[i] == m_currentMeasureLayer)
		{
			if (i > 0)
				i--;
			//m_currentLayer = i;
			m_currentMeasureLayer = (m_simpleMeasureVector[i]);
			return true;
		}
	}
	return false;
}

*/

/**
*/
/*
bool AddMeasureLayer(void)
{
	boost::shared_ptr<CSimpleMeasure>  simpleMeasure;

	simpleMeasure.reset(new CSimpleMeasure);
	if (!simpleMeasure.get())
	{
		MarkError();

		return false;
	}

	m_currentMeasureLayer.reset(new CSimpleMeasure);

	if (!m_currentMeasureLayer.get())
	{
		MarkError();

		return false;
	}


	m_simpleMeasureVector.push_back(simpleMeasure);
	m_currentMeasureLayer = simpleMeasure;
	return true;

};
*/
/**
*/
void DrawSlices(void)
{
	float modelViewMatrix[16],
		u[3],
		v[3],
		eyeVector[3];

	m_preIntegrationShader.Bind();

	m_volumeTexture->Bind(0);
	m_preIntegrationShader.SetTextureUnit("Volume", 0);

	m_voronoiVolumeTexture->Bind(1);
	m_preIntegrationShader.SetTextureUnit("Voronoi", 1);

	m_preIntegrationTableTexture.Bind(2);

	m_preIntegrationShader.SetTextureUnit("PreIntegrationTable", 2);

	m_voronoiTags.Bind(3);
	m_preIntegrationShader.SetTextureUnit("VoronoiColors", 3);

	//m_preIntegrationShader.SetUniformParameter("SliceDistance", 0.0);
	m_preIntegrationShader.SetUniformParameter("SliceDistance", SPACING);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);


	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);

	u[0] = modelViewMatrix[0];
	u[1] = modelViewMatrix[4];
	u[2] = modelViewMatrix[8];

	v[0] = modelViewMatrix[1];
	v[1] = modelViewMatrix[5];
	v[2] = modelViewMatrix[9];

	MyMath::CrossProduct<float>(v, u, eyeVector);

	glColor3f(1.0f, 0.0f, 1.0f);

	plot(eyeVector);

	glDisable(GL_BLEND);

	CTexture::DisableTextureUnit(0);
	CTexture::DisableTextureUnit(1);
	CTexture::DisableTextureUnit(2);
	CTexture::DisableTextureUnit(3);

	CShader::Disable();

}

/**
*/
void GetColorMapping(int normalizedValue, unsigned char *r, unsigned char *g, unsigned char *b)
{
	int unnormalizedKeyValueArray[13] = {
		-2048,
		-1024,
		-741,
		-70,
		37,
		42,
		53,
		75,
		98,
		190,
		260,
		1376,
		3071  /*
		500,//-741,
		600,//-70,
		700,//37,
		1000,//42,
		1300,//53,
		1500,//75,
		2000,//98,
		4000,//190,
		10000,//260,
		35000,//1376,
		50000//3071  */
	};
	unsigned char colorMappingArray[13][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{70, 28, 19},
		{255, 214, 152},
		{176, 140, 156},
		//{100, 42, 31},
		{128, 42, 31},
		//{68, 31, 25},
		{128, 31, 25},
		{149, 130, 100},
		{255, 255, 255},
		{164, 150, 123},
		{161, 146, 115},
		{161, 146, 115},
		{255, 255, 255}
	};
	int unnormalizedValue,
		i;
	float t;

	*r = 0;
	*g = 0;
	*b = 0;

	unnormalizedValue = MINIMUM_VALUE + (float(normalizedValue)/255.0f)*float(MAXIMUM_VALUE - MINIMUM_VALUE);

	for (i=0; i<(13 - 1); i++)
		if ((unnormalizedValue >= unnormalizedKeyValueArray[i]) &&(unnormalizedValue < unnormalizedKeyValueArray[i + 1]))
		{
			t = float(unnormalizedValue - unnormalizedKeyValueArray[i])/float(unnormalizedKeyValueArray[i + 1] - unnormalizedKeyValueArray[i]);

			*r = (1.0f - t)*float(colorMappingArray[i][0]) + t*float(colorMappingArray[i + 1][0]);
			*g = (1.0f - t)*float(colorMappingArray[i][1]) + t*float(colorMappingArray[i + 1][1]);
			*b = (1.0f - t)*float(colorMappingArray[i][2]) + t*float(colorMappingArray[i + 1][2]);
		}
}

/**
*/
void InitializeLookUpTable(int windowCenter, int windowWidth)
{
	int i;
	float windowCenterNormalized, 
		windowBeginNormalized,
		windowEndNormalized;
	unsigned char level;

	windowCenterNormalized = 255.0f*(float(windowCenter - MINIMUM_VALUE)/(MAXIMUM_VALUE - MINIMUM_VALUE));

	windowBeginNormalized = float(windowCenterNormalized) - 0.5f*float(windowWidth);
	windowEndNormalized = windowBeginNormalized + float(windowWidth);

	for (i=0; i<256; ++i)
	{
		if (i > windowEndNormalized)
			level = 255;
		else if (i > windowBeginNormalized)
			level = unsigned char(255.0f*((float(i) - windowBeginNormalized)/float(windowWidth)));
		else
			level = 0;

		GetColorMapping(i, &m_lookUpTable[4*i + 0], &m_lookUpTable[4*i + 1], &m_lookUpTable[4*i + 2]);
		m_lookUpTable[4*i + 3] = level;
	}

	cout << "Window center: " << windowCenter << endl;
	cout << "Window width: " << int((float(windowWidth)/255.0f)*5119.0f) << endl;
}

/**
@inproceedings{1103929,
 author = {Klaus Engel and Markus Hadwiger and Joe M. Kniss and Aaron E. Lefohn and Christof Rezk Salama and Daniel Weiskopf},
 title = {Real-time volume graphics},
 booktitle = {SIGGRAPH '04: ACM SIGGRAPH 2004 Course Notes},
 year = {2004},
 pages = {29},
 location = {Los Angeles, CA},
 doi = {http://doi.acm.org/10.1145/1103900.1103929},
 publisher = {ACM},
 address = {New York, NY, USA},
 }
*/
GLubyte clamp(int value, int minValue, int maxValue)
{
	if (value < minValue)
		return (GLubyte)minValue;
	else if (value > maxValue)
		return (GLubyte)maxValue;

	return (GLubyte)value;
}

/**
@inproceedings{1103929,
 author = {Klaus Engel and Markus Hadwiger and Joe M. Kniss and Aaron E. Lefohn and Christof Rezk Salama and Daniel Weiskopf},
 title = {Real-time volume graphics},
 booktitle = {SIGGRAPH '04: ACM SIGGRAPH 2004 Course Notes},
 year = {2004},
 pages = {29},
 location = {Los Angeles, CA},
 doi = {http://doi.acm.org/10.1145/1103900.1103929},
 publisher = {ACM},
 address = {New York, NY, USA},
 }
*/
bool createPreintegrationTable(GLubyte* Table) 
{
	double r,
		g,
		b,
		a,
		rInt[256],
		gInt[256],
		bInt[256],
		aInt[256],
		factor,
		// ?
		tauc;
	int rcol,
		gcol,
		bcol,
		acol,
		smin,
		smax,
		lookupindex;
	GLubyte lookupImg[256*256*4];
	CTimer timer;

	r = 0.0;
	g = 0.0;
	b = 0.0;
	a = 0.0;

	rInt[0] = 0.0;
	gInt[0] = 0.0;
	bInt[0] = 0.0;
	aInt[0] = 0.0;

	lookupindex = 0;

	// compute integral functions
	for (int i=1; i<256; i++)
	{
		tauc = (Table[(i - 1)*4 + 3] + Table[i*4 + 3])/2.0;
		r = r + (Table[(i - 1)*4 + 0] + Table[i*4 + 0])/2.0*tauc/255.0;
		g = g + (Table[(i - 1)*4 + 1] + Table[i*4 + 1])/2.0*tauc/255.0;
		b = b + (Table[(i - 1)*4 + 2] + Table[i*4 + 2])/2.0*tauc/255.0;
		a = a + tauc;

		rInt[i] = r;
		gInt[i] = g;
		bInt[i] = b;
		aInt[i] = a;
	}

	// compute look-up table from integral functions
	for (int sb=0; sb<256; sb++)
		for (int sf=0; sf<256; sf++)
		{
			if (sb < sf)
			{
				smin = sb;
				smax = sf;
			}
			else
			{
				smin = sf;
				smax = sb;
			}

			if (smax != smin)
			{
				factor = 1.0/(double)(smax - smin);

				rcol = (rInt[smax] - rInt[smin])*factor;
				gcol = (gInt[smax] - gInt[smin])*factor;
				bcol = (bInt[smax] - bInt[smin])*factor;
				acol = 256.0*(1.0 - exp(-(aInt[smax] - aInt[smin])*factor/255.0));
			}
			else
			{
				factor = 1.0/255.0;

				rcol = Table[smin*4+0]*Table[smin*4+3]*factor;
				gcol = Table[smin*4+1]*Table[smin*4+3]*factor;
				bcol = Table[smin*4+2]*Table[smin*4+3]*factor;
				acol = (1.0 - exp(-Table[smin*4 + 3]*factor))*256.0;
			}

			lookupImg[lookupindex++] = clamp(rcol, 0, 255);
			lookupImg[lookupindex++] = clamp(gcol, 0, 255);
			lookupImg[lookupindex++] = clamp(bcol, 0, 255);
			lookupImg[lookupindex++] = clamp(acol, 0, 255);
		}


	if (!m_preIntegrationTableTexture.IsValid())
	{
		if (!m_preIntegrationTableTexture.Initialize(CTexture::TEXTURE_2D, CTexture::RGBA))
		{
			MarkError();

			return false;
		}

		m_preIntegrationTableTexture.SetParameter(CTexture::MIN_FILTER, CTexture::LINEAR);
		m_preIntegrationTableTexture.SetParameter(CTexture::MAG_FILTER, CTexture::LINEAR);
		m_preIntegrationTableTexture.SetParameter(CTexture::WRAP_S, CTexture::CLAMP_TO_EDGE);
		m_preIntegrationTableTexture.SetParameter(CTexture::WRAP_T, CTexture::CLAMP_TO_EDGE);
	}

	if (!m_voronoiTags.IsValid())
	{
		if (!m_voronoiTags.Initialize(CTexture::TEXTURE_2D, CTexture::RGBA))
		{
			MarkError();

			return false;
		}

		m_voronoiTags.SetParameter(CTexture::MIN_FILTER, CTexture::LINEAR);
		m_voronoiTags.SetParameter(CTexture::MAG_FILTER, CTexture::LINEAR);
		m_voronoiTags.SetParameter(CTexture::WRAP_S, CTexture::CLAMP_TO_EDGE);
		m_voronoiTags.SetParameter(CTexture::WRAP_T, CTexture::CLAMP_TO_EDGE);
	}

	if (!m_preIntegrationTableTexture.SetImage(256, 256, 0, CTexture::RGBA, CTexture::UNSIGNED_BYTE, lookupImg))
	{
		MarkError();

		return false;
	}
	GLubyte lookupImgColors[10*3];
	lookupindex=0;
	acol=50;
	int VoronoiColors[10*4]={0,0,0,0,
		0,0,0,0,//0,255,255,m_voronoiAlpha,
		255,255,0,m_voronoiAlpha,
		255,0,255,m_voronoiAlpha,
		0,255,255,m_voronoiAlpha,
		255,0,0,m_voronoiAlpha,
		0,0,255,m_voronoiAlpha,
		0,255,0,m_voronoiAlpha,
		127,0,255,m_voronoiAlpha,
		255,0,0,m_voronoiAlpha};

	for (int i = 0; i < 10 ; i ++)
	{

		 	lookupImgColors[lookupindex++] = clamp(VoronoiColors[lookupindex], 0, 255);
			lookupImgColors[lookupindex++] = clamp(VoronoiColors[lookupindex], 0, 255);
			lookupImgColors[lookupindex++] = clamp(VoronoiColors[lookupindex], 0, 255);
			lookupImgColors[lookupindex++] = clamp(VoronoiColors[lookupindex], 0, 255);
			
	}

	if (!m_voronoiTags.SetImage(10, 1, 0, CTexture::RGBA, CTexture::UNSIGNED_BYTE, lookupImgColors))
	{
		MarkError();

		return false;
	}

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	cout << "Pre-integration table creation elapsed time: " << long(timer.GetElapsed()) << " ms." << endl;

	return true;
}

/**
*/
bool InitializeShaders(void)
{
	if (!m_preIntegrationShader.Initialize())
	{
		MarkError();

		return false;
	}

	if (!m_preIntegrationShader.LoadFromFile("preIntegrationVertexShader.glsl", CShader::VERTEX_PROGRAM))
	{
		MarkError();

		return false;
	}

	if (!m_preIntegrationShader.LoadFromFile("preIntegrationPixelShader.glsl", CShader::FRAGMENT_PROGRAM))
	{
		MarkError();

		return false;
	}

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
void OnDestroy(void)
{
	int windowName;

	windowName = glutGetWindow();

	if (windowName)
		glutDestroyWindow(windowName);

	if (!CErrorManager::Instance().IsOk())
		CErrorManager::Instance().FlushErrors();

	exit(EXIT_SUCCESS);
}
  	CSimpleMeasure m_mouseLine;
/**
*/
void OnPaint(void)
{
	float width,
		height,
		depth,
		sizeMax;

	glAlphaFunc(GL_GREATER, m_alphaThreshold);

	glEnable(GL_ALPHA_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    m_camera->ApplyTransform();

    glEnable(GL_DEPTH_TEST);

	//MyGLH::DrawGrid();
	
	width = m_currentVolume->GetBoundingBox().m_xMax - m_currentVolume->GetBoundingBox().m_xMin ;// m_meshBoundingBox.m_xMax - m_meshBoundingBox.m_xMin;
	height = m_currentVolume->GetBoundingBox().m_yMax - m_currentVolume->GetBoundingBox().m_yMin ; //m_meshBoundingBox.m_yMax - m_meshBoundingBox.m_yMin;
	depth = m_currentVolume->GetBoundingBox().m_zMax - m_currentVolume->GetBoundingBox().m_zMin ; //m_meshBoundingBox.m_zMax - m_meshBoundingBox.m_zMin;

	sizeMax = max(width, max(height, depth));

	glScalef(width/sizeMax, height/sizeMax, depth/sizeMax);



    glPolygonOffset(1.0, 1.0);

	glEnable(GL_POLYGON_OFFSET_FILL);
	
	DrawSlices();

	glDisable(GL_POLYGON_OFFSET_FILL);
	
	GLfloat colorAux;
	for (int i = 0 ; i < m_simpleMeasureVector.size() ; i++){

		if (m_simpleMeasureVector[i] == m_currentMeasureLayer && GetApplicationMode() == ADDINGPOINTSMODE)
		{
			
			glLineWidth(3.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			m_currentMeasureLayer->AddPoint(m_mouseX,m_mouseY);
			m_currentMeasureLayer->SaveObjectTransform();
			m_currentMeasureLayer->Draw();
			m_currentMeasureLayer->RemovePoint();

			glLineWidth(1.0f);
			
		}else 
		{

			glPointSize(4.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			//colorAux+=0.2;
			//glColor3f(colorAux, 1-colorAux, colorAux*2);
			m_simpleMeasureVector[i]->SaveObjectTransform();
			m_simpleMeasureVector[i]->DrawPoints();
		}
	}

	glColor3f(1.0f, 0.0f, 0.0f);

	//********************************************************
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (meshAlpha)
		glEnable(GL_BLEND);

	glShadeModel(GL_SMOOTH);
	if (showVeinsMesh){
		glPushMatrix();
		glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
		glTranslatef(-0.02,-0.085,-0.22);
		glScalef(0.004,0.004,0.006);
		glRotatef(180.0,0,0,1); // Uncomment for rotation
		drawMesh(*fromFile);
		glPopMatrix();
	}

	if (showLiverMesh){
		glPushMatrix();
		glColor4f(1.0f, 0.0f, 0.0f, 0.6f);
		glScalef(0.0043,0.0043,0.0068);
		glTranslatef(120,130,-88);
		glRotatef(180.0,0,0,1); // Uncomment for rotation
		drawMesh(*fromFile2);
		glPopMatrix();	
	}

		glDisable(GL_BLEND);

	//	glutSolidTeapot(1.0);
	
	
	//********************************************************/



    glDisable(GL_DEPTH_TEST);

	glDisable(GL_ALPHA_TEST);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	


	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	m_preIntegrationTableTexture.Bind(0);

	glColor3f(1.0f, 1.0f, 1.0f);

/*	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-0.5f, 0.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(-1.0f, 0.5f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-0.5f, 1.0f);

	glEnd();
*/
	CTexture::DisableTextureUnit(0);
/*
	glBegin(GL_LINE_LOOP);

	glVertex2f(0.5f, 0.5f);
	glVertex2f(1.0f, 0.5f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(0.5f, 1.0f);

	glEnd();

*/

/*
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glColor3ub( 0, 0, 0 );
	glRasterPos2i( 2, 2 );
	///*  printf( "text: %s\n", text );              
	///*** Render the live character array 'text' **
	char stringrender[20]="total de voxels: ";

	//itoa (volumeFigado, stringrender,10);
	for( int i=0; i<(int)strlen( stringrender ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, stringrender[i] );

	char string2render[10];

	itoa (volumeFigado, string2render,10);
	for( int i=0; i<(int)strlen( string2render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string2render[i] );
	
	glRasterPos2i( 35, 20 );
	char string3render[20]="volume total: ";

	//itoa (volumeFigado, string3render,10);
	for( int i=0; i<(int)strlen( string3render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string3render[i] );

	m_voxelsDoVolume = volumeFigado * (m_volumeDeUmVoxel*100.0);
	float volume_total =(float)volumeFigado * m_volumeDeUmVoxel;
	int vol_inteiro = m_voxelsDoVolume/1000;
	
	itoa (vol_inteiro, string2render,10);
	for( int i=0; i<(int)strlen( string2render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string2render[i] );
	glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, ',' );
	vol_inteiro = m_voxelsDoVolume%1000;
	itoa (vol_inteiro, string2render,10);
	for( int i=0; i<(int)strlen( string2render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string2render[i] );
	char string4render[20]="cm3 ";
	for( int i=0; i<(int)strlen( string4render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string4render[i] );

/*	glRasterPos2i( 35, 15 );
	char stringDireito[20];
	int auxdireito = m_voxelsLoboDireito * (m_volumeDeUmVoxel*100.0);
	int vol_direito = auxdireito/1000;
	itoa (vol_direito, stringDireito,10);
	for( int i=0; i<(int)strlen( stringDireito ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, stringDireito[i] );
	
	glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, ',' );
	
	vol_direito = auxdireito%1000;
	itoa (vol_direito, stringDireito,10);
	for( int i=0; i<(int)strlen( stringDireito ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, stringDireito[i] );
	
//	stringDireito="cm3 ";
	for( int i=0; i<(int)strlen( string4render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string4render[i] );


	
	glRasterPos2i( 35, 10 );
	char stringEsquerda[20];
	int auxesquerdo = m_voxelsLoboEsquerdo * (m_volumeDeUmVoxel*100.0);
	int vol_esquerdo = auxesquerdo/1000;
	itoa (vol_esquerdo, stringEsquerda,10);
	for( int i=0; i<(int)strlen( stringEsquerda ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, stringEsquerda[i] );
	
	glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, ',' );
	
	vol_esquerdo = auxesquerdo%1000;
	itoa (vol_esquerdo, stringEsquerda,10);
	for( int i=0; i<(int)strlen( stringEsquerda ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, stringEsquerda[i] );
	
//	stringEsquerda="cm3 ";
	for( int i=0; i<(int)strlen( string4render ); i++ )
	  glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string4render[i] );
*/


	glutSwapBuffers();
}

/**
*/
void OnSize(GLsizei w, GLsizei h)
{
    if (h == 0)
        h = 1;
    
    glViewport(0, 0, w, h);

    m_camera->SetViewport(0, 0, w, h);
}

/**
*/
void OnIdle(void)
{
    glutPostRedisplay();
}



void OnKeyDown(unsigned char key, int x, int y) 
{
    switch (key)
    {
	case 'w':
	case 'W':
		m_windowCenter += 5;
		
		if (m_windowCenter > MAXIMUM_VALUE)
			m_windowCenter = MAXIMUM_VALUE;

		InitializeLookUpTable(m_windowCenter, m_windowWidth);
		
		createPreintegrationTable(m_lookUpTable);
		break;
	case 's':
	case 'S':
		m_windowCenter -= 5;
		
		if (m_windowCenter < MINIMUM_VALUE)
			m_windowCenter = MINIMUM_VALUE;

		InitializeLookUpTable(m_windowCenter, m_windowWidth);
		
		createPreintegrationTable(m_lookUpTable);
		break;
	case 'a':
	case 'A':
		m_windowWidth -= 2;
		
		if (m_windowWidth < 0)
			m_windowWidth = 0;

		InitializeLookUpTable(m_windowCenter, m_windowWidth);
		
		createPreintegrationTable(m_lookUpTable);
		break;
	case 'd':
	case 'D':
		m_windowWidth += 2;
		
		InitializeLookUpTable(m_windowCenter, m_windowWidth);
		
		createPreintegrationTable(m_lookUpTable);
		break;
	case 'o':
	case 'O':
		m_alphaThreshold += 0.025f;

		if (m_alphaThreshold > 1.0f) {
			m_alphaThreshold = 1.0f;
		}

		cout << "Alpha threshold level: " << m_alphaThreshold << endl;
		break;
	case 'l':
	case 'L':
		m_alphaThreshold -= 0.025f;

		if (m_alphaThreshold < 0.0f) {
			m_alphaThreshold = 0.0f;
		}

		cout << "Alpha threshold level: " << m_alphaThreshold << endl;
		break;
    case 27:
        OnDestroy();
        
        exit(EXIT_SUCCESS);
        break;
    }
    
    glutPostRedisplay();
}
/*
vector<vector<int>> m_tagedVoxelArray;
float getClosest(int pos[3])
{
	int closestIndex=0;

	if (m_simpleMeasureVector.empty())
		return 0;

//	std::vector<int>	aux = m_simpleMeasureVector[0];
boost::shared_ptr<CSimpleMeasure> auxMeasure = m_simpleMeasureVector[0];
//std::vector<std::vector<int>>	aux = auxMeasure->voxelsVolume;
	if (auxMeasure->voxelsVolume.empty())
		return 0;

	float closestHipotenusa= (float((pos[0]-auxMeasure->voxelsVolume[0][0])*(pos[0]-auxMeasure->voxelsVolume[0][0])+(pos[1]-auxMeasure->voxelsVolume[0][1])*(pos[1]-auxMeasure->voxelsVolume[0][1])));
	closestHipotenusa = (float((closestHipotenusa)+(pos[2]-auxMeasure->voxelsVolume[0][2])*(pos[2]-auxMeasure->voxelsVolume[0][2])));

	for (int i = 0 ; i < m_simpleMeasureVector.size() ; i++){
		auxMeasure = m_simpleMeasureVector[i];
	//	aux = auxMeasure->voxelsVolume;
		for (int j = 0 ; j < auxMeasure->voxelsVolume.size() ; j++){
			float hipotenusa= (float((pos[0]-auxMeasure->voxelsVolume[j][0])*(pos[0]-auxMeasure->voxelsVolume[j][0])+(pos[1]-auxMeasure->voxelsVolume[j][1])*(pos[1]-auxMeasure->voxelsVolume[j][1])));
			hipotenusa = (float((hipotenusa)+(pos[2]-auxMeasure->voxelsVolume[j][2])*(pos[2]-auxMeasure->voxelsVolume[j][2])));
			if (hipotenusa<closestHipotenusa)
			{	
				closestHipotenusa=hipotenusa;
				closestIndex=i;
			}
		}

		//if (m_simpleMeasureVector[i] == m_currentMeasureLayer)
		//{
		//	if (i < m_simpleMeasureVector.size()-1)
		//		i++;
			//m_currentLayer = i;
		//	m_currentMeasureLayer = (m_simpleMeasureVector[i]);
		//	return true;
		//}
	}

//	cout << closestIndex << endl;
	return (float)closestIndex/10+0.1;

}
*/
void calcEsqDir(){
m_voxelsLoboDireito=0;
m_voxelsLoboEsquerdo=0;
float *volumeVor = m_voronoiVolume->GetData();

	for (int k = 0 ; k < m_depth ; k++)
	{
		for (int j = 0 ; j < m_height ; j++)
		{
			for (int i = 0 ; i < m_width ; i++)
			{

				int pos3d[3];
				pos3d[2]= k;
				pos3d[0]= i;
				pos3d[1]= j;

				if (volumeVor[i+j*m_width+(k*m_width*m_height)]>0.05f && volumeVor[i+j*m_width+(k*m_width*m_height)]<0.15f )
					m_voxelsLoboDireito++;
				else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.15f && volumeVor[i+j*m_width+(k*m_width*m_height)]<0.25f )
					m_voxelsLoboEsquerdo++;
			}
		}
	}
}

/*
bool calcVoronoi()
{
	CTimer timer;
	boost::shared_ptr<Volume> volumeAux;
	 m_volumeArray->GetVolume(1,volumeAux);
	float *volume =	volumeAux->GetData();
	float *volume2 = m_voronoiVolume->GetData();
	for (int k = 0 ; k < m_depth ; k++)
	{
		for (int j = 0 ; j < m_height ; j++)
		{
			for (int i = 0 ; i < m_width ; i++)
			{

				int pos3d[3];
				pos3d[2]= k;
				pos3d[0]= i;
				pos3d[1]= j;

				if (volume[i+j*m_width+(k*m_width*m_height)]>0.3f)
					volume2[i+j*m_width+(k*m_width*m_height)]=getClosest(pos3d);
			}
		}
	}
	//calcEsqDir();
	m_voronoiVolume->SetTexture();
	m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);

	cout << "Voronoi elapsed time: " << timer.GetElapsed() << endl;

	return true;
}*/
/**
*/
void OnMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) 
	{
		switch (GetApplicationMode())
		{

	/*	case SELECTINGVOXEL:
			if (button == GLUT_LEFT_BUTTON)
			{
				float xyz[3];
				m_currentMeasureLayer->GetCandidatePoint(x, y, xyz);
				m_selectedVoxel[0]=m_width*(xyz[0]+0.5);
				m_selectedVoxel[1]=m_height*(xyz[1]+0.5);
				m_selectedVoxel[2]=m_depth*(xyz[2]+0.5);
				m_selectedVoxel[0]=m_width - m_selectedVoxel[0];
				vector<int> temp;
				temp.push_back(m_selectedVoxel[0]);
				temp.push_back(m_selectedVoxel[1]);
				temp.push_back(m_selectedVoxel[2]);

				m_tagedVoxelArray.push_back(temp);

				calcVoronoi();


			}
	*//*	case ADDINGPOINTSMODE:
			if (button == GLUT_LEFT_BUTTON)
			{
				float xyz[3];
				m_currentMeasureLayer->AddPoint(x, y);
				m_currentMeasureLayer->GetCandidatePoint(x, y, xyz);
				m_selectedVoxel[0]=m_width*(xyz[0]+0.5);
				m_selectedVoxel[1]=m_height*(xyz[1]+0.5);
				m_selectedVoxel[2]=m_depth*(xyz[2]+0.5);
				m_selectedVoxel[0]=m_width - m_selectedVoxel[0];

				vector<int> temp;
				temp.push_back(m_selectedVoxel[0]);
				temp.push_back(m_selectedVoxel[1]);
				temp.push_back(m_selectedVoxel[2]);

				if (temp[0]>=0 && temp[0]<=m_width && temp[1]>=0 && temp[1]<=m_height && temp[2]>=0 && temp[2]<=m_depth)
					m_currentMeasureLayer->voxelsVolume.push_back(temp);

				//vector<int> tagedVoxelArray =(m_tagedVoxelArray[m_currentLayer]);

				calcVoronoi();

			} else if (button == GLUT_RIGHT_BUTTON)
			{
				m_currentMeasureLayer->RemovePoint();
			}
		break;
		*/
		case MOVINGBODYMODE:
			m_mouseButton = button;
		break;
		case LBUTTON2WINDOW:
			m_mouseButton = button;
		break;
		}
	} else {
        m_mouseButton = -1;
	}

    m_mouseX = x;
    m_mouseY = y;

	glutPostRedisplay();
}



/**
*/
void OnMouseMove(int x, int y)
{		
    switch (m_mouseButton)
    {
    case GLUT_LEFT_BUTTON:
		switch (GetApplicationMode())
		{
		case LBUTTON2WINDOW:
			m_windowCenter += 0.1f*(x - m_mouseX);
			m_windowWidth += 0.1f*(y - m_mouseY);	

			if (m_windowCenter > MAXIMUM_VALUE)
				m_windowCenter = MAXIMUM_VALUE;
			
			if (m_windowCenter < MINIMUM_VALUE)
				m_windowCenter = MINIMUM_VALUE;

			if (m_windowWidth < 0)
			m_windowWidth = 0;

			InitializeLookUpTable(m_windowCenter, m_windowWidth);
			
			createPreintegrationTable(m_lookUpTable);
			break;
		case MOVINGBODYMODE:
			m_camera->Pitch(0.1f*(y - m_mouseY));
			m_camera->Yaw(0.1f*(x - m_mouseX));
        break;
		}
		break;
	case GLUT_MIDDLE_BUTTON:

        m_camera->MoveSide(0.025*(x - m_mouseX));
        m_camera->MoveUp(0.025f*(m_mouseY - y));
        break;

    case GLUT_RIGHT_BUTTON:
        m_camera->MoveFront(0.01f*(y - m_mouseY));
        break;
    }

	m_mouseX = x;
    m_mouseY = y;

    glutPostRedisplay();
}


void OnPassiveMouseMove(int x, int y)
{
	m_mouseX = x;
    m_mouseY = y;

    glutPostRedisplay();
}
/**
 *
 */

ApplicationModeEnum GetApplicationMode(void)
{
	return m_applicationMode;
}


/**
 *
 */


bool SetApplicationMode(ApplicationModeEnum applicationMode)
{
	switch (applicationMode)
	{
	case SELECTINGVOXEL:
		m_applicationMode = SELECTINGVOXEL;
		break;
	case ADDINGPOINTSMODE:
		//AddMeasureLayer();
		m_applicationMode = ADDINGPOINTSMODE;
		break;
	case MOVINGBODYMODE:
		m_applicationMode = MOVINGBODYMODE;
		break;
	case LBUTTON2WINDOW:
		m_applicationMode = LBUTTON2WINDOW;
		break;
		
	default:
		MarkError();

		return false;
	}
	return true;
}

/**
*/

void setMode( int control )
{
    switch (control)
    {
		
	/*case LOAD_SEGMENTS_ID:
		onLoad(m_fileName);
		break;
	case SAVE_DENSITY_ID:
		onSave(m_fileName,"vasos");
		break;
	case SAVE_SEGMENTS_ID:
		onSave(m_fileName,"separadas");
		break;
	case SAVE_SEGMENTS_RAW_ID:
		onSave(m_fileName, "raw");
		break;
	case SELECT_VOXEL_ID:
		if (!SetApplicationMode(SELECTINGVOXEL))
		{
			MarkError();

			OnDestroy();
		}
		break;		
*/	case MOVING_BODY_ID:
		if (!SetApplicationMode(MOVINGBODYMODE))
		{
			MarkError();

			OnDestroy();
		}
		break;
/*	case ADDING_POINTS_ID:
		if (!SetApplicationMode(ADDINGPOINTSMODE))
		{
			MarkError();

			OnDestroy();
		}
		break;
*/	case REGULAGEM_JANELA_ID:
		if (!SetApplicationMode(LBUTTON2WINDOW))
		{
			MarkError();

			OnDestroy();
		}
		break;
/*	case NEW_LAYER_ID:
		//m_currentLayer++;
		AddMeasureLayer();
		break;
	case LAYER_FORWARD_ID:
		measureLayerForward();
		break;


	case LAYER_BACKWARD_ID:
		measureLayerBackward();
		break;
*/
	case VORONOI_ALPHA_ID:
		createPreintegrationTable(m_lookUpTable);
		break;	

	case VOLUME_BODY_ID:
		if (showBody)
			m_volumeArray->GetVolume(0,m_currentVolume);
		else
			m_volumeArray->GetVolume(2,m_currentVolume);
		m_currentVolume->GetCTexture(m_volumeTexture);
		break;
	case VOLUME_BACKWARD_ID:
		if (m_volumeArray->GetVolume(m_currentID-1,m_currentVolume))
			m_currentID--;
		m_currentVolume->GetCTexture(m_volumeTexture);
		break;
	case VOLUME_FORWARD_ID:
		if (m_volumeArray->GetVolume(m_currentID+1,m_currentVolume))
			m_currentID++;
		m_currentVolume->GetCTexture(m_volumeTexture);
		std::cout << m_currentID << std::endl;
		break;


	case RADIO_ID:
		{
			switch(radioObj){
			case VORONOI_DESLIGADO_ID:
				std::cout << radioObj << std::endl;
				std::cout << m_volumeArray->GetVolume(1,m_voronoiVolume) << std::endl;
				m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);
				break;
			case VORONOI_DRENAGEM_ID:
				std::cout << m_volumeArray->GetVolume(4,m_voronoiVolume) << std::endl;
				//m_voronoiVolumeTexture
				m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);
				break;
			case VORONOI_ESQ_DIR_ID:
				std::cout << m_volumeArray->GetVolume(5,m_voronoiVolume) << std::endl;
				m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);
				break;
			
			case VORONOI_TUMOR_ID:
				std::cout << m_volumeArray->GetVolume(3,m_voronoiVolume) << std::endl;
				m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);
				break;
			}
			
		}
		break;
	}
	glutPostRedisplay();
}

/**
*/

bool OnCreate(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowPosition( 50, 50 );
	glutInitWindowSize(800, 600);

	int main_window = glutCreateWindow( "LiverSegments" );

 	if (glewInit() != GLEW_OK)
	{  glutPostRedisplay();
		MarkError();

		return false;
	}

	m_camera.reset(new CPinholeCamera);

	if (!m_camera.get())
	{
		MarkError();

		return false;
	}

	m_camera->Create(45.0f, 1.0f, 50.0f, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));


	m_camera->ResetViewMatrix();

    m_camera->MoveFront(-2.0f);
    m_camera->ApplyTransform(); 

    glutDisplayFunc(OnPaint);
    GLUI_Master.set_glutKeyboardFunc(OnKeyDown);
    GLUI_Master.set_glutMouseFunc(OnMouse);
    glutMotionFunc(OnMouseMove);
	glutPassiveMotionFunc(OnPassiveMouseMove);
    GLUI_Master.set_glutReshapeFunc(OnSize);
   // glutVisibilityFunc(OnVisible);

  
  GLUI *glui = GLUI_Master.create_glui_subwindow( main_window, 
					    GLUI_SUBWINDOW_RIGHT );
  new GLUI_StaticText( glui, "LiverSegments" );
  new GLUI_Separator( glui );

  GLUI_Panel *obj_panel;
  		obj_panel = new GLUI_Panel(glui, "Modos");
		
		new GLUI_Button( obj_panel, "Move Corpo", MOVING_BODY_ID, setMode);
		new GLUI_Button( obj_panel, "Janela", REGULAGEM_JANELA_ID, setMode);

		//new GLUI_Button( obj_panel, "Seleciona Voxel", SELECT_VOXEL_ID, setMode);

		new GLUI_StaticText( glui, "" );

 /* 		obj_panel = new GLUI_Panel(glui, "Linhas");
		new GLUI_Button( obj_panel, "Adiciona Ponto", ADDING_POINTS_ID, setMode);
		new GLUI_StaticText( obj_panel, "" );
		new GLUI_Button( obj_panel, "Cria nova Linha", NEW_LAYER_ID, setMode);
		new GLUI_StaticText( obj_panel, "" );
		new GLUI_StaticText(obj_panel,"Seleciona Linha:");
		new GLUI_Button( obj_panel, "+", LAYER_FORWARD_ID, setMode);
		new GLUI_Button( obj_panel, "-", LAYER_BACKWARD_ID, setMode);
*/



		new GLUI_StaticText( glui, "" );

		obj_panel = new GLUI_Panel( glui, "Volume" );
		new GLUI_Checkbox( obj_panel, "Malha do Figado", &showLiverMesh);
		new GLUI_Checkbox( obj_panel, "Malha dos Vasos", &showVeinsMesh);
		new GLUI_Checkbox( obj_panel, "Transparencia", &meshAlpha);

		new GLUI_StaticText( glui, "" );

		obj_panel = new GLUI_Panel( glui, "Volume" );
		//new GLUI_StaticText(obj_panel,"Seleciona Volume:");
		new GLUI_Checkbox( obj_panel, "Corpo", &showBody, VOLUME_BODY_ID, setMode);
		new GLUI_StaticText( obj_panel, "" );
	radio = new GLUI_RadioGroup( obj_panel,&radioObj,RADIO_ID,setMode);
		new GLUI_RadioButton( radio, "Sem Segmentacao" );
		new GLUI_RadioButton( radio, "Seg Drenagem" );
		new GLUI_RadioButton( radio, "Seg Esq Dir" );
		new GLUI_RadioButton( radio, "Seg Tumor" );
		new GLUI_Button( obj_panel, "+", VOLUME_FORWARD_ID, setMode);
		new GLUI_Button( obj_panel, "-", VOLUME_BACKWARD_ID, setMode);

		new GLUI_StaticText( glui, "" );

		obj_panel = new GLUI_Panel(glui, "Voronoi");
		GLUI_Spinner *spinner;
		spinner = new GLUI_Spinner( obj_panel, "Canal Alfa:", &m_voronoiAlpha, VORONOI_ALPHA_ID, setMode);
			spinner->set_float_limits( 0, 255 );
/*
		obj_panel = new GLUI_Panel(glui, "Arquivo" );
		new GLUI_EditText( obj_panel, "Nome:", m_fileName, 3 );
		new GLUI_Button( obj_panel, "Salvar Vasos Imgs 2D .xx", SAVE_DENSITY_ID, setMode);
		new GLUI_Button( obj_panel, "Salvar Segm Imgs 2D .xx", SAVE_SEGMENTS_ID, setMode);
		new GLUI_Button( obj_panel, "Salvar Segm Img 3D .RAW", SAVE_SEGMENTS_RAW_ID, setMode);
		new GLUI_Button( obj_panel, "Carregar Segm Img 3D .RAW", LOAD_SEGMENTS_ID, setMode);
*/
		new GLUI_StaticText( glui, "" );
		new GLUI_StaticText( glui, "" );
		new GLUI_StaticText( glui, "" );
		new GLUI_StaticText( glui, "" );
		new GLUI_StaticText( glui, "" );

		new GLUI_Button( glui, "Sair", 0,(GLUI_Update_CB)exit );
 
  glui->set_main_gfx_window( main_window );

  /* We register the idle callback with GLUI, *not* with GLUT */
  GLUI_Master.set_glutIdleFunc( OnIdle );

/*	AddMeasureLayer();
*/
	return true;
}
/*
void onSave(std::string name2save, string tipoDeArquivo){

	int res = m_height*m_width;

	short **imageSegmentada;
	//alloc memoria para imageSegmentada
	imageSegmentada = (short**)malloc(m_depth * sizeof(short*));
	for (int i=0; i < m_depth; i++)
		imageSegmentada[i] = (short*)malloc(sizeof(short) * (res));
	//preenche com algum valor
	for( int k = 0; k < m_depth; k++ ){
		for( int j = 0; j < res; j++ ){
			imageSegmentada[k][j] = 0;
		}
	}

	if (tipoDeArquivo=="vasos"){
	float *volumeAtual = m_currentVolume->GetData();
		for (int k = 0 ; k < m_depth ; k++)
		{
			for (int i = 0 ; i < res ; i++)
			{
					if (volumeAtual[i+(k*res)]>0.51f)
						imageSegmentada[k][i]=255;
			}
		}
	tipoDeArquivo="separadas";
//	tipoDeArquivo="raw";
	}else{

		//atribui valores de acordo com o segmento
		float *volumeVor = m_voronoiVolume->GetData();
		for (int k = 0 ; k < m_depth ; k++)
		{
			for (int i = 0 ; i < m_height ; i++)
			{
				for (int j = 0 ; j < m_width ; j++)
				{
					if (volumeVor[i+j*m_width+(k*m_width*m_height)]>0.05f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.15f )
						imageSegmentada[k][i+j*m_width]=80;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.15f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.25f )
						imageSegmentada[k][i+j*m_width]=40;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.25f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.35f )
						imageSegmentada[k][i+j*m_width]=60;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.35f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.45f )
						imageSegmentada[k][i+j*m_width]=80;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.45f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.55f )
						imageSegmentada[k][i+j*m_width]=100;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.55f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.65f )
						imageSegmentada[k][i+j*m_width]=120;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.65f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.75f )
						imageSegmentada[k][i+j*m_width]=140;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.75f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.85f )
						imageSegmentada[k][i+j*m_width]=160;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.85f && volumeVor[i+j*m_width+(k*m_width*m_height)]<=0.95f )
						imageSegmentada[k][i+j*m_width]=180;
					else if(volumeVor[i+j*m_width+(k*m_width*m_height)]>0.95f)
						imageSegmentada[k][i+j*m_width]=200;
				}
			}
		}
	}

	if (tipoDeArquivo == "raw"){
		FILE* shortFile;
		string saveName = name2save+".raw";
		if( shortFile = fopen( saveName.c_str(), "wb+" ) ){
			for (int k=0 ; k < m_depth ; k++){
				for (int j=0; j<m_width;j++){
					for (int i=0 ; i < m_height ; i++){
						fwrite( &imageSegmentada[k][(m_width*j+i)], 1, sizeof(short), shortFile );					
					}
				}
			}
		}
		fclose(shortFile);
		
	}
	else{
		for (int k=0 ; k < m_depth ; k++){
			FILE* shortFile;
			char formato[5];
			itoa (k, formato,10);
			string saveName = name2save+/*".raw";//+*//*"."+formato;
			if( shortFile = fopen( saveName.c_str(), "wb+" ) ){
				//for (int k=0 ; k < fatias ; k++){
				for (int j=0; j<m_width;j++){
					for (int i=0 ; i < m_height ; i++){
						fwrite( &imageSegmentada[k][(m_width*j+i)], 1, sizeof(short), shortFile );					
					}
				}
			}
			fclose(shortFile);
		}
	}
//	return true;
}
*/
/*
void onLoad(std::string file2load){

	int res = m_height*m_width;

	float *volumeVor = m_voronoiVolume->GetData();
	FILE* shortFile;
	std::string file2loadRaw = file2load+".raw";
	if( shortFile = fopen( file2loadRaw.c_str(), "rb" )  ){
		// read file into image matrix
		for( int k = 0; k < m_depth; k++ ){
			for( int i = 0; i < res; i++ ){
				short value;
				fread( &value, 1, sizeof(short), shortFile );
				if (value == 20)
					volumeVor[i+k*res]=0.1f;
				else if (value == 40)
					volumeVor[i+k*res]=0.2f;
				else if (value == 60)
					volumeVor[i+k*res]=0.3f;
				else if (value == 80)
					volumeVor[i+k*res]=0.4f;
				else if (value == 100)
					volumeVor[i+k*res]=0.5f;
				else if (value == 120)
					volumeVor[i+k*res]=0.6f;
				else if (value == 140)
					volumeVor[i+k*res]=0.7f;
				else if (value == 160)
					volumeVor[i+k*res]=0.8f;
				else if (value == 180)
					volumeVor[i+k*res]=0.9f;
				else if (value == 200)
					volumeVor[i+k*res]=1.0f;
				
			}
		}
		m_voronoiVolume->SetTexture();
		m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);
	}else{
			printf( "Could not open input file %s .\n", file2load.c_str() );
	}

//	return true;
}*/



/**
*/
bool LoadVolume(char* fileName)
{
//init volume array in the first time
	if (!m_volumeArray.get())
	{
		m_volumeArray.reset(new VolumeArray);

		if (!m_volumeArray.get())
		{
			MarkError();

			return false;
		}
	}

	m_currentVolume.reset(new Volume);

	if (!m_currentVolume.get())
	{
		MarkError();

		return false;
	}
	if (!fileName==NULL)
	{
		if (!m_currentVolume->LoadImage(fileName))
		{
			MarkError();

			OnDestroy();
		}
	}

	if (!m_currentVolume->CreateVolume(m_width, m_height, m_depth))
	{
		MarkError();

		OnDestroy();
	}

	m_currentVolume->GetCTexture(m_volumeTexture);

	m_volumeArray->AddVolume(m_currentVolume);

	return true;

}

bool LoadVoronoiVolume(char* fileName)
{

	m_voronoiVolume.reset(new Volume);

	if (!m_voronoiVolume.get())
	{
		MarkError();

		return false;
	}
	if (!fileName==NULL)
	{
		if (!m_voronoiVolume->LoadImage(fileName))
		{
			MarkError();

			OnDestroy();
		}
	}

	if (!m_voronoiVolume->CreateVolume(m_width, m_height, m_depth))
	{
		MarkError();

		OnDestroy();
	}

	m_voronoiVolume->GetCTexture(m_voronoiVolumeTexture);



	return true;

}


/**
*/
int main(int argc, char **argv)
{
	m_volumeDeUmVoxel=((33.0/512.0)*(33.0/512.0)*((169.0/43.0)*1.3)*(512.0/128.0)*(512.0/128.0)*(43.0/64.0));
	if (argc != 11)
	{
		cout << endl;
		cout << "SYPNOSIS" << endl;
		cout << "Visualizador de Volumes [programa] [width] [height] [depth] (volume original) (figado segmentado) (tumor segmentado) (segmentos de drenagem) (segmentos esq e dir) [centro da janela] [largura da janela]" << endl;
		cout << endl;

		OnDestroy();
	}

    if (!OnCreate(argc, argv))
	{
		MarkError();

		OnDestroy();
	}
    
	m_width = atoi(argv[1]);
	m_height = atoi(argv[2]);
	m_depth = atoi(argv[3]);

	m_windowCenter = atoi(argv[9]);
	m_windowWidth = atoi(argv[10]);

	if (!LoadVolume(argv[4]))
	{
		MarkError();

		return false;
	}

	if (!LoadVolume(argv[5]))
	{
		MarkError();

		return false;
	}

	if (!LoadVolume())
	{
		MarkError();

		return false;
	}

	if (!LoadVolume(argv[6]))
	{
		MarkError();

		return false;
	}

	if (!LoadVolume(argv[7]))
	{
		MarkError();

		return false;
	}

	if (!LoadVolume(argv[8]))
	{
		MarkError();

		return false;
	}

	if (!LoadVoronoiVolume())
	{
		MarkError();

		return false;
	}


	//*	*******************************************
	
		fromFile=ObjLoader::ReadMesh("modificado_0_7_red_0_5_ver_2.3ds");
		//textureMagic();
		fromFile->initNeighbors();
		fromFile->calcNormals();
		fromFile2=ObjLoader::ReadMesh("multislice_figado.3ds");
		//textureMagic();
		fromFile2->initNeighbors();
		fromFile2->calcNormals();

		setupRC();
	
//	********************************************/


	volumeFigado = m_volumeArray->RecortaVolume(0.0008f);

	InitializeLookUpTable(m_windowCenter, m_windowWidth);

	createPreintegrationTable(m_lookUpTable);

	if (!InitializeShaders())
	{
		MarkError();

		OnDestroy();
	}

	m_volumeArray->GetVolume(2,m_currentVolume);
	m_currentVolume->GetCTexture(m_volumeTexture);

    glutMainLoop();
   

	//********************************************
	
		delete fromFile;
		delete fromFile2;
	
//	********************************************/


    return EXIT_SUCCESS;
}