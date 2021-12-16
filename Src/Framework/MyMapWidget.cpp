#include "MyMapWidget.h"
#include <gl/GLU.h>

#include <QMouseEvent>
#include <QDebug>

#include "MeteLayer.h"
#include "CostLineLayer.h"
#include "EnsembleLayer.h"
#include "MeteModel.h"
#include "LayerLayout.h"
#include "Switch.h"



// used for draw grid line
const int g_globalW = 361;
const int g_globalH = 181;


MyMapWidget::MyMapWidget(QWidget *parent)
: MyGLWidget(parent)
{
	_pLayout = new LayerLayout();
	
	_fScaleW = 0.01;
	_fScaleH = 0.01;

	_fChartLeft = 0.54;
	_fChartW = .5;
	_fChartRight = _fChartLeft + _fChartW;
	
	_fChartLRight = -0.54;
	_fChartLW = .5;
	_fChartLLeft = _fChartLRight-_fChartLW;

	// for china area
	bool g_bChina = true;
	if (g_bChina) {
		m_dbFovy = 7.79498;
		m_pt3Eye.x = 1.05068;
		m_pt3Eye.y = 0.333055;
	}

}

MyMapWidget::~MyMapWidget()
{
	// release the layers
	for each (MeteLayer* layer in _vecLayers)
	{
		delete layer;
	}

	delete _pMapLayer;
	delete _pLayout;
}

void MyMapWidget::init()
{
	_pEnsLayer = new EnsembleLayer();
	_pEnsLayer->SetModel(_pModelE);
	_pEnsLayer->InitLayer(_pLayout, _fScaleW, _fScaleH);
	_vecLayers.push_back(_pEnsLayer);

	/*
	pLayer = new ClusterLayer();
	pLayer->SetModel(_pModelE);
	pLayer->InitLayer(_pLayout, _fScaleW, _fScaleH);
	_vecLayers.push_back(pLayer);
	*/
	// create cost line layer
	_pMapLayer = new CostLineLayer();
	_pMapLayer->InitLayer(_pLayout, _fScaleW, _fScaleH);
	//_vecLayers.push_back(pLayer);


	_font.InitFont(wglGetCurrentDC(), L"Arial", 22);
	MeteLayer::_pCB = this;
}

void MyMapWidget::drawContourLine(const QList<ContourLine>& contours) {

	for each (ContourLine contour in contours)
	{
		glBegin(GL_LINE_STRIP);
		for each (QPointF pt in contour._listPt)
		{
			double x = _pLayout->_dbLeft + pt.x() * _fScaleW;
			double y = _pLayout->_dbTop - pt.y() * _fScaleH;
			glVertex2f(x, y);
		}
		glEnd();
	}
}

void MyMapWidget::paint() {

	// draw selected point
	glLineWidth(2.0f);
	glColor4f(1.0, .3, 0.0, 1.0);
	double fX0 = _pLayout->_dbLeft + _nSelectedLeft*_fScaleW;
	double fY0 = _pLayout->_dbBottom + _nSelectedBottom*_fScaleH;
	double fX1 = _pLayout->_dbLeft + _nSelectedRight*_fScaleW;
	double fY1 = _pLayout->_dbBottom + _nSelectedTop*_fScaleH;
	glBegin(GL_LINE_LOOP);
	glVertex2f(fX0, fY0);
	glVertex2f(fX0, fY1);
	glVertex2f(fX1, fY1);
	glVertex2f(fX1, fY0);
	glEnd();


	for each (MeteLayer* pLayer in _vecLayers)
	{
		pLayer->DrawLayer(_displayStates);
	}

	// grid lines
	if (_bShowGridLines) {

		glColor4f(.5, .5, .5, .5);
		glBegin(GL_LINES);
		int nStep = 10;
		for (int i = 0; i < g_globalW; i += nStep)
		{
			glVertex2f(_pLayout->_dbLeft + i*_fScaleW, _pLayout->_dbBottom);
			glVertex2f(_pLayout->_dbLeft + i*_fScaleW, _pLayout->_dbTop);
		}
		for (int j = 0; j < g_globalH; j += nStep)
		{
			glVertex2f(_pLayout->_dbLeft, _pLayout->_dbBottom + j*_fScaleH);
			glVertex2f(_pLayout->_dbRight, _pLayout->_dbBottom + j*_fScaleH);
		}
		glEnd();
		// show detail
		if (true) {
			glColor4f(.5, .5, .5, .1);
			glBegin(GL_LINES);
			int nStep = 1;
			for (int i = 0; i < g_globalW; i += nStep)
			{
				glVertex2f(_pLayout->_dbLeft + i*_fScaleW, _pLayout->_dbBottom);
				glVertex2f(_pLayout->_dbLeft + i*_fScaleW, _pLayout->_dbTop);
			}
			for (int j = 0; j < g_globalH; j += nStep)
			{
				glVertex2f(_pLayout->_dbLeft, _pLayout->_dbBottom + j*_fScaleH);
				glVertex2f(_pLayout->_dbRight, _pLayout->_dbBottom + j*_fScaleH);
			}
			glEnd();
		}
	}


	if (_bShowMap)
		_pMapLayer->DrawLayer(_displayStates);
}

