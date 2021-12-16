#include "MyGLWidget.h"

#include <gl/GLU.h>

#include <QMouseEvent>
#include <QDebug>
#include <math.h>
using namespace std;

MyGLWidget::MyGLWidget(QWidget *parent):MyGLWidgetBase(parent)
, m_dbRadius(1000)
, m_nMouseState(0)
, c_dbPI(3.14159265)
, _nSelectedX(10)
, _nSelectedY(10)
, _nSelectedLeft(-1)
, _nSelectedRight(-1)
, _nSelectedTop(-1)
, _nSelectedBottom(-1)

{
}


MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL()
{
	// 	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//	glEnable(GL_DEPTH_TEST);
	// 	glEnable(GL_LIGHTING);
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
/*
void MyGLWidget::paintGL(){
	//1.viewport setting
	glViewport(0, 0, _nWidth, _nHeight);
	//2.projection mode setting
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-_nWidth / 2.0, _nWidth / 2.0, -_nHeight / 2.0, _nHeight / 2.0, m_dbZNear, m_dbZFar);
	// 	gluPerspective(m_dbFovy, (m_nHeight == 0) ? 1.0 : (double)m_nWidth / (double)m_nHeight, m_dbZNear, m_dbZFar);

	//3.clear background
	glDrawBuffer(GL_BACK);
	// 	glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//4.Model View Setting
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//4.1 View
	double dbScale = 1;// m_dbScale / _dbScaleBase;
	gluLookAt(m_pt3Eye.x*dbScale, m_pt3Eye.y*dbScale, m_pt3Eye.z, m_pt3Eye.x*dbScale, m_pt3Eye.y*dbScale, 0, 0, 1, 0);

	//qDebug() << m_pt3Eye.x << "," << m_pt3Eye.y << "," << m_dbScale;
	glScaled(m_dbScale, m_dbScale, 1);
	

	//qDebug() << m_pt3Eye.x << m_pt3Eye.y << m_pt3Eye.z << m_dbScale << m_dbFovy << m_dbZNear << m_dbZFar;

	paint();
}


void MyGLWidget::resizeGL(int width, int height) {
	_nWidth = width;
	_nHeight = height;
	// 1.viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// 2.projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(30.0, (GLfloat)width / (GLfloat)height, .1, 100.0);
	
}

void MyGLWidget::timerEvent(QTimerEvent* event)
{

}

void MyGLWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::MidButton)
	{
	}
	else if (event->button() == Qt::LeftButton)
	{
		updateTrackBallPos(event->pos(), m_dbTrackBallPos);
		m_nMouseState = 1;
		_ptLast = event->pos();
	}
	else if (event->button() == Qt::RightButton)
	{
		m_nMouseState = 2;
		_ptLast = event->pos();
	}
	updateGL();
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent * event)
{

	m_nMouseState = 0;
	updateGL();

}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (m_nMouseState == 1) {	// left button down

		//double scale = GetSWScale();
		double dbScale = 1;// m_dbScale / _dbScaleBase;
		double dbBiasX = (event->pos().x() - _ptLast.x()); //cx*scale;//(x*cos(m_rotation.z)-y*sin(m_rotation.z));
		double dbBiasY = (event->pos().y() - _ptLast.y()); //cy*scale;//(y*cos(m_rotation.z)+x*sin(m_rotation.z));
		m_pt3Eye.x -= dbBiasX * dbScale;
		m_pt3Eye.y += dbBiasY * dbScale;

		_ptLast = event->pos();
	}
	updateGL();
}

void MyGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
}

void MyGLWidget::wheelEvent(QWheelEvent * event)
{
	if (event->delta()>0)
	{
		m_dbScale *= 1.03;
	}
	else
	{
		m_dbScale *= .97;
	}
	updateGL();
}
*/
void MyGLWidget::updateTrackBallPos(QPoint pt, double* result)
{

	double x = this->width() / 2 - pt.x();
	double y = pt.y() - this->height() / 2;
	if (x > m_dbRadius) x = m_dbRadius;
	if (y > m_dbRadius) y = m_dbRadius;
	double x2y2 = x*x + y*y;

	result[0] = x;
	result[1] = y;
	result[2] = sqrt(m_dbRadius*m_dbRadius - x2y2);

}



void MyGLWidget::drawCircle(double dbCX, double dbCY,double dbCZ, double dbR) {
	int nLen = 10;
	glBegin(GL_LINE_LOOP);
	for (size_t i = 0; i < nLen; i++)
	{
		double dbX = dbCX + dbR * cos(PI2d*i / nLen);
		double dbY = dbCY + dbR * sin(PI2d*i / nLen);
		glVertex3d(dbX, dbY, dbCZ);
	}
	glEnd();
}

void MyGLWidget::drawHull(std::vector<DPoint2> pts) {

	
}

