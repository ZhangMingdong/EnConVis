#include "EnsembleLayer.h"
#include "MeteModel.h"

#include "ColorMap.h"
#include "def.h"
#include "LayerLayout.h"


#include <QDebug>

#include "Switch.h"


EnsembleLayer::EnsembleLayer()
{
}

EnsembleLayer::~EnsembleLayer()
{
	if (_colorbarTexture)
	{
		delete[]_colorbarTexture;
	}	
}

void SetColor(int nIndex,double dbValue,double dbOpacity=1.0) {
	bool bCategorical = true;
	if (bCategorical) {
		glColor4f(ColorMap::GetCategory10D(nIndex, 0)
			, ColorMap::GetCategory10D(nIndex, 1)
			, ColorMap::GetCategory10D(nIndex, 2)
			, dbOpacity);
	}
	else {
		ColorMap* pColorMap=ColorMap::GetInstance(ColorMap::CP_EOF);
		MYGLColor color = pColorMap->GetColor(dbValue);
		glColor4f(color._rgb[0] / 255.0
			, color._rgb[1] / 255.0
			, color._rgb[2] / 255.0
			, dbOpacity);
	}
}

void EnsembleLayer::ReloadTexture() {


	// truth texture, generated from truth data
	const GLubyte* dataTexture = _pModel->GenerateTexture();

	glBindTexture(GL_TEXTURE_2D, _uiTexID[0]);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _pModel->GetTexW(), _pModel->GetTexH(), 0, GL_RGBA, GL_UNSIGNED_BYTE, dataTexture);

	qDebug() << _pModel->GetTexW() << "," << _pModel->GetTexH();

	generateColorBarTexture();
}

void EnsembleLayer::init(){
	// 0.generate texture
	glGenTextures(2, _uiTexID);

	// 1.load texture
	ReloadTexture();

	// 2.generate color bar
	generateColorBarTexture();

	// 3.initialize tess
	buildTess();
}

void EnsembleLayer::drawContourLine(const QList<ContourLine>& contours){

	for each (ContourLine contour in contours)
	{
		glBegin(GL_LINE_STRIP);
		for each (QPointF pt in contour._listPt)
		{
			double x = pt.x();// *_fScaleW;
			double y = pt.y();// *_fScaleH;
			glVertex2f(x, y);
		}
		glEnd();
	}
}

void EnsembleLayer::Brush(int nLeft, int nRight, int nTop, int nBottom) {
	_pModel->Brush(nLeft, nRight, nTop, nBottom);
}