void MyMapWidget::SetModelE(MeteModel* pModelE){
	_pModelE = pModelE;
}

bool MyMapWidget::processHits(GLint hits, GLuint buffer[], PickIndex& pick)
{
	if (hits > 0)
	{
		GLuint* ptr = (GLuint*)buffer;
		double fLastDepth = 0;
		if (hits>0)
		{
			// name
			GLuint names = *ptr;
			ptr++;
			PickIndex pickI;
			// depth1
			double depth1 = (double)*ptr;
			ptr++;
			// depth2
			double depth2 = (double)*ptr;
			ptr++;
			// x
			pickI._nX = *ptr;
			ptr++;
			// y
			pickI._nY = *ptr;
			ptr++;
			pick = pickI;
		}
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void MyMapWidget::drawPlaceHolder()
{
	/*
	for (int i = 0; i < g_focus_w;i++)
	{
		glLoadName(i);
		for (int j = 0; j < g_focus_h; j++){
			glPushName(j);
			double fX = _pLayout->_dbLeft + i*_fScaleW;
			double fY = _pLayout->_dbBottom + j*_fScaleH;
			glBegin(GL_QUADS);
			glVertex2f(fX - 0.5*_fScaleW, fY - 0.5*_fScaleH);
			glVertex2f(fX + 0.5*_fScaleW, fY - 0.5*_fScaleH);
			glVertex2f(fX + 0.5*_fScaleW, fY + 0.5*_fScaleH);
			glVertex2f(fX - 0.5*_fScaleW, fY + 0.5*_fScaleH);
			glEnd();
			glPopName();
		}
	}

	// for variance chart
	int nWidth = VarAnalysis::_nVarBars;
	double dbStep = (_pLayout->_dbRight-_pLayout->_dbLeft) / nWidth;

	for (size_t i = 0; i < nWidth; i++)
	{
		glLoadName(i + g_focus_w);
		double fX0 = _pLayout->_dbLeft + i*dbStep;
		double fX1 = _pLayout->_dbLeft + (i+1)*dbStep;
		double fY = _pLayout->_dbTop;
		glBegin(GL_QUADS);
		glVertex2f(fX0, fY );
		glVertex2f(fX1, fY );
		glVertex2f(fX1, fY + 10);
		glVertex2f(fX0, fY + 10);
		glEnd();
	}
	*/
}

void MyMapWidget::viewShowMap(bool on) {
	_bShowMap = on;
	updateGL();
}

void MyMapWidget::viewShowGrid(bool on){
	_bShowGridLines = on;
	updateGL();
}

void MyMapWidget::viewShowBackground(bool on){
	_displayStates._bShowBackground = on; updateGL();
};

void MyMapWidget::onCheckShowBeliefEllipse(bool bChecked) {
	_displayStates._bShowBeliefEllipse = bChecked; updateGL();
}



void MyMapWidget::viewShowCB(bool on) {
	_displayStates._bShowCB = on;
	updateGL();
};
void MyMapWidget::viewShowVP(bool on) {
	_displayStates._bShowVP = on;
	updateGL();
};
void MyMapWidget::viewShowCPP(bool on) {
	_displayStates._bShowCPP = on;
	updateGL();
};


void MyMapWidget::viewShowContourLine(bool on){
	_displayStates._bShowContourLine = on; updateGL(); };

void MyMapWidget::viewShowContourLineSorted(bool on) {
	_displayStates._bShowContourLineSorted = on; updateGL();
};

void MyMapWidget::viewShowContourLineSortedSDF(bool on) {
	_displayStates._bShowContourLineSortedSDF = on; updateGL();
};

void MyMapWidget::viewShowContourLineResampled(bool on) {
	_displayStates._bShowContourLineResampled = on; updateGL();
};

void MyMapWidget::viewShowContourLineDomainResampled(bool on) {
	_displayStates._bShowContourLineDomainResampled = on; updateGL();
};

void MyMapWidget::viewShowContourLineSDF(bool on) {
	_displayStates._bShowContourLineSDF = on; updateGL();
};

void MyMapWidget::viewShowContourLineMin(bool on){
	_displayStates._bShowContourLineMin = on; updateGL();
};

void MyMapWidget::viewShowContourLineMax(bool on){
	_displayStates._bShowContourLineMax = on; updateGL();
};

void MyMapWidget::viewShowContourLineMean(bool on){ 
	_displayStates._bShowContourLineMean = on; updateGL();
};

void MyMapWidget::viewShowContourLineMedian(bool on) {
	_displayStates._bShowContourLineMedian = on; updateGL();
};

void MyMapWidget::viewShowContourLineOutlier(bool on) {
	_displayStates._bShowContourLineOutlier = on; updateGL();
};

void MyMapWidget::viewShowCluster(bool on) {
	_displayStates._bShowCluster = on; updateGL();
};

void MyMapWidget::viewShowRCBands(bool on) {
	_displayStates._bShowRCBands = on; updateGL();
};


void MyMapWidget::DrawText(char* pText, double fX, double fY){
	_font.PrintText(pText, fX, fY);

}

// reload texture
void MyMapWidget::onTextureReloaded() {
	for each (MeteLayer* pLayer in _vecLayers)
	{
		pLayer->ReloadTexture();
	}

	updateGL();
}

void MyMapWidget::updateMinPts(int minPts) {
	qDebug() << "set minpts";
	/*
	if(_pModelE)
		_pModelE->SetMinPts(minPts);

	onTextureReloaded();
	*/
}

void MyMapWidget::updateEps(double eps) {
	qDebug() << "set eps";
	/*
	if (_pModelE)
		_pModelE->SetEps(eps);

	onTextureReloaded(); 
	*/
}

void MyMapWidget::updateVarSmooth(int nSmooth) {
	if (_pModelE)
		_pModelE->SetSmooth(nSmooth);
	onTextureReloaded();
}

void MyMapWidget::updateBgFunction(int nBgFunction) {
	if (_pModelE)
		_pModelE->SetBgFunction((MeteModel::enumBackgroundFunction)nBgFunction);
	onTextureReloaded();
}

void MyMapWidget::updateTransparency(int nT) {
	if (_pModelE)
		_pModelE->SetTransparency(nT);

	updateGL();
}

void MyMapWidget::updateFocusedCluster(int nFocusedCluster) {
	for each (MeteLayer* pLayer in _vecLayers)
	{
		pLayer->SetFocusedCluster(nFocusedCluster);
	}

	updateGL();
}

void MyMapWidget::updateEOF(int nEOF) {

}

void MyMapWidget::updateMember(int nMember) {
	if (_pModelE)
		_pModelE->SetMember(nMember);
	onTextureReloaded();
}

void MyMapWidget::updateEnsCluster(int nEnsCluster) {
	if (_pModelE)
		_pModelE->SetEnsCluster(nEnsCluster);
	onTextureReloaded();
}

void MyMapWidget::updateEnsClusterLen(int nEnsClusterLen) {
	if (_pModelE)
		_pModelE->SetEnsClusterLen(nEnsClusterLen);
	onTextureReloaded();
}



void MyMapWidget::onUpdateView() {
	qDebug() << "MyMapWidget::onUpdateView()";
	updateGL();
}

void MyMapWidget::onIsoValue0(bool bState) { _pEnsLayer->_arrIsoValueState[0] = bState; updateGL(); };
void MyMapWidget::onIsoValue1(bool bState) { _pEnsLayer->_arrIsoValueState[1] = bState; updateGL(); };
void MyMapWidget::onIsoValue2(bool bState) { _pEnsLayer->_arrIsoValueState[2] = bState; updateGL(); };
void MyMapWidget::onIsoValue3(bool bState) { _pEnsLayer->_arrIsoValueState[3] = bState; updateGL(); };
void MyMapWidget::onIsoValue4(bool bState) { _pEnsLayer->_arrIsoValueState[4] = bState; updateGL(); };
void MyMapWidget::onIsoValue5(bool bState) { _pEnsLayer->_arrIsoValueState[5] = bState; updateGL(); };
void MyMapWidget::onIsoValue6(bool bState) { _pEnsLayer->_arrIsoValueState[6] = bState; updateGL(); };
void MyMapWidget::onIsoValue7(bool bState) { _pEnsLayer->_arrIsoValueState[7] = bState; updateGL(); };
void MyMapWidget::onIsoValue8(bool bState) { _pEnsLayer->_arrIsoValueState[8] = bState; updateGL(); };
void MyMapWidget::onIsoValue9(bool bState) { _pEnsLayer->_arrIsoValueState[9] = bState; updateGL(); };

void MyMapWidget::onMouse(DPoint3& pt) {
	_pModelE->SetFocus(DPoint2(pt.x,pt.y));
}