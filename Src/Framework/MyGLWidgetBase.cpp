#include "MyGLWidgetBase.h"
//#include "zcalcfuncs.h"
#include <sstream>

#include <math.h>
#include <algorithm>

#include <QWheelEvent>

#include <assert.h>
#include <fstream>
#include <QDebug>

#include"MyGLOperator.h"

using namespace std;

MyGLWidgetBase::MyGLWidgetBase(QWidget *parent)
	: QGLWidget(parent)
	, m_bLBtnDown(false)
	, m_bRBtnDown(false)
	, m_bMBtnDown(false)
	, m_pt3Eye(0.0, 0.0, 4.0)
{
	_pGLOperator = new MyGLOperator(this);
	onCreate();
	_pGLOperator->SetPointSize(2.0f);
	_pGLOperator->SetLineWidth(1.0f);


	m_dbFovy = 45.0;
	m_dbZNear = 0.0001;
	m_dbZFar = 1500.0;
	_nHeight = _nWidth = 1;


}

MyGLWidgetBase::~MyGLWidgetBase()
{

	delete _pGLOperator;

}

void MyGLWidgetBase::initializeGL()
{
// 	glewInit();
	// build texture
// 	char* fileName[] = { "resources\\1.png"
// 		, "resources\\2.png"
// 		, "resources\\3.png"
// 		, "resources\\4.png"
// 		, "resources\\5.png"
// 		, "resources\\6.png"
// 	};
// 	for (int i = 0; i < 6; i++)
// 	{
// 		QImage image(fileName[i]);
// 		texWidth = image.width();
// 		texHeight = image.height();
// 		// 	makeCheckImage();
// 		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 
// 		glGenTextures(1, texName + i);
// 		glBindTexture(GL_TEXTURE_2D, texName[i]);
// 
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
// 			0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
// 		glGenerateMipmap(GL_TEXTURE_2D);
// 	}
	// ~build texture
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// used for border
	glLineWidth(2.0);

	// enable blending
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth(1.5);
	init();
}

void MyGLWidgetBase::paintGL(){
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	// 1.��������
// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();
// 	glTranslatef(0.0, 0.0, -10.0);
// 	// 2.����
// 	glScalef(_fScale, _fScale, _fScale);
// 	// 3.��ת
// 	glRotatef(_nRotateX, 1, 0, 0);
// 	glRotatef(_nRotateY, 0, 1, 0);
// 	glRotatef(_nRotateZ, 0, 0, 1);
// 
// 	// 4.���������
// 	glDisable(GL_TEXTURE_2D);
// 	// axis
// 	glColor3f(1, 0, 0);
// 	glBegin(GL_LINES);
// 	glVertex3f(0, 0, 0);
// 	glVertex3f(10000, 0, 0);
// 	glEnd();
// 	glColor3f(0, 1, 0);
// 	glBegin(GL_LINES);
// 	glVertex3f(0, 0, 0);
// 	glVertex3f(0, 10000, 0);
// 	glEnd();
// 	glColor3f(0, 0, 1);
// 	glBegin(GL_LINES);
// 	glVertex3f(0, 0, 0);
// 	glVertex3f(0, 0, 10000);
// 	glEnd();
// 	// 5.�Ƶ�����
// 	glTranslatef(-1.5, -1.5, -1.5);
// 	// 6.ħ������
// 	glEnable(GL_TEXTURE_2D);
// 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
// 	drawMagicCube();

	Draw();


}

void MyGLWidgetBase::resizeGL(int width, int height){
	// 	_nWidth = width;
	// 	_nHeight = height;
// 	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	gluLookAt(0, 0, 1.0, 0, 0, 0, 0, 1, 0);
// 	gluPerspective(60.0, (GLfloat)width / (GLfloat)height, .1, 30.0);
	// 	glMatrixMode(GL_MODELVIEW);
	// 	glLoadIdentity();
	// 	glTranslatef(0.0, 0.0, -3.6);
	OnSize(width, height);
}

