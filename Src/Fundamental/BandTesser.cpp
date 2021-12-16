
#include"BandTesser.h"
#include <QDebug>


// for tess

/// 初始化用户自定义的程序变量

/** 环绕规则 */
GLdouble TessProperty[5] =
{
	GLU_TESS_WINDING_ODD,        /**< 环绕数为奇数 */
	GLU_TESS_WINDING_NONZERO,    /**< 环绕数为非0 */
	GLU_TESS_WINDING_POSITIVE,   /**< 环绕数为正数 */
	GLU_TESS_WINDING_NEGATIVE,   /**< 环绕数为负数 */
	GLU_TESS_WINDING_ABS_GEQ_TWO /**< 环绕数绝对值大于等于2 */
};
GLuint nProperty = 0;              /**< 环绕规则索引 */

/** gluTessCallback注册的回调函数 */
void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *string;
	///输出错误信息
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
	/** 随即产生颜色值 */
	red = (GLubyte)rand() & 0xff;
	green = (GLubyte)rand() & 0xff;
	blue = (GLubyte)rand() & 0xff;
	glColor3ub(red, green, blue);
	glVertex3dv(pt);

}

/** 用于处理检测轮廓线交点，并决定是否合并顶点，
新创建的顶点最多可以是4个已有顶点线性组合，这些定点坐标存储在data中
其中weight为权重，weight[i]的总合为1.0
*/
void CALLBACK combineCallback(GLdouble coords[3],
	GLdouble *vertex_data[4],
	GLfloat weight[4],
	GLdouble **dataOut)
{
	GLdouble *vertex;
	int i;
	/** 分配存储新顶点的内存 */
	vertex = (GLdouble *)malloc(6 * sizeof(GLdouble));

	/** 存储坐标值 */
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	/** 通过插值计算RGB颜色值 */
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
	this function is modified from ‘tessSegmentation’, build uncertainty area only
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
						// 这里正反方向貌似没有影响呢20200629
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



