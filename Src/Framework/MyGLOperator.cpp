#include "MyGLOperator.h"

#include"MyGLWidgetBase.h"

#define POLYGON_STYLE_FILL		1


MyGLOperator::MyGLOperator(IWorldMapping* pMapping) :_pMapping(pMapping)
{
	//polygon tess
	m_pGlTessObj = gluNewTess();
	gluTessCallback(m_pGlTessObj, GLU_TESS_BEGIN, (void (CALLBACK *) ())glBegin);
	gluTessCallback(m_pGlTessObj, GLU_TESS_VERTEX, (void (CALLBACK *) ()) &glVertex3dv);
	gluTessCallback(m_pGlTessObj, GLU_TESS_END, (void (CALLBACK *) ())glEnd);
	gluTessCallback(m_pGlTessObj, GLU_TESS_ERROR, (void (CALLBACK *) ())errorCallback);
	gluTessCallback(m_pGlTessObj, GLU_TESS_COMBINE, (void (CALLBACK *) ())combineCallback);
	gluTessProperty(m_pGlTessObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);


}

MyGLOperator::~MyGLOperator()
{

	gluDeleteTess(m_pGlTessObj);

}

void MyGLOperator::DrawImage(DPoint3& ptWS, DPoint3& ptEN, GLsizei imgWidth, GLsizei imgHeight, unsigned char* pData)
{
	GLuint texture = 0;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_POLYGON);
	//	glColor4f(0.5,0.5,0.5,1.0);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(ptEN.x, ptEN.y);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(ptWS.x, ptEN.y);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(ptWS.x, ptWS.y);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(ptEN.x, ptWS.y);

	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDeleteTextures(1, &texture);

	glDisable(GL_BLEND);
}
void MyGLOperator::DrawPoint(const DPoint3& p)
{
	glBegin(GL_POINTS);
	glVertex3d(p.x, p.y, p.z);
	glEnd();
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::DrawLine(const DPoint3& p1, const DPoint3& p2)
{
	glBegin(GL_LINE_STRIP);
	glVertex3d(p1.x, p1.y, 0);
	glVertex3d(p2.x, p2.y, 0);
	glEnd();
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::DrawLine3D(const DPoint3& p1, const DPoint3& p2)
{
	glBegin(GL_LINE_STRIP);
	glVertex3d(p1.x, p1.y, p1.z);
	glVertex3d(p2.x, p2.y, p2.z);
	glEnd();
}//----------------------------------------------------------------------------------------------------

void MyGLOperator::DrawLine(const std::vector<DPoint3> &dline, bool bClose)
{
	size_t size = dline.size();
	if (size < 2) return;
	bClose ? glBegin(GL_LINE_LOOP) : glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < size; i++)
	{
		glVertex3d(dline[i].x, dline[i].y, 0);
	}
	glEnd();
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::DrawLine3D(const std::vector<DPoint3> &dline, bool bClose)
{
	size_t size = dline.size();
	if (size < 2) return;
	bClose ? glBegin(GL_LINE_LOOP) : glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < size; i++)
	{
		glVertex3d(dline[i].x, dline[i].y, dline[i].z);
	}
	glEnd();
}//----------------------------------------------------------------------------------------------------

void MyGLOperator::DrawLine2D(const IPoint2& pt1, const IPoint2& pt2)
{
	DPoint3 p1, p2;
	_pMapping->ScreenToWorld(pt1, p1);
	_pMapping->ScreenToWorld(pt2, p2);
	glBegin(GL_LINE_STRIP);
	glVertex3d(p1.x, p1.y, 0);
	glVertex3d(p2.x, p2.y, 0);
	glEnd();
}

void MyGLOperator::DrawRectangle(const DPoint3& p1, const DPoint3& p2)
{
	glBegin(GL_LINE_LOOP);
	glVertex3d(p1.x, p1.y, 0);
	glVertex3d(p1.x, p2.y, 0);
	glVertex3d(p2.x, p2.y, 0);
	glVertex3d(p2.x, p1.y, 0);
	glEnd();
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::DrawRectangle(const IPoint2& pt1, const IPoint2& pt2)
{
	DPoint3 p1, p2;
	_pMapping->ScreenToWorld(pt1, p1);
	_pMapping->ScreenToWorld(pt2, p2);
	glBegin(GL_LINE_LOOP);
	glVertex3d(p1.x, p1.y, 0);
	glVertex3d(p1.x, p2.y, 0);
	glVertex3d(p2.x, p2.y, 0);
	glVertex3d(p2.x, p1.y, 0);
	glEnd();
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::DrawRectangleO(const IPoint2& pt1, const IPoint2& pt2)
{
	DPoint3 p1, p2, p3, p4, p5, p6, p7, p8;
	_pMapping->ScreenToWorld(pt1 + IPoint2(8, 0), p1);
	_pMapping->ScreenToWorld(pt1 + IPoint2(4, 2), p2);
	_pMapping->ScreenToWorld(pt1 + IPoint2(2, 4), p3);
	_pMapping->ScreenToWorld(pt1 + IPoint2(0, 8), p4);

	_pMapping->ScreenToWorld(pt2 + IPoint2(-8, 0), p5);
	_pMapping->ScreenToWorld(pt2 + IPoint2(-4, -2), p6);
	_pMapping->ScreenToWorld(pt2 + IPoint2(-2, -4), p7);
	_pMapping->ScreenToWorld(pt2 + IPoint2(0, -8), p8);

	glBegin(GL_LINE_LOOP);
	glVertex3d(p1.x, p1.y, 0);
	glVertex3d(p2.x, p2.y, 0);
	glVertex3d(p3.x, p3.y, 0);
	glVertex3d(p4.x, p4.y, 0);
	glVertex3d(p4.x, p8.y, 0);
	glVertex3d(p3.x, p7.y, 0);
	glVertex3d(p2.x, p6.y, 0);
	glVertex3d(p1.x, p5.y, 0);
	glVertex3d(p5.x, p5.y, 0);
	glVertex3d(p6.x, p6.y, 0);
	glVertex3d(p7.x, p7.y, 0);
	glVertex3d(p8.x, p8.y, 0);

	glVertex3d(p8.x, p4.y, 0);
	glVertex3d(p7.x, p3.y, 0);
	glVertex3d(p6.x, p2.y, 0);
	glVertex3d(p5.x, p1.y, 0);
	glEnd();

}//----------------------------------------------------------------------------------------------------

void MyGLOperator::DrawPolygon(const std::vector<std::vector<DPoint3>> &poly)
{
	//size of contours
	size_t contours = poly.size();
	if (m_sPolyStyle == POLYGON_STYLE_FILL)
	{
		size_t p = 0;
		static GLdouble v[32000][3];
		//only one polygon, but multiple contours
		gluTessBeginPolygon(m_pGlTessObj, NULL);
		for (size_t i = 0; i < contours; i++)
		{
			size_t size = poly[i].size();
			gluTessBeginContour(m_pGlTessObj);

			for (size_t k = 0; k < size; k++)
			{
				v[p][0] = poly[i][k].x;
				v[p][1] = poly[i][k].y;
				v[p][2] = poly[i][k].z;
				gluTessVertex(m_pGlTessObj, v[p], v[p]);
				p++;
			}
			gluTessEndContour(m_pGlTessObj);
		}
		gluTessEndPolygon(m_pGlTessObj);
		//glPopAttrib();
	}
	else
	{
		// just draw each ring
		for (int i = 0; i < contours; i++) DrawLine(poly[i], true);
	}
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::DrawPolygons(const std::vector<std::vector<std::vector<DPoint3>>> &polys)
{
	//size of contours
	size_t nPolys = polys.size();
	if (m_sPolyStyle == POLYGON_STYLE_FILL)
	{
		size_t p = 0;
		static GLdouble v[32000][3];
		for (int i = 0; i < nPolys; i++)
		{
			size_t contours = polys[i].size();
			//only one polygon, but multiple contours
			gluTessBeginPolygon(m_pGlTessObj, NULL);
			for (size_t j = 0; j < contours; j++)
			{
				size_t size = polys[i][j].size();
				gluTessBeginContour(m_pGlTessObj);

				for (size_t k = 0; k < size; k++)
				{
					v[p][0] = polys[i][j][k].x;
					v[p][1] = polys[i][j][k].y;
					v[p][2] = 0.0;
					gluTessVertex(m_pGlTessObj, v[p], v[p]);
					p++;
				}
				gluTessEndContour(m_pGlTessObj);
			}
			gluTessEndPolygon(m_pGlTessObj);
		}
	}
	else
	{
		// just draw each ring
		for (int i = 0; i < nPolys; i++)
		{
			size_t contours = polys[i].size();
			for (int j = 0; j < contours; j++)
			{
				DrawLine(polys[i][j], true);
			}
		}
	}
}//----------------------------------------------------------------------------------------------------
//20121017
void MyGLOperator::DrawCircle(DPoint3 center, double radius)
{
	std::vector<DPoint3> circle;
	double pi = 3.14;
	for (int i = 0; i < 360; i++)
	{
		DPoint3 pt;
		pt.x = center.x + radius * sin((double)i*3.14 / 180);
		pt.y = center.y + radius * cos((double)i*3.14 / 180);
		circle.push_back(pt);
	}
	DrawLine(circle, true);
}
void MyGLOperator::DrawRect2D(const IPoint2& pt1, const IPoint2& pt2)
{
	DPoint3 p1, p2;
	_pMapping->ScreenToWorld(pt1, p1);
	_pMapping->ScreenToWorld(pt2, p2);
	glRectf(p1.x, p1.y, p2.x, p2.y);
}
void MyGLOperator::DrawPoint2D(const IPoint2& p)
{
	DPoint3 pt;
	_pMapping->ScreenToWorld(p, pt);
	DrawPoint(pt);
}
void MyGLOperator::DrawRect3D(const DPoint3& pt1, const DPoint3& pt2)
{
	glRectf(pt1.x, pt1.y, pt2.x, pt2.y);
}

void MyGLOperator::SetPolygonStyle(short style)
{
	m_sPolyStyle = style;
}


void CALLBACK MyGLOperator::errorCallback(GLenum errorCode)
{
	const GLubyte *estring;

	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}//----------------------------------------------------------------------------------------------------
void CALLBACK MyGLOperator::combineCallback(GLdouble coords[3], GLdouble *data[4], GLfloat weight[4], GLdouble **dataOut)
{
	GLdouble *vertex;
	vertex = (GLdouble *)malloc(3 * sizeof(GLdouble));

	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	*dataOut = vertex;
}//----------------------------------------------------------------------------------------------------

void MyGLOperator::SetLineWidth(const float wt)
{
	glLineWidth(wt);
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::SetPointSize(const float ps)
{
	glPointSize(ps);
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::SetColor(const RGBAf& col)
{
	// 	m_currentColor=col;
	glColor4f(col.r, col.g, col.b, col.a);
}//----------------------------------------------------------------------------------------------------
void MyGLOperator::EnableBlend(bool b)
{
	if (b)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA
		return;
	}
	glDisable(GL_BLEND);
}//----------------------------------------------------------------------------------------------------

