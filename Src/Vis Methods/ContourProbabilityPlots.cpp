#include "ContourProbabilityPlots.h"
#include "Switch.h"
#include "ContourGenerator.h"
#include "UnCertaintyArea.h"

#include "DataField.h"

#include <QDebug>

#include <MathTool.h>



ContourProbabilityPlots::~ContourProbabilityPlots() {

	delete[] _gridUpperPercentage;
	for each (QList<UnCertaintyArea*> listRegions in _arrlistRegions)
	{
		for each (UnCertaintyArea* pArea in listRegions)
			delete pArea;

	}
}

void ContourProbabilityPlots::BuildTess() {

	// tess
	for each (QList<UnCertaintyArea*> listRegions in _arrlistRegions)
	{
		BandTesser tesser1;
		tesser1.Tess(listRegions);
		_listTesser.append(tesser1);
	}
}

void ContourProbabilityPlots::generateBands() {
	//return;
	// 6.generate bands
	if (g_bCPP) {
		for (int i = 0; i < c_nContours / 2; i++)
		{
			generateContourImp(_arrlistContours[i]
				, _arrlistContours[c_nContours - 1 - i]
				, _arrlistRegions[i]
				, _nSubdividedW, _nSubdividedH);
			if (_bSubdividedGrids)
			{
				for each (UnCertaintyArea * pRegion in _arrlistRegions[i])
					pRegion->Scale(1.0 / _nSubdivideNumber, 1.0 / _nSubdivideNumber);
			}
		}
		if (_bSubdividedGrids)
		{
			for (int j = 0; j < c_nContours; j++)
			{
				for (int i = 0, length = _arrlistContours[j].size(); i < length; i++)
				{
					_arrlistContours[j][i].Scale(1.0 / _nSubdivideNumber, 1.0 / _nSubdivideNumber);
				}
	
			}
		}
	}
}

void ContourProbabilityPlots::init() {

	if (_bSubdividedGrids) {
		_nSubdividedW = (_nWidth - 1) * _nSubdivideNumber + 1;
		_nSubdividedH = (_nHeight - 1) * _nSubdivideNumber + 1;
		_gridUpperPercentage = new double[_nSubdividedW*_nSubdividedH];
	}
	else
	{
		_nSubdividedW = _nWidth;
		_nSubdividedH = _nHeight;
		_gridUpperPercentage = new double[_nGrids];
	}


}

void ContourProbabilityPlots::generateContours() {
	for (int i = 0; i < c_nContours; i++)
	{
		ContourGenerator::GetInstance()->Generate(_gridUpperPercentage, _arrlistContours[i], _arrIsovalues[i], _nSubdividedW, _nSubdividedH);
	}
}

void ContourProbabilityPlots::doStatistics() {
	if (_bSubdividedGrids) {
		for (int i = 0; i < _nSubdividedH; i++)
		{
			for (int j = 0; j < _nSubdividedW; j++)
			{
				int nIndex = i * _nSubdividedW + j;
				_gridUpperPercentage[nIndex] = 0;
				for (int l = 0; l < _nEnsembleLen; l++)
				{
					double dbV = MT::GetInterpolatedData(_pData->GetData(l), _nWidth, _nHeight, 
						j / (double)_nSubdivideNumber, i / (double)_nSubdivideNumber);
					_gridUpperPercentage[nIndex] += (dbV > _dbIsoValue ? 1 : 0);
				}
				_gridUpperPercentage[nIndex] /= _nEnsembleLen;
				//_gridUpperPercentage[nIndex] = (double)i / _nSubdividedH;
			}
		}
	}
	else {

		for (int i = 0; i < _nGrids; i++)
		{
			_gridUpperPercentage[i] = 0;
			for (int l = 0; l < _nEnsembleLen; l++)
			{
				_gridUpperPercentage[i] += (_pData->GetData(l, i) > _dbIsoValue ? 1 : 0);
			}
			_gridUpperPercentage[i] /= _nEnsembleLen;
		//	{
		//		int row = i / _nWidth;
		//		_gridUpperPercentage[i] = (double)row / _nHeight;
		//	}
			//if(_gridUpperPercentage[i]>0) qDebug() << _gridUpperPercentage[i];
		}
	}
}

void ContourProbabilityPlots::draw() {
	return;

	// draw the region border
	glColor4f(0, 0, 0, _scdbLineOpacity);

	for (int i = 0; i < c_nContours; i++)
	{
		for each (ContourLine contour in _arrlistContours[i])
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
}