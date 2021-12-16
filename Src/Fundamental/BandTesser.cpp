
#include"BandTesser.h"
#include <QDebug>


// for tess

/// ��ʼ���û��Զ���ĳ������

/** ���ƹ��� */
GLdouble TessProperty[5] =
{
	GLU_TESS_WINDING_ODD,        /**< ������Ϊ���� */
	GLU_TESS_WINDING_NONZERO,    /**< ������Ϊ��0 */
	GLU_TESS_WINDING_POSITIVE,   /**< ������Ϊ���� */
	GLU_TESS_WINDING_NEGATIVE,   /**< ������Ϊ���� */
	GLU_TESS_WINDING_ABS_GEQ_TWO /**< ����������ֵ���ڵ���2 */
};
GLuint nProperty = 0;              /**< ���ƹ������� */

/** gluTessCallbackע��Ļص����� */
void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *string;
	///���������Ϣ
	string = gluErrorString(errorCode);
	// 	fprintf(stderr, "Tessellation Error: %s\n", string);
	// 	exit(0);
}

void CALLBACK endCallback(void)
{
	glEnd();
}

void CALLBACK vertexCallback(GLvoid *vertex)
{

	GLdouble* pt;
	GLubyte red, green, blue;
	pt = (GLdouble*)vertex;
	/** �漴������ɫֵ */
	red = (GLubyte)rand() & 0xff;
	green = (GLubyte)rand() & 0xff;
	blue = (GLubyte)rand() & 0xff;
	glColor3ub(red, green, blue);
	glVertex3dv(pt);

}

/** ���ڴ����������߽��㣬�������Ƿ�ϲ����㣬
�´����Ķ�����������4�����ж���������ϣ���Щ��������洢��data��
����weightΪȨ�أ�weight[i]���ܺ�Ϊ1.0
*/
void CALLBACK combineCallback(GLdouble coords[3],
	GLdouble *vertex_data[4],
	GLfloat weight[4],
	GLdouble **dataOut)
{
	GLdouble *vertex;
	int i;
	/** ����洢�¶�����ڴ� */
	vertex = (GLdouble *)malloc(6 * sizeof(GLdouble));

	/** �洢����ֵ */
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	/** ͨ����ֵ����RGB��ɫֵ */
	for (i = 3; i < 6; i++)
		vertex[i] = weight[0] * vertex_data[0][i] +
		weight[1] * vertex_data[1][i] +
		weight[2] * vertex_data[2][i] +
		weight[3] * vertex_data[3][i];
	*dataOut = vertex;
}

// ~for tess








GLUtesselator *BandTesser::_tobj = NULL;


void BandTesser::tessSegmentation(GLuint gllist, QList<UnCertaintyArea*> areas) {
	// create buffer for the uncertainty areas	
	int nAreaLen = areas.size();										// how many area
	GLdouble**** contourBuffer = new GLdouble***[areas.size()];
	// for each area
	for (int i = 0; i < nAreaLen; i++)
	{
		int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;		// how many contour
		contourBuffer[i] = new GLdouble**[nContourLen];
		// first contour
		int nLen = areas[i]->_contour._listPt.size();
		contourBuffer[i][0] = new GLdouble*[nLen];
		for (int j = 0; j < nLen; j++)
		{
			contourBuffer[i][0][j] = new GLdouble[3];
			contourBuffer[i][0][j][0] = areas[i]->_contour._listPt[j].x();
			contourBuffer[i][0][j][1] = areas[i]->_contour._listPt[j].y();
			contourBuffer[i][0][j][2] = 0;
		}
		// other contours
		for (int k = 0; k < nContourLen - 1; k++)
		{
			int nLen = areas[i]->_listEmbeddedArea[k]->_contour._listPt.size();
			contourBuffer[i][k + 1] = new GLdouble*[nLen];
			for (int j = 0; j < nLen; j++)
			{
				contourBuffer[i][k + 1][j] = new GLdouble[3];
				contourBuffer[i][k + 1][j][0] = areas[i]->_listEmbeddedArea[k]->_contour._listPt[j].x();
				contourBuffer[i][k + 1][j][1] = areas[i]->_listEmbeddedArea[k]->_contour._listPt[j].y();
				contourBuffer[i][k + 1][j][2] = 0;
			}
		}
	}
	/*
		state==-1: negative
		state==0:  uncertain
		state==1:  positive
	*/
	for (int state = -1; state < 2; state++)
	{
		glNewList(gllist + 1 + state, GL_COMPILE);
		gluTessProperty(_tobj, GLU_TESS_WINDING_RULE, TessProperty[nProperty]);
		gluTessBeginPolygon(_tobj, NULL);
		for (int i = 0; i < nAreaLen; i++)
		{
			// 		if (i != 1) continue;
			if (areas[i]->_nState == state)
			{
				int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;
				for (int k = 0; k < nContourLen; k++)
				{
					int nLen;
					if (k == 0) nLen = areas[i]->_contour._listPt.size();
					else nLen = areas[i]->_listEmbeddedArea[k - 1]->_contour._listPt.size();
					gluTessBeginContour(_tobj);
					for (int j = 0; j < nLen; j++)
					{
						gluTessVertex(_tobj, contourBuffer[i][k][j], contourBuffer[i][k][j]);
					}
					gluTessEndContour(_tobj);
				}
			}
		}
		gluTessEndPolygon(_tobj);
		glEndList();
	}

	for (int i = 0; i < nAreaLen; i++)
	{
		int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;		// how many contour
		// first contour
		int nLen = areas[i]->_contour._listPt.size();
		for (int j = 0; j < nLen; j++)
		{
			delete contourBuffer[i][0][j];
		}
		for (int k = 0; k < nContourLen - 1; k++)
		{
			int nLen = areas[i]->_listEmbeddedArea[k]->_contour._listPt.size();
			for (int j = 0; j < nLen; j++)
			{
				delete[]contourBuffer[i][k + 1][j];
			}
			delete[]contourBuffer[i][k + 1];
		}
		delete[]contourBuffer[i];
	}

	delete[]contourBuffer;
}

