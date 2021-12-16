#include "VariabilityPlots.h"
#include "Switch.h"
#include "ContourGenerator.h"
#include "UnCertaintyArea.h"

#include "DataField.h"

#include <QDebug>

#include <MathTool.h>


VariabilityPlots::~VariabilityPlots() {

	delete[] _gridBeliefMax;
	delete[] _gridBeliefMin;
	for (size_t i = 0; i < g_nClusterMax; i++)
	{
		delete _arrGridBeliefMax[i];
		delete _arrGridBeliefMin[i];
		delete _arrGridMean[i];
	}

	for each (UnCertaintyArea* pArea in _listAreaBelief)
		delete pArea;


	for (size_t i = 0; i < g_nClusterMax; i++)
		for each (UnCertaintyArea* pArea in _listAreaBelief_C[i])
			delete pArea;


}

void VariabilityPlots::BuildTess() {

	// tess
	BandTesser tesser1;
	tesser1.Tess(_listAreaBelief);
	_listTesser.append(tesser1);

	if (_cpLabels)
	{
		for (size_t iC = 0; iC < _nClusters; iC++)
		{
			if (_arrClusterLen[iC] > 1)
			{		
				_arrTesser[iC].Tess(_listAreaBelief_C[iC]);
			}
		}
	}
}

void VariabilityPlots::generateBands() {
	// 6.generate bands
	if (_bInitialized) {
		generateContourImp(_listContourMinBelief, _listContourMaxBelief, _listAreaBelief);
	}
}

void VariabilityPlots::init() {


	_gridBeliefMax = new double[_nGrids];
	_gridBeliefMin = new double[_nGrids];

	for (size_t i = 0; i < g_nClusterMax; i++)
	{
		_arrGridBeliefMax[i] = new double[_nGrids];
		_arrGridBeliefMin[i] = new double[_nGrids];
		_arrGridMean[i] = new double[_nGrids];
	}


}

void VariabilityPlots::generateContours() {

	ContourGenerator::GetInstance()->Generate(_gridBeliefMin, _listContourMinBelief, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate(_gridBeliefMax, _listContourMaxBelief, _dbIsoValue, _nWidth, _nHeight);

}

void VariabilityPlots::doStatistics() {
	double* arrBuf = new double[_nEnsembleLen];

	for (int i=0;i<_nGrids;i++)
	{
		for (int l = 0; l < _nEnsembleLen; l++)
		{
			arrBuf[l] = _pData->GetData(l, i);
		}
		double dbMean = 0;
		double dbVar = 0;
		MT::Statistics(arrBuf, _nEnsembleLen, dbMean, dbVar);	

		_gridBeliefMax[i] = dbMean + dbVar*1.5;
		_gridBeliefMin[i] = dbMean - dbVar*1.5;

	}

	delete[] arrBuf;
}

void VariabilityPlots::SetLabels(int nClusters,const int* cpLabels) {
	_cpLabels = cpLabels; 
	_nClusters = nClusters;
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		_arrClusterLen[_cpLabels[i]]++;
	}
	// do statistics
	for (int i = 0; i < _nGrids; i++)
	{
		double dbSum[g_nClusterMax] = { 0,0,0,0,0,0,0,0,0,0 };
		for (int l = 0; l < _nEnsembleLen; l++)
		{
			dbSum[_cpLabels[l]] += _pData->GetData(l, i);
		}
		for (size_t iC = 0; iC < _nClusters; iC++)
		{
			_arrGridMean[iC][i] = dbSum[iC] / _arrClusterLen[iC];
		}

		double dbVarSum[g_nClusterMax] = { 0,0,0,0,0,0,0,0,0,0 };
		for (int l = 0; l < _nEnsembleLen; l++)
		{
			double dbBias = _pData->GetData(l, i) - _arrGridMean[_cpLabels[l]][i];
			dbVarSum[_cpLabels[l]] += dbBias * dbBias;
		}

		for (size_t iC = 0; iC < _nClusters; iC++)
		{
			double dbVar = sqrt(dbVarSum[iC] / _arrClusterLen[iC]);

			_arrGridBeliefMax[iC][i] = _arrGridMean[iC][i] + dbVar * 1.5;
			_arrGridBeliefMin[iC][i] = _arrGridMean[iC][i] - dbVar * 1.5;
		}
	}

	// generate contours
	if (_bInitialized) {
		for (size_t iC = 0; iC < _nClusters; iC++)
		{
			ContourGenerator::GetInstance()->Generate(_arrGridBeliefMin[iC], _listContourMinBelief_C[iC], _dbIsoValue, _nWidth, _nHeight);
			ContourGenerator::GetInstance()->Generate(_arrGridBeliefMax[iC], _listContourMaxBelief_C[iC], _dbIsoValue, _nWidth, _nHeight);
			ContourGenerator::GetInstance()->Generate(_arrGridMean[iC], _listContourMean_C[iC], _dbIsoValue, _nWidth, _nHeight);
			if (_arrClusterLen[iC] > 1)
			{
				generateContourImp(_listContourMinBelief_C[iC], _listContourMaxBelief_C[iC], _listAreaBelief_C[iC]);
			}
		}
	}
}

void VariabilityPlots::DrawCluster(int nIndex) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	setOpacity(_scdbBandOpacity);
	_arrTesser[nIndex].Draw();


	setOpacity(1.0);
	glLineWidth(_arrClusterLen[nIndex]/5.0);
	for each (ContourLine contour in _listContourMean_C[nIndex])
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


	glPopAttrib();
}

void VariabilityPlots::draw() {

}