void EnsembleLayer::drawColorBar() {
	// border
	glColor3f(0, 0, 0);		// black
	glBegin(GL_LINE_LOOP);
	glVertex2f(_pLayout->_dbColorBarLeft, _pLayout->_dbBottom);
	glVertex2f(_pLayout->_dbColorBarRight, _pLayout->_dbBottom);
	glVertex2f(_pLayout->_dbColorBarRight, _pLayout->_dbTop);
	glVertex2f(_pLayout->_dbColorBarLeft, _pLayout->_dbTop);
	glEnd();
	MeteModel::enumBackgroundFunction bgFun = _pModel->GetBgFunction();
	switch (bgFun)
	{
	case MeteModel::bg_mean:
	case MeteModel::bg_Obs:
	case MeteModel::bg_err:
	case MeteModel::bg_vari:
	case MeteModel::bg_vari_smooth:
	case MeteModel::bg_dipValue:
	case MeteModel::bg_EOF:
	{
		ColorMap* colormap;
		switch (bgFun)
		{
		case MeteModel::bg_mean:
		case MeteModel::bg_Obs:
			colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
			break;
		case MeteModel::bg_err:
			colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
			break;
		default:
			colormap = ColorMap::GetInstance();
			break;

		}

		int nLen = colormap->GetLength();
		int nStep = colormap->GetStep();
		int nMin = colormap->GetMin();
		for (int i = 0; i < nLen; i++)
		{
			glBegin(GL_LINES);
			glVertex2f(_pLayout->_dbColorBarRight, _pLayout->_dbBottom + (_pLayout->_dbTop - _pLayout->_dbBottom)*i / (nLen - 1));
			glVertex2f(_pLayout->_dbColorBarRight-.01, _pLayout->_dbBottom + (_pLayout->_dbTop - _pLayout->_dbBottom)*i / (nLen - 1));
			glEnd();
			// draw text

			char buf[10];
			sprintf_s(buf, "%d", nMin+i*nStep);
			_pCB->DrawText(buf, _pLayout->_dbColorBarRight + .02, _pLayout->_dbBottom + (_pLayout->_dbTop - _pLayout->_dbBottom)*i / (nLen - 1) - .01);
		}
		// colors
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _uiTexID[1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.1f, 0.0f); glVertex2f(_pLayout->_dbColorBarLeft, _pLayout->_dbBottom);
		glTexCoord2f(0.9f, 0.0f); glVertex2f(_pLayout->_dbColorBarRight, _pLayout->_dbBottom);
		glTexCoord2f(0.9f, 1.0f); glVertex2f(_pLayout->_dbColorBarRight, _pLayout->_dbTop);
		glTexCoord2f(0.1f, 1.0f); glVertex2f(_pLayout->_dbColorBarLeft, _pLayout->_dbTop);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
		break;
	case MeteModel::bg_cluster:

		break;
	case MeteModel::bg_varThreshold:
	case MeteModel::bg_dipValueThreshold:
	{
		int nLen = _pModel->GetGridLength();
		int nTLen = _pModel->GetThresholdedGridLength();
		double dbMid = _pLayout->_dbTop-_pLayout->_dbHeight*nTLen / nLen;
		glColor3f(ColorMap::GetThresholdColorD(0), ColorMap::GetThresholdColorD(1), ColorMap::GetThresholdColorD(2));
		glBegin(GL_QUADS);
		glVertex2f(_pLayout->_dbColorBarLeft, dbMid);
		glVertex2f(_pLayout->_dbColorBarRight, dbMid);
		glVertex2f(_pLayout->_dbColorBarRight, _pLayout->_dbTop);
		glVertex2f(_pLayout->_dbColorBarLeft, _pLayout->_dbTop);
		glEnd();

		glColor3f(0, 0, 0);
		int nPercentage = nTLen * 100 / nLen;
		char buf[10];
		sprintf_s(buf, "%d%%", nPercentage);
		_pCB->DrawText(buf, _pLayout->_dbColorBarRight + .02, (_pLayout->_dbTop + dbMid) / 2.0);


		sprintf_s(buf, "%d%%", 100-nPercentage);
		_pCB->DrawText(buf, _pLayout->_dbColorBarRight + .02, (_pLayout->_dbBottom + dbMid) / 2.0);

	}
		break;
	default:
		break;
	}
}

// generate the texture for the color bar
void EnsembleLayer::generateColorBarTexture() {
	// 1.generate color bar data
	MeteModel::enumBackgroundFunction bgFun = _pModel->GetBgFunction();

	ColorMap* colormap;
	switch (bgFun)
	{
	case MeteModel::bg_mean:
	case MeteModel::bg_Obs:
		colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
		break;
	case MeteModel::bg_err:
		colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
		break;
	default:
		colormap = ColorMap::GetInstance();
		break;

	}

	int nLen = colormap->GetLength();
	int nStep = colormap->GetStep();
	int nMin = colormap->GetMin();
	int nColorBarW = 1;
	int nColorBarH = (nLen - 1) * 10 + 1;
	_colorbarTexture = new GLubyte[nColorBarH*nColorBarW * 4];
	for (int i = 0; i < nColorBarH; i++)
	{
//		MYGLColor color = colormap->GetColor(i / 10.0*nStep);
		MYGLColor color = colormap->GetColor(nMin+i*nStep/10.0);
		for (size_t j = 0; j < nColorBarW; j++)
		{
			int nIndex = nColorBarW*i + j;
			_colorbarTexture[4 * nIndex + 0] = color._rgb[0];
			_colorbarTexture[4 * nIndex + 1] = color._rgb[1];
			_colorbarTexture[4 * nIndex + 2] = color._rgb[2];
			_colorbarTexture[4 * nIndex + 3] = (GLubyte)255;
		}
	}
	// 2.build texture
	glBindTexture(GL_TEXTURE_2D, _uiTexID[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nColorBarW, nColorBarH, 0, GL_RGBA, GL_UNSIGNED_BYTE, _colorbarTexture);
	// ~build texture
}

// build the tess for uncertainty regions
void EnsembleLayer::buildTess() {
	int nIsoValues = _pModel->GetIsoValues().length();
	int nTimeSteps = (GlobalSetting::g_nTimeEnd - GlobalSetting::g_nTimeStart) / 6 + 1;
	int nTimeStartIndex = GlobalSetting::g_nTimeStart / 6;
	   
	// 4.tess the areas
	for (size_t i = 0; i < nTimeSteps; i++)
	{
		for (size_t j = 0; j < nIsoValues; j++)
		{
			_pModel->BuildTess(nTimeStartIndex + i, j);
		}
	}
}


void EnsembleLayer::draw(DisplayStates states) {
	// 0.get variables
	int nClusterIndex = -1;	// -1 means show all clusters

	// the x radius and y radius of the map in drawing space
	double biasX = (_pModel->GetWest() + _pModel->GetEast()) / 2.0 *_fScaleW;
	double biasY = (_pModel->GetSouth() + _pModel->GetNorth()) / 2.0*_fScaleH;

	// the focused x radius and y radius of the map in drawing space
	double biasFocusX = (_pModel->GetWest() + _pModel->GetEast()) / 2.0 *_fScaleW;
	double biasFocusY = (_pModel->GetSouth() + _pModel->GetNorth()) / 2.0*_fScaleH;

	double scaleFocusX = (_pModel->GetEast() - _pModel->GetWest()) / 360.0;
	double scaleFocusY = (_pModel->GetNorth() - _pModel->GetSouth()) / 180.0;



	// 1.border
	glColor3f(0, 0, 0);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(_pLayout->_dbLeft, _pLayout->_dbBottom);
	glVertex2f(_pLayout->_dbRight, _pLayout->_dbBottom);
	glVertex2f(_pLayout->_dbRight, _pLayout->_dbTop);
	glVertex2f(_pLayout->_dbLeft, _pLayout->_dbTop);
	glEnd();





	// 2.draw background
	if (states._bShowBackground) {
		if (false) {
			glPushMatrix();
			glTranslatef(biasFocusX, biasFocusY, 0);			// 位置偏移
			glScalef(scaleFocusX, scaleFocusY, 0);				// 改变尺寸

			drawIsoContourDensityPlots();
			

			glPopMatrix();

			// color bar
			drawColorBar();
		}
		else {
			// old background codes
			glPushMatrix();
			glTranslatef(biasFocusX, biasFocusY, 0);			// 位置偏移
			glScalef(scaleFocusX, scaleFocusY, 0);				// 改变尺寸


			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, _uiTexID[0]);

			qDebug()<<"texture id" << _uiTexID[0]<< _uiTexID[1];


			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(_pLayout->_dbLeft, _pLayout->_dbBottom);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(_pLayout->_dbRight, _pLayout->_dbBottom);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(_pLayout->_dbRight, _pLayout->_dbTop);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(_pLayout->_dbLeft, _pLayout->_dbTop);

			glEnd();

			glDisable(GL_TEXTURE_2D);


			glPopMatrix();

			// color bar
			drawColorBar();
		}
	}


	// 3. draw content

	glTranslatef(biasX, biasY, 0);				// 移动到指定位置
	glTranslatef(-(_pModel->GetW() - 1)*_fScaleW / 2, -(_pModel->GetH() - 1)*_fScaleH / 2, 0);		// 移动到中间
	glScalef(_fScaleW, _fScaleH, 0);

	// ==area==

	if (states._bShowVP) drawVariabilityPlots(states._bShowCluster);

	if (states._bShowCB) drawContourBoxplots();
	if (states._bShowCPP) drawContourProbabilityPlots();

	// == lines ==
	if (states._bShowContourLineOutlier)
		drawContourLineOutlier();
	if (states._bShowContourLineMin)
		drawContourLineMin();
	if (states._bShowContourLineMax)
		drawContourLineMax();
	if (states._bShowContourLineMean)
		drawContourLineMean();
	if (states._bShowContourLine)
		drawContourLine(states._bShowCluster);
	if (states._bShowContourLineSorted)
		drawContourLineSorted();
	if (states._bShowContourLineSortedSDF)
		drawContourLineSortedSDF();
	if (states._bShowContourLineResampled)
		drawContourLineResampled(states._bShowCluster);
	if (states._bShowContourLineDomainResampled)
		drawContourLineDomainResampled();
	if (states._bShowContourLineSDF)
		drawContourLineSDF(states._bShowCluster);
	if (states._bShowRCBands)
		drawEnConVisBands(states._bShowCluster);

}

void EnsembleLayer::drawContourBoxplots() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = _pModel->GetIsoValues().length();
	//int nTimeSteps = (g_nTimeEnd - g_nTimeStart) / 6 + 1;
	int nTimeStartIndex = GlobalSetting::g_nTimeStart / 6;
	for (size_t j = 0; j < nIsoValues; j++)
	{
		SetColor(j, listIsoValue[j], .3);
		_pModel->DrawContourBoxplots(nTimeStartIndex, j);
	}
}