void MyGLWidgetBase::timerEvent(QTimerEvent* event)
{
	// 	if (_bRotate)
	// 	{
	// 		_nRotate = (_nRotate + 10) % 360;
	// 	}
	updateGL();
}

void MyGLWidgetBase::mousePressEvent(QMouseEvent * event)
{
	switch (event->button())
	{
	case Qt::MidButton:
		break;
	case Qt::LeftButton:
		OnLButtonDown(event->pos().x(), event->pos().y());
		break;
	case Qt::RightButton:
		OnRButtonDown(event->pos().x(), event->pos().y());
		break;
	default:
		break;
	}
	updateGL();
}

void MyGLWidgetBase::mouseReleaseEvent(QMouseEvent * event)
{
	OnLButtonUp(event->pos().x(), event->pos().y());
	updateGL();

}

void MyGLWidgetBase::mouseMoveEvent(QMouseEvent * event)
{
	OnMouseMove(event->pos().x(), event->pos().y());
	updateGL();
}

void MyGLWidgetBase::wheelEvent(QWheelEvent * event)
{
	// 	int n = event->delta();
	// 	int b = 0;
// 	if (event->delta() > 0)
// 	{
// 		_fScale *= 1.1;
// 	}
// 	else
// 	{
// 		_fScale /= 1.1;
// 	}
	OnMouseWheel(event->delta());
	updateGL();
}

void MyGLWidgetBase::keyPressEvent(QKeyEvent * event)
{
// 	switch (event->key())
// 	{
// 	case 'x':
// 		_nRotateX = (_nRotateX + 10) % 360;
// 
// 		break;
// 	case 'y':
// 		_nRotateY = (_nRotateY + 10) % 360;
// 		break;
// 	case 'z':
// 		_nRotateZ = (_nRotateZ + 10) % 360;
// 		break;
// 	default:
// 		break;
// 	}
	updateGL();
}


const double SQRT_3 = 1.732;
const double arrX[6] = { 2,1,-1,-2,-1,1 };
const double arrY[6] = { 0,SQRT_3 ,SQRT_3 ,0,-SQRT_3 ,-SQRT_3 };

