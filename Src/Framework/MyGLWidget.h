#pragma once

#include <MathTypes.hpp>

#include"MyGLWidgetBase.h"
class MyGLWidget : public MyGLWidgetBase
{
	Q_OBJECT
public:
	MyGLWidget(QWidget *parent = 0);
	~MyGLWidget();



protected:
	// state of the mouse
	// 0-noButton
	// 1-L Button down
	// 2-R button down and picking
	int m_nMouseState;

				// Trackball radius
	double m_dbRadius;
	// last trackball position
	double m_dbTrackBallPos[3];
				// global modelview matrix
	double m_modelViewMatrixGlobal[16];
	const double c_dbPI;
	QPoint _ptLast;
	// viewport size
	int _nWidth;
	int _nHeight;

	// the selected x and y
	int _nSelectedX;
	int _nSelectedY;

	// selected area, used for brushing
	int _nSelectedLeft;
	int _nSelectedRight;
	int _nSelectedTop;
	int _nSelectedBottom;





protected:
	double GetSWScale()
	{
		return ((0 == _nHeight) ? 1.0 : (m_pt3Eye.z*(tan(m_dbFovy*PId / 180.0 / 2)*2.0)) / (double)_nHeight);
	}
protected:
	// paint the content
	virtual void paint() = 0;
	virtual void init() = 0;
	
protected:
	virtual void initializeGL();
	//virtual void paintGL();
	//virtual void resizeGL(int width, int height);
	//virtual void timerEvent(QTimerEvent* event);
	//virtual void mousePressEvent(QMouseEvent * event);
	//virtual void mouseDoubleClickEvent(QMouseEvent *event);
	//virtual void mouseReleaseEvent(QMouseEvent * event);
	//virtual void mouseMoveEvent(QMouseEvent *event);
	//virtual void wheelEvent(QWheelEvent * event);
	
protected:
	void updateTrackBallPos(QPoint pt, double* result);
protected:
	void drawCircle(double dbX, double dbY, double dbCZ, double dbR);
	void drawHull(std::vector<DPoint2> pts);


	virtual void draw() { paint(); };
	virtual void onMouse(DPoint3& pt) {};
};