void EnsembleLayer::drawVariabilityPlots(bool bCluster) {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = _pModel->GetIsoValues().length();
	//int nTimeSteps = (g_nTimeEnd - g_nTimeStart) / 6 + 1;
	int nTimeStartIndex = GlobalSetting::g_nTimeStart / 6;
	for (size_t j = 0; j < nIsoValues; j++)
	{
		SetColor(j, listIsoValue[j], .3);
		_pModel->DrawVariabilityPlots(bCluster, nTimeStartIndex, j);
	}
}

void EnsembleLayer::drawContourProbabilityPlots() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = _pModel->GetIsoValues().length();
	//int nTimeSteps = (g_nTimeEnd - g_nTimeStart) / 6 + 1;
	int nTimeStartIndex = GlobalSetting::g_nTimeStart / 6;
	for (size_t j = 0; j < nIsoValues; j++)
	{
		SetColor(j, listIsoValue[j], .3);
		_pModel->DrawContourProbabilityPlots(nTimeStartIndex, j);
	}
}

void EnsembleLayer::drawIsoContourDensityPlots() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = _pModel->GetIsoValues().length();
	int nTimeStartIndex = GlobalSetting::g_nTimeStart / 6;
	for (size_t j = 0; j < nIsoValues; j++)
	{
		_pModel->DrawIsoContourDensityPlots(nTimeStartIndex, j);
	}
}

