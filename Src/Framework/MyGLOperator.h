#pragma once

#include <MathTypes.hpp>

// #include <gl/glew.h>
#include <QGLWidget>
#include <gl/GLU.h>

class IWorldMapping;

class MyGLOperator
{

public:
	MyGLOperator(IWorldMapping* pMapping);
	~MyGLOperator();

	//poly tess
	GLUtesselator *m_pGlTessObj;
	IWorldMapping* _pMapping;

	short m_sPolyStyle=0;

public://Callback
	static void CALLBACK errorCallback(GLenum errorCode);
	static void CALLBACK combineCallback(GLdouble coords[3], GLdouble *data[4], GLfloat weight[4], GLdouble **dataOut);
public:// drawing
	//20121021
	void DrawRect2D(const IPoint2& pt1, const IPoint2& pt2);
	void DrawPoint2D(const IPoint2& p);
	//20121022
	void DrawRect3D(const DPoint3& pt1, const DPoint3& pt2);
	void DrawPoint(const DPoint3& p);

	void DrawLine(const DPoint3& p1, const DPoint3& p2);
	void DrawLine3D(const DPoint3& p1, const DPoint3& p2);
	void DrawLine(const std::vector<DPoint3> &dline, bool bClose);		//draw a line
	void DrawLine3D(const std::vector<DPoint3> &dline, bool bClose);		//draw a line
	void DrawLine2D(const IPoint2& p1, const IPoint2& p2);
	//draw a polygon
	void DrawPolygon(const std::vector<std::vector<DPoint3>> &poly);
	//draw a list of polygon
	void DrawPolygons(const std::vector<std::vector<std::vector<DPoint3>>> &polys);
	void DrawRectangle(const DPoint3& p1, const DPoint3& p2);
	void DrawRectangle(const IPoint2& p1, const IPoint2& p2);
	void DrawRectangleO(const IPoint2& p1, const IPoint2& p2);
	// 	void DrawRectangleOTextAsW(const IPoint2& p1, const IPoint2& p2,std::string& str);

	void DrawCircle(DPoint3 center, double radius);
	void DrawImage(DPoint3& ptWS, DPoint3& ptEN, GLsizei imgWidth, GLsizei imgHeight, unsigned char* pData);


	void SetPolygonStyle(short style);

public://Set Status

	void SetLineWidth(const float wt);
	void SetPointSize(const float ps);
	void SetColor(const RGBAf& col);
	void EnableBlend(bool b);

};

