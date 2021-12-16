#pragma once


#include "def.h"
#include "GLFont.h"
#include "MeteLayer.h"

#include "MyGLWidget.h"
#include "BasicStruct.h"

class MeteLayer;
class MeteModel;
class EnsembleLayer;
class LayerLayout;
class CostLineLayer;


class MyMapWidget : public MyGLWidget,ILayerCB
{
	Q_OBJECT

public:
	// draw text
	virtual void DrawText(char* pText, double fX, double fY);
public:
	MyMapWidget(QWidget *parent = 0);
	~MyMapWidget();
protected:
//	virtual void resizeGL(int width, int height);
//	virtual void timerEvent(QTimerEvent* event);
//	virtual void mousePressEvent(QMouseEvent * event);
//	virtual void mouseDoubleClickEvent(QMouseEvent *event);
//	virtual void mouseReleaseEvent(QMouseEvent * event);
//	virtual void mouseMoveEvent(QMouseEvent *event);
//	virtual void wheelEvent(QWheelEvent * event);
private:

	GLFont _font;                /**< 定义一个字体实例 */

	// layout of the layers
	LayerLayout* _pLayout = NULL;

	// scale: 0.01 (180/1.8)
	double _fScaleW;
	double _fScaleH;

	double _fChartW;
	double _fChartLeft;
	double _fChartRight;

	double _fChartLW;
	double _fChartLLeft;
	double _fChartLRight;

	struct PickIndex
	{
		int _nX;
		int _nY;
		PickIndex(){}
		PickIndex(int x, int y) :_nX(x), _nY(y){}
	};

private:
	void drawPlaceHolder();
	// process the hits and trigger rotateLayer
	bool processHits(GLint hits, GLuint buffer[], PickIndex& pick);

	// draw a contour line
	void drawContourLine(const QList<ContourLine>& contours);

	// generate the background texture
	void generateBackground();


	// called when texture reloaded
	void onTextureReloaded();
public:
	void SetModelE(MeteModel* pModelE);
private:// state
	MeteLayer::DisplayStates _displayStates;
	bool _bShowMap = false;
	bool _bShowGridLines = false;




public slots:
	void viewShowMap(bool on);
	void viewShowGrid(bool on);
	void viewShowBackground(bool on);
	void viewShowCB(bool on);
	void viewShowVP(bool on);
	void viewShowCPP(bool on);


	void viewShowContourLine(bool on);
	void viewShowContourLineSorted(bool on);
	void viewShowContourLineSortedSDF(bool on);
	void viewShowContourLineResampled(bool on);
	void viewShowContourLineDomainResampled(bool on);
	void viewShowContourLineSDF(bool on);
	void viewShowContourLineMin(bool on);
	void viewShowContourLineMax(bool on);
	void viewShowContourLineMean(bool on);
	void viewShowContourLineMedian(bool on);
	void viewShowContourLineOutlier(bool on);

	void viewShowCluster(bool on);
	void viewShowRCBands(bool on);
	void onCheckShowBeliefEllipse(bool bChecked);
	// for clustering
	void updateMinPts(int minPts);
	void updateEps(double eps);
	// for clustered variance
	void updateVarSmooth(int nSmooth);
	void updateBgFunction(int nBgFunction);
	void updateTransparency(int nT);
	void updateFocusedCluster(int nFocusedCluster);
	void updateEOF(int nEOF);
	void updateMember(int nMember);
	void updateEnsCluster(int nEnsCluster);
	void updateEnsClusterLen(int nEnsClusterLen);
	void onUpdateView();


	void onIsoValue0(bool bState);
	void onIsoValue1(bool bState);
	void onIsoValue2(bool bState);
	void onIsoValue3(bool bState);
	void onIsoValue4(bool bState);
	void onIsoValue5(bool bState);
	void onIsoValue6(bool bState);
	void onIsoValue7(bool bState);
	void onIsoValue8(bool bState);
	void onIsoValue9(bool bState);


private:
	
	std::vector<MeteLayer*> _vecLayers;	// vector of layers to render
	EnsembleLayer* _pEnsLayer;			// ensemble layer
	CostLineLayer* _pMapLayer;
	MeteModel* _pModelE;				// model



protected:
	// paint the content
	virtual void paint(); 
	virtual void init();
	virtual void onMouse(DPoint3& pt);

};

