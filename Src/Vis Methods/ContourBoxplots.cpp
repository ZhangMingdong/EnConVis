#include "ContourBoxplots.h"
#include "Switch.h"
#include "ContourGenerator.h"
#include "UnCertaintyArea.h"
#include "DataField.h"

#include <QDebug>

ContourBoxplots::~ContourBoxplots() {

	delete[] _gridHalfMax;
	delete[] _gridHalfMin;
	delete[] _gridValidMax;
	delete[] _gridValidMin;
	for each (UnCertaintyArea* pArea in _listAreaValid)
		delete pArea;
	for each (UnCertaintyArea* pArea in _listAreaHalf)
		delete pArea;

	delete[] _pUpperSet;
	delete[] _pSetBandDepth;
	delete[] _pMemberType;

}

void ContourBoxplots::BuildTess() {

	// tess
	BandTesser tesser1, tesser2;
	tesser1.Tess(_listAreaValid);
	tesser2.Tess(_listAreaHalf);

	_listTesser.append(tesser1);
	_listTesser.append(tesser2);
}

void ContourBoxplots::generateBands() {
	// 6.generate bands
	if (_bInitialized) {
		generateContourImp(_listContourMinValid, _listContourMaxValid, _listAreaValid);
		generateContourImp(_listContourMinHalf, _listContourMaxHalf, _listAreaHalf);
	}
}

void ContourBoxplots::init() {

	_gridHalfMax = new double[_nGrids];
	_gridHalfMin = new double[_nGrids];
	_gridValidMax = new double[_nGrids];
	_gridValidMin = new double[_nGrids];

	_pUpperSet = new bool[_nGrids*_nEnsembleLen];
	_pSetBandDepth = new int[_nEnsembleLen];
	_pMemberType = new int[_nEnsembleLen];
}