/*
	tessellation the area segmentation, generate uncertain area only
	this function is modified from ��tessSegmentation��, build uncertainty area only
*/
void BandTesser::tessSegmentation_uncertainOnly(GLuint gllist, QList<UnCertaintyArea*> areas) {
	// 1.allocate resource
	int nAreaLen = areas.size();										// how many area
	GLdouble**** contourBuffer = new GLdouble***[nAreaLen];	
	//contourBuffer[index of region][index of contour][index of point][x,y,z]
	
	for (int i = 0; i < nAreaLen; i++){		// for each area
		int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;		// how many contour
		contourBuffer[i] = new GLdouble**[nContourLen];
		// first contour
		int nLen = areas[i]->_contour._listPt.size();
		contourBuffer[i][0] = new GLdouble*[nLen];
		for (int j = 0; j < nLen; j++)
		{
			contourBuffer[i][0][j] = new GLdouble[3];
			contourBuffer[i][0][j][0] = areas[i]->_contour._listPt[j].x();
			contourBuffer[i][0][j][1] = areas[i]->_contour._listPt[j].y();
			contourBuffer[i][0][j][2] = 0;
		}
		// other contours
		for (int k = 0; k < nContourLen - 1; k++)
		{
			int nLen = areas[i]->_listEmbeddedArea[k]->_contour._listPt.size();
			contourBuffer[i][k + 1] = new GLdouble*[nLen];
			for (int j = 0; j < nLen; j++)
			{
				contourBuffer[i][k + 1][j] = new GLdouble[3];
				contourBuffer[i][k + 1][j][0] = areas[i]->_listEmbeddedArea[k]->_contour._listPt[j].x();
				contourBuffer[i][k + 1][j][1] = areas[i]->_listEmbeddedArea[k]->_contour._listPt[j].y();
				contourBuffer[i][k + 1][j][2] = 0;
			}
		}
	}
	// 2.tess

	// uncertain area only, state==0;
	{
		int nState = 0;
		glNewList(gllist, GL_COMPILE);
		gluTessProperty(_tobj, GLU_TESS_WINDING_RULE, TessProperty[nProperty]);
		gluTessBeginPolygon(_tobj, NULL);
		for (int i = 0; i < nAreaLen; i++)
		{
			// 		if (i != 1) continue;
			if (areas[i]->_nState == nState)
			{

				int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;
				for (int k = 0; k < nContourLen; k++)
				{
					if (k == 0) {
						int nLen = areas[i]->_contour._listPt.size();
						gluTessBeginContour(_tobj);
						for (int j = 0; j < nLen; j++)
						{
							gluTessVertex(_tobj, contourBuffer[i][k][j], contourBuffer[i][k][j]);
						}

						gluTessEndContour(_tobj);
					}
					else {
						int nLen = areas[i]->_listEmbeddedArea[k - 1]->_contour._listPt.size();
						gluTessBeginContour(_tobj);
						// ������������ò��û��Ӱ����20200629
						for (int j = 0; j < nLen; j++)
						//for (int j = nLen-1; j >=0 ; j--)
						{
							gluTessVertex(_tobj, contourBuffer[i][k][j], contourBuffer[i][k][j]);
						}
						gluTessEndContour(_tobj);
					}
				}
			}
		}
		gluTessEndPolygon(_tobj);
		glEndList();
	}

	// 3.release resource
	for (int i = 0; i < nAreaLen; i++)
	{
		int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;		// how many contour
		// first contour
		int nLen = areas[i]->_contour._listPt.size();
		for (int j = 0; j < nLen; j++)
		{
			delete contourBuffer[i][0][j];
		}
		for (int k = 0; k < nContourLen - 1; k++)
		{
			int nLen = areas[i]->_listEmbeddedArea[k]->_contour._listPt.size();
			for (int j = 0; j < nLen; j++)
			{
				delete[]contourBuffer[i][k + 1][j];
			}
			delete[]contourBuffer[i][k + 1];
		}
		delete[]contourBuffer[i];
	}

	delete[]contourBuffer;
}

BandTesser::BandTesser() {
	
}

BandTesser::~BandTesser() {
	
}

// build the tess for uncertainty regions
void BandTesser::BuildTess() {

	_tobj = gluNewTess();
	gluTessCallback(_tobj, GLU_TESS_VERTEX,
		(void(__stdcall*)())glVertex3dv);
	gluTessCallback(_tobj, GLU_TESS_BEGIN,
		(void(__stdcall*)())beginCallback);
	gluTessCallback(_tobj, GLU_TESS_END,
		(void(__stdcall*)())endCallback);
	gluTessCallback(_tobj, GLU_TESS_ERROR,
		(void(__stdcall*)())errorCallback);

}

void BandTesser::Release() {
	if (_tobj)
		gluDeleteTess(_tobj);
}

void BandTesser::Tess(QList<UnCertaintyArea*> areas) {
	_gllist = glGenLists(1);
	tessSegmentation_uncertainOnly(_gllist, areas);
}

void BandTesser::Draw() {
	glCallList(_gllist);
}