void MyGLWidgetBase::Draw()
{
	_pGLOperator->SetColor(RGBAf(0, 1, 1, 1));
	onPreRenderScene();
	onTransform();
	//SetPolygonStyle(1);		//fill
	// ��������ϵ
	//drawCoords();


	// a circle
	if(false)
	{
		double dbRadius = .019;
		double dbRadiusII = (dbRadius + .001)*SQRT_3;
		glBegin(GL_POLYGON);
		for (size_t i = 0; i < 6; i++)
		{
			glVertex2f(dbRadius*arrX[i], dbRadius*arrY[i]);
		}
		glEnd();
		for (size_t j = 0; j < 6; j++)
		{
			double dbX = arrY[j] * dbRadiusII;
			double dbY = arrX[j] * dbRadiusII;
			glBegin(GL_POLYGON);
			for (size_t i = 0; i < 6; i++)
			{
				glVertex2f(dbX + dbRadius*arrX[i], dbY + dbRadius*arrY[i]);
			}
			glEnd();

		}
	}

	// �������������
	if(false)
	{
		double dbRadius = .017;
		double dbRadiusII = .02;
		int nWidth = 20;
		int nHeight = 20;
		for (size_t i = 0; i < nHeight; i++)
		{
			for (size_t j = 0; j < nWidth; j++) {
				double dbX = j * 3 * dbRadiusII;
				double dbY = i*dbRadiusII * 2 * SQRT_3;
				if (j%2)
				{
					dbY += dbRadiusII*SQRT_3;
				}
				glBegin(GL_POLYGON);
				for (size_t iP = 0; iP < 6; iP++)
				{
					glVertex2f(dbX + dbRadius*arrX[iP], dbY + dbRadius*arrY[iP]);
				}
				glEnd();

			}
		}
	}


	draw();

	bool bDrawGrid = false;
	if (bDrawGrid) {
		glColor3f(0, 1.0, 0);
		glBegin(GL_LINES);
		glVertex2d(-1.0, -1.0);
		glVertex2d(-1.0, 1.0);
		glVertex2d(0.0, -1.0);
		glVertex2d(0.0, 1.0);
		glVertex2d(1.0, -1.0);
		glVertex2d(1.0, 1.0);
		glVertex2d(-1.0, -1.0);
		glVertex2d(1.0, -1.0);
		glVertex2d(-1.0, 0.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(-1.0, 1.0);
		glVertex2d(1.0, 1.0);
		glEnd();
	}


	
	if (m_bRBtnDown) {
		// �������Բ�Ͱ뾶
		if (false) {
			DPoint3 pt1, pt2;
			screenToWorld(m_ptLBtnDown, pt1);
			screenToWorld(m_ptMouseCurrent, pt2);
			_pGLOperator->DrawCircle(pt1, (pt2 - pt1).norm());

			glBegin(GL_LINES);
			glVertex2d(pt1.x, pt1.y);
			glVertex2d(pt2.x, pt2.y);
			glEnd();
		}
	}
	


	onReTransform();
	onPostRenderScene();
}

void MyGLWidgetBase::drawCoords()
{
	/*
	//������
	SetColor(RGBAf(0, 0, 0, 1));
	// 	SetLineWidth(3);
	DrawLine3D(DPoint3(0, 0, 0), DPoint3(1000, 0, 0));
	DrawLine3D(DPoint3(0, 0, 0), DPoint3(0, 1000, 0));
	DrawLine3D(DPoint3(0, 0, 0), DPoint3(0, 0, 1000));
	//ͬ��Բ
	for (int i = 1; i<10; i++)
	{
		DrawCircle(DPoint3(0, 0, 0), i / 10.0);
	}
	*/
}
//////////////////////////////////////////////////////////////////////////
void MyGLWidgetBase::OnLButtonDown(int x, int y)
{
	m_bLBtnDown = true;
	m_ptLBtnDown.x = x;
	m_ptLBtnDown.y = y;
	m_ptMouseCurrent.x = x;
	m_ptMouseCurrent.y = y;


}
void MyGLWidgetBase::OnLButtonUp(int x, int y)
{
	m_bLBtnDown = false;
}
void MyGLWidgetBase::OnMouseMove(int x, int y)
{

	if (m_bLBtnDown)
	{
		onPanScreenDelta(x - m_ptMouseCurrent.x, m_ptMouseCurrent.y - y);

		m_ptMouseCurrent.x = x;
		m_ptMouseCurrent.y = y;
	}
	else if (m_bRBtnDown)
	{

		m_ptMouseCurrent.x = x;
		m_ptMouseCurrent.y = y;

		DPoint3 pt1;
		screenToWorld(IPoint2(x, y), pt1);
		onMouse(pt1);
	}
	else if (m_bMBtnDown)
	{

		m_ptMouseCurrent.x = x;
		m_ptMouseCurrent.y = y;
		//			ScreenRotate(m_ptMouseCurrent.x, m_ptMouseCurrent.y, x, y);
	}

}
void MyGLWidgetBase::OnMouseWheel(short zDelta)
{
	onZoomDelta(zDelta> 0 ? 0.03 : -0.03);
}
void MyGLWidgetBase::OnSize(int cx, int cy)
{
	_nHeight = cy;
	_nWidth = cx;
}
void MyGLWidgetBase::OnLButtonDblClk(int x, int y)
{

}
void MyGLWidgetBase::OnRButtonDown(int x, int y)
{
	m_bRBtnDown = TRUE;
	m_ptLBtnDown.x = x;
	m_ptLBtnDown.y = y;
	m_ptMouseCurrent.x = x;
	m_ptMouseCurrent.y = y;


}
void MyGLWidgetBase::OnRButtonUp(int x, int y)
{
	m_bRBtnDown = false;
}
void MyGLWidgetBase::OnMButtonDown(int x, int y)
{
	m_bMBtnDown = TRUE;
	m_ptLBtnDown.x = x;
	m_ptLBtnDown.y = y;
	m_ptMouseCurrent.x = x;
	m_ptMouseCurrent.y = y;
}
void MyGLWidgetBase::OnMButtonUp(int x, int y)
{
	m_bMBtnDown = false;
}
//////////////////////////////////////////////////////////////////////////
void MyGLWidgetBase::screenToWorld(int sx, int sy, double& wx, double& wy)
{
	DPoint3 wp;
	screenToWorld(IPoint2(sx, sy), wp);
	wx = wp.x;
	wy = wp.y;
}
//////////////////////////////////////////////////////////////////////////
void MyGLWidgetBase::OnZoomOut()
{
	onZoomDelta(0.5);
}

void MyGLWidgetBase::OnZoomIn()
{
	onZoomDelta(-0.5);
}

void MyGLWidgetBase::OnZoomBase()
{
	onBase();
}

void MyGLWidgetBase::onBase()
{
	// 	ZoomFitWrold(118.8,29.2,119.2,28.8);
	onZoomFitWrold(-0.4, 0.4, 0.4, -0.4);
}

void MyGLWidgetBase::OnKey(UINT nChar)
{
	/*
	37����
	38����
	39����
	40����
	*/

}





//////////////////////////////////////////////////////////////////////////OGL

#define WCHAR_SIZE					100
#define MIN(x,y)							(x)>(y)?(y):(x)
#define MAX(x,y)							(x)>(y)?(x):(y)
//��ȡ���������ԽǶ�
//��Χ��-90~270��X��������Ϊ0
double TwoPointToAngel(DPoint3 pt1, DPoint3 pt2)
{
	double xx = pt2.x - pt1.x;
	double yy = pt2.y - pt1.y;
	if (xx == 0)
	{
		return yy > 0 ? 90 : -90;
	}
	double angel = atan(yy / xx) * 180 / 3.14;
	if (xx < 0)
	{
		angel += 180;
	}
	return angel;
}
//��Χ��-90~270��X��������Ϊ0����Ļ���꣬y������
double TwoPointToAngel(int x1, int y1, int x2, int y2)
{
	double xx = x2 - x1;
	double yy = y2 - y1;
	if (xx == 0)
	{
		return yy > 0 ? 90 : -90;
	}
	double angel = atan(yy / xx) * 180 / 3.14;
	if (xx < 0)
	{
		angel += 180;
	}
	return angel;
}

void MyGLWidgetBase::onCreate()
{
	// Default mode
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glShadeModel(GL_FLAT);
	glEnable(GL_NORMALIZE);
	// 	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 	::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//��һ������������Ӱ
	//	glEnable( GL_TEXTURE_2D);//��һ���ÿռ�Ĺ�����������
	// 	glEnable(GL_DEPTH_TEST);
	//glEnable( GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA
	glClearDepth(1.0);

}//----------------------------------------------------------------------------------------------------

void MyGLWidgetBase::onPreRenderScene()
{
	//1.viewport setting
	glViewport(0, 0, _nWidth, _nHeight);
	//2.projection mode setting
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_dbFovy, (_nHeight == 0) ? 1.0 : (double)_nWidth / (double)_nHeight, m_dbZNear, m_dbZFar);

	//3.clear background
	glDrawBuffer(GL_BACK);
	//glClearColor(0,0,0,1);
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//4.Model View Setting
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//4.1 View
	gluLookAt(m_pt3Eye.x, m_pt3Eye.y, m_pt3Eye.z, m_pt3Eye.x, m_pt3Eye.y, 0, 0, 1, 0);
}//----------------------------------------------------------------------------------------------------
void MyGLWidgetBase::onPostRenderScene()
{
	// 	drawEagleView();
}//----------------------------------------------------------------------------------------------------
void MyGLWidgetBase::onTransform()
{
	//4.2 Model
	glTranslated(m_pt3Eye.x, m_pt3Eye.y, 0);
	glRotated(m_dbRotat.x, 1.0, 0.0, 0.0);
	glRotated(m_dbRotat.y, 0.0, 1.0, 0.0);
	glRotated(m_dbRotat.z, 0.0, 0.0, 1.0);
	glTranslated(-m_pt3Eye.x, -m_pt3Eye.y, 0);
	//��ת�����������ƶ���Eye��Ϊ���ģ������Ϳ������ӵ�Ϊ��׼�㡣
	//֮���ƶ���ԭ����λ�á�
	//ƽ�Ʊ任��ʱû��ʹ�ã�ƽ��Ч����ʹ���ӵ���ʵ�ֵġ�
	//20120127
	//ȥ����״̬�ı�����ָ����Լ�GL_DEPTH_TEST���޸�
	//20120205
	//ģ�ͱ任����ֻ�漰һ����ת���⣬ƽ��ͨ���ӵ�Ͳο��������
	//����ͨ���ӽǾ�����
	//20120206

	//���任���ڵ����ĺ����У�ʹ�ⲿ���Կ��Ʊ任��������
	//20121017
}
void MyGLWidgetBase::onReTransform()
{
	glTranslated(m_pt3Eye.x, m_pt3Eye.y, 0);
	glRotated(-m_dbRotat.z, 0.0, 0.0, 1.0);
	glRotated(-m_dbRotat.y, 0.0, 1.0, 0.0);
	glRotated(-m_dbRotat.x, 1.0, 0.0, 0.0);
	glTranslated(-m_pt3Eye.x, -m_pt3Eye.y, 0);
}
void MyGLWidgetBase::screenToWorld(const IPoint2& ps, DPoint3& pw)
{
	// 	//��Ļ��������
	// 	int x0=_nWidth/2;
	// 	int y0=_nHeight/2;
	// 	//��ȡ����Ļ����˳ʱ����תm_dbRotate.z�������
	// 	int x1=(ps.x-x0)*cos(m_dbRotat.z)+(y0-ps.y)*sin(m_dbRotat.z)+x0;
	// 	int y1=y0-((y0-ps.y)*cos(m_dbRotat.z)-(ps.x-x0)*sin(m_dbRotat.z));
	// 
	// 	pw.x=m_pt3Eye.x+(x1-_nWidth/2.0)*GetSWScale();
	// 	pw.y=m_pt3Eye.y+(_nHeight/2.0-y1)*GetSWScale();

	//��������ת�Ĵ���20120201
	pw.x = m_pt3Eye.x + (ps.x - _nWidth / 2.0)*GetSWScale();
	pw.y = m_pt3Eye.y + (_nHeight / 2.0 - ps.y)*GetSWScale();

}//----------------------------------------------------------------------------------------------------
void MyGLWidgetBase::worldToScreen(DPoint3& wp, IPoint2& sp)
{
	//notice, add 0.5
	sp.x = (int)((wp.x - m_pt3Eye.x) / GetSWScale() + _nWidth / 2.0 + 0.5);
	sp.y = (int)(_nHeight / 2.0 - (wp.y - m_pt3Eye.y) / GetSWScale() + 0.5);
}//----------------------------------------------------------------------------------------------------
void MyGLWidgetBase::onPanScreenDelta(int cx, int cy)
{
	double scale = GetSWScale();
	m_pt3Eye.x -= cx*scale;//(x*cos(m_rotation.z)-y*sin(m_rotation.z));
	m_pt3Eye.y -= cy*scale;//(y*cos(m_rotation.z)+x*sin(m_rotation.z));
	//cx,cy��ʾ��ʾ����ƽ�Ƶľ��롣
	//��Ҫ�������ű������ú��ȡ��ͼ��Ӧ�ľ��룬Ȼ���������ӵ㣨���㣩�ϡ�

//	qDebug() << m_dbFovy << "," << m_pt3Eye.x << "," << m_pt3Eye.y;
}//----------------------------------------------------------------------------------------------------
//20111208ע�ͣ��ݲ�������Ļ��ת
//20120127�������ã������ٴμ�����ת
void MyGLWidgetBase::onScreenRotate(int x1, int y1, int x2, int y2)
{
	// 	DPoint3 pt1;
	// 	DPoint3 pt2;
	// 	screenToWorld(IPoint2(x1,y1),pt1);
	// 	screenToWorld(IPoint2(x2,y2),pt2);
	// 	double angel1=TwoPointToAngel(m_pt3Eye,pt1);
	// 	double angel2=TwoPointToAngel(m_pt3Eye,pt2);
	// 	double angel = m_dbRotat.z+angel2-angel1;
	// 	if (angel>360)
	// 	{
	// 		angel-=360;
	// 	}
	// 	else if (angel<0)
	// 	{
	// 		angel+=360;
	// 	}
	// 	m_dbRotat.z=angel;
	//���ϴ�������������ϵ�ڽ�����ת�����ڸ�Ϊ����Ļ����ϵ����ת��20120201


	//��Ļ��������
	int x0 = _nWidth / 2;
	int y0 = _nHeight / 2;

	//����ת��
	double angle1 = TwoPointToAngel(x0, y0, x1, y1);
	double angle2 = TwoPointToAngel(x0, y0, x2, y2);

	double angel = m_dbRotat.z + angle1 - angle2;
	if (angel > 360)
	{
		angel -= 360;
	}
	else if (angel < 0)
	{
		angel += 360;
	}
	m_dbRotat.z = angel;
}//----------------------------------------------------------------------------------------------------
//20121011��x,y����ת��Ļ��
void MyGLWidgetBase::onScreenRoll(int cx, int cy)
{
	m_dbRotat.x += cx / 4.0;
	m_dbRotat.y += cy / 4.0;
}
void MyGLWidgetBase::onZoomDelta(double dScale)
{
	m_dbFovy *= (1 - dScale / 2.0);
	m_dbFovy > 180 ? m_dbFovy = 180 : 0;
	//�����Ұ����180�ȣ��ͻ���־���Ч����������Ӧ�ñ���ġ�20121011
	// 	m_dbScale*=(1+dScale);
	//֮ǰʹ��z����������ʵĴ���
	// 	double z=m_dbEyeZ-m_dbEyeZ*dScale;
	// 	if(z<m_dbZFar && z>m_dbZNear)
	// 	{
	// 		m_dbEyeZ=z;
	// 		calculateSWScale();
	// 	}
}//----------------------------------------------------------------------------------------------------
//20120105
void MyGLWidgetBase::onZoomFitWrold(double e, double n, double w, double s)
{

	char c[50];
	sprintf(c, "ClientW:%d\nClientH:%d\n", _nWidth, _nHeight);
	//Reset Eye DPoint3
	m_pt3Eye = DPoint3((e + w) / 2, (n + s) / 2, m_pt3Eye.z);

	//Get the Key factor(width or height which present a larger area)
	double dbWidth = abs(e - w);
	double dbHeight = abs(n - s);
	double dbHeightT = dbWidth*(double)_nHeight / (double)_nWidth;
	double dbKey = MAX(dbHeight, dbHeightT);
	//adjust m_dbScale
	// 	m_dbScale=m_pt3Eye.z*(tan(m_dbFovy*PId/180.0/2)*2.0)/dbKey;
	m_dbFovy = atan(dbKey / m_pt3Eye.z / 2.0)*180.0*2.0 / PId;

	//Reset Three Transform Factor
	m_dbRotat = DPoint3(0, 0, 0);
	// 	m_dbTranslate=DPoint3(0,0,0);
	//m_dbScale=1;
}//----------------------------------------------------------------------------------------------------

//20111208
void MyGLWidgetBase::drawEagleView()
{
	glReadBuffer(GL_BACK);
	IPoint2 ptS((double)_nWidth*0.8, (double)_nHeight*0.9);
	DPoint3 ptW;
	screenToWorld(ptS, ptW);
	glRasterPos3d(ptW.x, ptW.y, 0);
	glPixelZoom(0.1f, 0.1f);
	glCopyPixels(0, 0, _nWidth, _nHeight, GL_COLOR);
	glPixelZoom(1, 1);
}