void ContourBoxplots::generateContours() {

	ContourGenerator::GetInstance()->Generate(_gridValidMin, _listContourMinValid, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate(_gridValidMax, _listContourMaxValid, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate( _gridHalfMin, _listContourMinHalf, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate(_gridHalfMax, _listContourMaxHalf, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate(_pData->GetData(_nMedianIndex), _listContourMedian, _dbIsoValue, _nWidth, _nHeight);

	// outliers
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		if (_pMemberType[i]==0)
		{
			QList<ContourLine> contour;
			ContourGenerator::GetInstance()->Generate(_pData->GetLayer(i), contour, _dbIsoValue, _nWidth, _nHeight);
			_listContourOutlier.push_back(contour);
		}
	}
}

void ContourBoxplots::doStatistics() {
	// 1.calculate set
	calculateLevelSet();

	// 2.calculate band depth
	calculateBandDepth();

	// 3.calculate member type
	calculateMemberType();
	
	// 4.calculate median and min, max
	calculateStatistics();
}

void ContourBoxplots::calculateStatistics() {

	// 1.calculate the border fields of valid and half
	for (int i = 0; i < _nGrids; i++) {
		std::vector<double> vecDataValid;
		std::vector<double> vecDataHalf;
		// 1.calculate sets of valid and half
		for (int j = 0; j < _nEnsembleLen; j++)
		{
			if (_pMemberType[j])
			{
				vecDataValid.push_back(_pData->GetData(j, i));
				if (_pMemberType[j] == 2)
				{
					vecDataHalf.push_back(_pData->GetData(j, i));
				}
			}
		}
		// 2.calculate min and max of the two sets
		std::sort(vecDataValid.begin(), vecDataValid.end());
		std::sort(vecDataHalf.begin(), vecDataHalf.end());
		_gridHalfMin[i] = vecDataHalf[0];
		_gridHalfMax[i] = vecDataHalf[vecDataHalf.size() - 1];
		_gridValidMin[i] = vecDataValid[0];
		_gridValidMax[i] = vecDataValid[vecDataValid.size() - 1];

	}

	// 2.find median
	int nMaxDepth = 0;
	_nMedianIndex = -1;
	for (int i = 0; i < _nEnsembleLen; i++) {
		if (_pSetBandDepth[i] > nMaxDepth)
		{
			nMaxDepth = _pSetBandDepth[i];
			_nMedianIndex = i;
		}
	}
}

/*
	calculate type of each member: 0-outlier; 1-100%; 2-50%
	using SetBandDepth
*/
void ContourBoxplots::calculateMemberType() {
	// 1.initializ the type of member all to 1
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		_pMemberType[i] = 1;
	}

	// 2.find outlier
	int nOutliers = 0;
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		if (_pSetBandDepth[i] < _nOutlierThreshold) {
			_pMemberType[i] = 0;
			nOutliers++;
		}
	}

	// 3.find valid and 50%
	int nValidHalfLen = (_nEnsembleLen - nOutliers) / 2;
	for (int i = 0; i < nValidHalfLen; i++)
	{
		int nMaxDepth = 0;
		int nIndex = -1;
		for (size_t j = 0; j < _nEnsembleLen; j++)
		{
			if (_pMemberType[j] == 1 && _pSetBandDepth[j] > nMaxDepth)
			{
				nIndex = j;
				nMaxDepth = _pSetBandDepth[j];
			}
		}
		_pMemberType[nIndex] = 2;
	}
}

void ContourBoxplots::calculateLevelSet() {
	// 1.calculate set
	for (int l = 0; l < _nEnsembleLen; l++)
	{
		for (int i = 0; i < _nGrids; i++) {
			_pUpperSet[l*_nGrids + i] = (_pData->GetData(l, i) > _dbIsoValue);
		}
	}
}

/*
	caclulate sBand depth
	using UpperSet;
*/
void ContourBoxplots::calculateBandDepth() {
	// the length of the set is C(m-2,2)= (m-1)(m-2)/2, combination of each pair of ensmeble excep the current one
	int mSet = (_nEnsembleLen - 1) * (_nEnsembleLen - 2) / 2;
	int nMatrixLength = _nEnsembleLen * mSet;
	int* arrMatrix = new int[nMatrixLength];					// the matrix

	// 1.calculate the matrix, n * C(n,2)
	for (size_t i = 0; i < _nEnsembleLen; i++) {					// for each member
		int nSetIndex = 0;										// index of current set
		for (size_t j = 0; j < _nEnsembleLen; j++) {
			if (i == j) continue;
			for (size_t k = 0; k < j; k++) {
				if (i == k || j == k) continue;
				// for each set

				int nBreaks = 0;				// count of breaks(i not between j and k)
				for (int l = 0; l < _nGrids; l++)
				{
					// check union
					if ((_pUpperSet[i*_nGrids + l] && !_pUpperSet[j*_nGrids + l] && !_pUpperSet[k*_nGrids + l])) nBreaks++;
					// check intersection
					if ((!_pUpperSet[i*_nGrids + l] && _pUpperSet[j*_nGrids + l] && _pUpperSet[k*_nGrids + l])) nBreaks++;
				}
				arrMatrix[i*mSet + nSetIndex] = nBreaks;
				nSetIndex++;
			}
		}
	}

	// 2.calculate epsilon
	double dbThreshold = nMatrixLength / 6.0;
	/*
		'6' is from "Contour Boxplots: A Method for Characterizing Uncertainty in Feature Sets from Simulation Ensembles"
		number of breaks that more than 1/6 grid points have can be though as threshold.
	*/
	int epsilon = 1;				// breaks less than epsilon can be considered as be sandwiched
	while (true) {	// break when find a proper epsilon
		int nCount = 0;
		for (size_t i = 0; i < nMatrixLength; i++)
		{
			if (arrMatrix[i] < epsilon) nCount++;
		}
		if (nCount > dbThreshold) break;
		else epsilon++;
	}

	// 3.claculate band depth
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		_pSetBandDepth[i] = 0;
		for (size_t j = 0; j < mSet; j++) {
			if (arrMatrix[i*mSet + j] < epsilon) _pSetBandDepth[i]++;
		}
	}


	delete[] arrMatrix;
}

void ContourBoxplots::draw() {

	// draw median
	glColor4f(1, 1, 0, _scdbLineOpacity);
	for each (ContourLine contour in _listContourMedian)
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

	DrawOutliers();

}

void ContourBoxplots::DrawOutliers() {
	// draw outliers
	glPushAttrib(GL_ENABLE_BIT);

	glColor4f(1, 0, 0, _scdbLineOpacity);
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);


	for each (QList<ContourLine> contours in _listContourOutlier)
	{
		for each (ContourLine contour in contours)
		{
			drawContour(contour);
		}
	}
	glPopAttrib();
}