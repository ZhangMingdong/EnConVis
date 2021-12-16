#pragma once

#include <MathTypes.hpp>

// #include <gl/glew.h>
#include <QGLWidget>
#include <gl/GLU.h>

class MyGLOperator;

class IWorldMapping {
public:
	//Change point between screen and world
	virtual void ScreenToWorld(const IPoint2& ps, DPoint3& pw) = 0;
	virtual void WorldToScreen(DPoint3& wp, IPoint2& sp) = 0;
};

class MyGLWidgetBase : public QGLWidget,public IWorldMapping
{
	Q_OBJECT

public:
	MyGLWidgetBase(QWidget *parent);
	~MyGLWidgetBase();
protected:
	MyGLOperator* _pGLOperator = NULL;

protected: // system events
	virtual void	mousePressEvent(QMouseEvent * event);
	virtual void	mouseReleaseEvent(QMouseEvent * event);
	virtual void	mouseMoveEvent(QMouseEvent * event);
	virtual void	wheelEvent(QWheelEvent * event);
	virtual void	keyPressEvent(QKeyEvent * event);
protected:// system operation
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	virtual void timerEvent(QTimerEvent* event);

public:
	virtual void Draw();
public://Interaction
	virtual void OnLButtonDown(int x, int y);
	virtual void OnLButtonUp(int x, int y);
	virtual void OnLButtonDblClk(int x, int y);
	virtual void OnRButtonDown(int x, int y);
	virtual void OnRButtonUp(int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseWheel(short zDelta);
	virtual void OnMButtonDown(int x, int y);
	virtual void OnMButtonUp(int x, int y);
public://OpenGL operation
	virtual void screenToWorld(int sx, int sy, double& wx, double& wy);
	virtual void OnZoomOut();
	virtual void OnZoomIn();
	virtual void OnZoomBase();
	virtual void OnSize(int cx, int cy);
protected:// states of the mouse
	bool m_bLBtnDown;
	bool m_bRBtnDown;
	bool m_bMBtnDown;
	IPoint2 m_ptLBtnDown;
	IPoint2 m_ptMouseCurrent;
protected://Perspective
	int _nWidth;
	int _nHeight;
	DPoint3 m_pt3Eye;					// camera point
	GLdouble m_dbZNear;
	GLdouble m_dbZFar;
	GLdouble m_dbFovy;					// angle of eye open

	DPoint3 m_dbRotat;

protected:// coordinate operation

	//Prepare before drawing
	void  onPreRenderScene();
	//Post scene after drawing
	void  onPostRenderScene();
	//Model View Transform
	void onTransform();
	//Recover Model View Transform
	void onReTransform();

	//Change point between screen and world
	void screenToWorld(const IPoint2& ps, DPoint3& pw);
	void worldToScreen(DPoint3& wp, IPoint2& sp);
	//Move the current area
	void onPanScreenDelta(int cx, int cy);

	//Initialization
	void  onCreate();

	//Rotate Screen
	void onScreenRotate(int x1, int y1, int x2, int y2);
	//Roll Screen
	void onScreenRoll(int cx, int cy);
	//Zoom in or zoom out
	void onZoomDelta(double dScale);
	//Zoom into a rect	
	void onZoomFitWrold(double e, double n, double w, double s);

	double GetSWScale()
	{
		return ((0 == _nHeight) ? 1.0 : (m_pt3Eye.z*(tan(m_dbFovy*PId / 180.0 / 2)*2.0)) / (double)_nHeight);
	}
	IPoint2 GetClientRect() { return IPoint2(_nWidth, _nHeight); }
	// 	IPoint2& GetClientRect(){return m_clientRect;}


protected:
	//set base rect
	void onBase();
	// draw coordinate
	void drawCoords();
public:
	virtual void OnKey(UINT nChar);
protected:
	double SW(double s){ return s*GetSWScale(); }
	void SW(FBox3& sb)
	{
		sb._x1 *= GetSWScale();
		sb._x2 *= GetSWScale();
		sb._y1 *= GetSWScale();
		sb._y2 *= GetSWScale();
	}
protected:
	//20111208
	void drawEagleView();





protected:
	virtual void draw() = 0;
	virtual void onMouse(DPoint3& pt) = 0;
	virtual void init() = 0;
public:
	//Change point between screen and world
	virtual void ScreenToWorld(const IPoint2& ps, DPoint3& pw) { screenToWorld(ps, pw); }
	virtual void WorldToScreen(DPoint3& wp, IPoint2& sp) { worldToScreen(wp, sp); }
};