void EnsembleLayer::drawContourLineOutlier()
{
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			_pModel->DrawOutliers(isoIndex);
		}
	}
}

void EnsembleLayer::drawContourLineMin() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			glColor4f(0.0, 1.0, 1.0, .8);
			drawContourLine(_pModel->GetContourMin(isoIndex));
		}
	}
}

void EnsembleLayer::drawContourLineMax() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			glColor4f(0.0, 1.0, 1.0, .8);
			drawContourLine(_pModel->GetContourMax(isoIndex));
		}
	}
}

void EnsembleLayer::drawContourLineMean() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			glColor4f(0.0, 1.0, 1.0, 1.0);
			drawContourLine(_pModel->GetContourMean(isoIndex));
		}
	}
}

void EnsembleLayer::drawContourLine(bool bCluster) {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			SetColor(isoIndex, listIsoValue[isoIndex]);
			_pModel->DrawContour(bCluster, isoIndex);
		}
	}
}

void EnsembleLayer::drawContourLineSorted() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			SetColor(isoIndex, listIsoValue[isoIndex]);
			_pModel->DrawContourSorted(isoIndex);
		}
	}
}

void EnsembleLayer::drawContourLineSortedSDF() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			SetColor(isoIndex, listIsoValue[isoIndex]);
			_pModel->DrawContourSortedSDF(isoIndex);
		}
	}
}

void EnsembleLayer::drawContourLineResampled(bool bCluster) {
	if (bCluster)
	{
		QList<double> listIsoValue = _pModel->GetIsoValues();
		int nIsoValues = listIsoValue.length();
		if (nIsoValues != 1)
		{
			qDebug() << "EnsembleLayer::drawContourLineResampled, error";
			return;
		}

		_pModel->DrawContoursResampledForClusters();
	}
	else {
		QList<double> listIsoValue = _pModel->GetIsoValues();
		int nIsoValues = listIsoValue.length();
		for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
		{
			if (_arrIsoValueState[isoIndex])
			{
				SetColor(isoIndex, listIsoValue[isoIndex]);
				_pModel->DrawContourResampledSDF(isoIndex);
			}
		}
	}
}

void EnsembleLayer::drawContourLineDomainResampled() {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{

		if (_arrIsoValueState[isoIndex])
		{
			SetColor(isoIndex, listIsoValue[isoIndex]);

			_pModel->DrawContourResampled(isoIndex);
		}
	}
}

void EnsembleLayer::drawContourLineSDF(bool bCluster) {
	QList<double> listIsoValue = _pModel->GetIsoValues();
	int nIsoValues = listIsoValue.length();
	for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
	{
		if (_arrIsoValueState[isoIndex])
		{
			SetColor(isoIndex, listIsoValue[isoIndex]);
			_pModel->DrawContourSDF(bCluster,isoIndex);
		}
	}
}

void EnsembleLayer::drawEnConVisBands(bool bCluster) {
	if (bCluster)
	{
		QList<double> listIsoValue = _pModel->GetIsoValues();
		int nIsoValues = listIsoValue.length();
		if (nIsoValues != 1)
		{
			qDebug() << "EnsembleLayer::drawContourLineResampled, error";
			return;
		}

		_pModel->DrawEnConVisBands_C(0);
	}
	else {
		QList<double> listIsoValue = _pModel->GetIsoValues();
		int nIsoValues = listIsoValue.length();
		for (size_t isoIndex = 0; isoIndex < nIsoValues; isoIndex++)
		{
			if (_arrIsoValueState[isoIndex])
			{
				_pModel->DrawEnConVisBands(isoIndex);
			}
		}
	}
}