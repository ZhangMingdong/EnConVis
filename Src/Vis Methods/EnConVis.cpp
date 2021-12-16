#include "EnConVis.h"
#include "ContourGenerator.h"
#include "DataField.h"
#include "ColorMap.h"
#include "Switch.h"
#include <MathTool.h>
#include <QDebug>


#include<MathTypes.hpp>

#include "Timer.h"

bool g_bTest = false;

double CalcL2Dis(const double* buf1, const double *buf2, int nLen) {
	double dbSum = 0;
	for (size_t i = 0; i < nLen; i++)
	{
		double dbBias = buf1[i] - buf2[i];
		dbSum += dbBias;
	}
	return sqrt(dbSum);
}

EnConVis::EnConVis()
{
}

EnConVis::~EnConVis()
{
	for each (QList<UnCertaintyArea*> area in _listArea)
		for each (UnCertaintyArea* part in area) 
			delete part;

	for (size_t i = 0; i < g_nClusterMax; i++)
		for each (QList<UnCertaintyArea*> area in _listArea_C[i])
			for each (UnCertaintyArea* part in area)
				delete part;

	if(_pSortedBuf) delete[] _pSortedBuf;

	if(_pResampledBuf) delete[] _pResampledBuf;


	if (_arrResampledBuf_C) delete[]_arrResampledBuf_C;
}

void EnConVis::doStatistics()
{
}

void EnConVis::generateBands()
{
	if (_bInitialized) {
		int nMid = c_nResampleLen / 2;
		// from inner to outer
		for (int i = 1; i < nMid; i++)
		{
			QList<UnCertaintyArea*> area;
			generateContourImp(_listContourResampled[nMid - i], _listContourResampled[nMid + i], area);
			_listArea.append(area);
		}
	}
}

void EnConVis::generateContours()
{
	// 1.generate spaghetti and sorted
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		// sorted
		{
			QList<ContourLine> contour;
			ContourGenerator::GetInstance()->Generate(getSortedData(i), contour, _dbIsoValue, _nWidth, _nHeight);
			_listContourSorted.push_back(contour);
		}
	}

	// 2.generate resampled contours
	for (size_t i = 0; i < c_nResampleLen; i++)
	{
		// contours from resampled SDF
		QList<ContourLine> contour;
		ContourGenerator::GetInstance()->Generate(getResampledData(i), contour, _dbIsoValue, _nWidth, _nHeight);
		_listContourResampled.push_back(contour);
	}
}

void EnConVis::draw()
{
}

void EnConVis::init()
{
	_pSortedBuf = new double[_nGrids * _nEnsembleLen];
	_pResampledBuf = new double[_nGrids * c_nResampleLen];

	// 1.sort buffer
	sortBuf(_pData->GetData(), _pSortedBuf);


	// 5.Resample
	if (g_bResampleContours)
		resampleBuf(_pSortedBuf, _pResampledBuf, c_nResampleLen);

}

void EnConVis::DrawSortedContours(int nLevel) {
	setOpacity(_scdbLineOpacity);
	switch (nLevel)
	{
	case 0:
	{
		int arrIndices[1] = { 24 };
		for each (int nIndex in arrIndices)
		{
			QList<ContourLine> contours = _listContourSorted[nIndex];
			for each (ContourLine contour in contours)
			{
				drawContour(contour);
			}
		}
	}
		break;
	case 1:
	{
		int arrIndices[3] = { 12,24,36 };
		for each (int nIndex in arrIndices)
		{
			QList<ContourLine> contours = _listContourSorted[nIndex];
			for each (ContourLine contour in contours)
			{
				drawContour(contour);
			}
		}
	}
		break;
	case 2:
	{
		int arrIndices[7] = { 6,12,18,24,30,36,42 };
		for each (int nIndex in arrIndices)
		{
			QList<ContourLine> contours = _listContourSorted[nIndex];
			for each (ContourLine contour in contours)
			{
				drawContour(contour);
			}
		}
	}
		break;
	case 3:
	{
		int arrIndices[15] = {3, 6,9,12,15,18,21,24,27,30,33,36,39,42,45 };
		for each (int nIndex in arrIndices)
		{
			QList<ContourLine> contours = _listContourSorted[nIndex];
			for each (ContourLine contour in contours)
			{
				drawContour(contour);
			}
		}
	}
		break;
	default:
		// draw all
		for each (QList<ContourLine> contours in _listContourSorted)
		{
			for each (ContourLine contour in contours)
			{
				drawContour(contour);
			}
		}
		break;
	}
}

void EnConVis::DrawResampledContours(int nLevel) {
	double dbOpacity = _scdbLineOpacity;
	for (size_t i = 1; i < nLevel; i++)
	{
		dbOpacity /= 1.2;
	}
	setOpacity(dbOpacity);
	drawResampledContours(_listContourResampled,0, c_nResampleLen, nLevel);
}

void EnConVis::DrawContoursResampledForClusters(int nLevel, int nCluster) {
	for (int i = 0; i < _nClusters; i++)
	{
		if (nCluster == 0 || nCluster == i + 1)
		{
			glColor4f(ColorMap::GetCategory10D(i, 0)
				, ColorMap::GetCategory10D(i, 1)
				, ColorMap::GetCategory10D(i, 2)
				, _scdbLineOpacity
			);		

			drawResampledContours(_listContourResampled_C[i],0, c_nResampleLen_C, nLevel);
		}
	}
}

void EnConVis::SetLabels(int nClusters, const int* cpLabels) {
	_nClusters = nClusters;
	_cpLabels = cpLabels;

	if (!_arrResampledBuf_C)
		_arrResampledBuf_C = new double[_nGrids * nClusters * c_nResampleLen_C];


	resampleBuf_C(_pData->GetData(), cpLabels, _arrResampledBuf_C, nClusters, c_nResampleLen_C);


	if (g_bResampleForClusters)	GenerateContoursForCluster();

}

void EnConVis::drawResampledContours(const QList<QList<ContourLine>>& contours, int nIndex0,int nIndex1,int nLevel) {
	if (nIndex1 > nIndex0) {
		int nMedian = (nIndex1 + nIndex0) / 2;
		drawContours(contours[nMedian]);
		if (nLevel > 0) {
			drawResampledContours(contours,nIndex0, nMedian, nLevel - 1);
			drawResampledContours(contours,nMedian + 1, nIndex1, nLevel - 1);
		}
	}
}

void EnConVis::GenerateContoursForCluster() {
	// 1.generate contours
	for (int l = 0; l < _nClusters; l++)
	{
		for (int i = 0; i < c_nResampleLen_C; i++)
		{
			QList<ContourLine> contour;
			ContourGenerator::GetInstance()->Generate(getResampledForCluster(l, i), contour, 0, _nWidth, _nHeight);
			_listContourResampled_C[l].push_back(contour);
		}
	}

	// 2.generate regions
	if (_bInitialized) {
		// generate bands
		int nMid = c_nResampleLen_C / 2;
		// from inner to outer
		for (int i = 1; i < nMid; i++)
		{
			for (int j = 0; j < _nClusters; j++)
			{
				QList<UnCertaintyArea*> area;
				generateContourImp(_listContourResampled_C[j][nMid - i], _listContourResampled_C[j][nMid + i], area);
				_listArea_C[j].append(area);
			}
		}
	}

	// 3.generate representatives
	for (int i = 0; i < _nClusters; i++)
	{
		const double* arrMedian = getResampledForCluster(i, c_nResampleLen_C / 2);
		_arrRepresentativeIndices_C[i] = -1;
		double dbDis = 0;
		for (int j = 0; j < _nEnsembleLen; j++)
		{
			if (_cpLabels[j] ==i)
			{
				double dbNewDis = CalcL2Dis(arrMedian, _pData->GetData(j),_nGrids);
				if (_arrRepresentativeIndices_C[i] == -1 || dbNewDis < dbDis) {
					dbDis = dbNewDis;
					_arrRepresentativeIndices_C[i] = j;
				}
			}
		}
	}
}

void EnConVis::BuildTess() {
	for each (QList<UnCertaintyArea*> area in _listArea)
	{
		BandTesser tesser;
		tesser.Tess(area);
		_listTesser.append(tesser);
	}
	
	if (_nClusters) {
		for (size_t iC = 0; iC < _nClusters; iC++)
		{
			for each (QList<UnCertaintyArea*> area in _listArea_C[iC])
			{
				BandTesser tesser;
				tesser.Tess(area);
				_arrTesser_C[iC].append(tesser);
			}
		}
	}
}

// get start and step given level
void GetStartAndStep(int&nStart, int&nStep, int nLevel) {
	/*
	// for 15
	switch (nLevel)
	{
	case 0:
		return;
	case 1:
		nStart = 3;
		nStep = 4;
		break;
	case 2:
		nStart = 1;
		nStep = 2;
		break;
	default:
		nStart = 0;
		nStep = 1;
		break;
	}
	*/
	switch (nLevel)
	{
	case 1:
		nStart = 15;
		nStep = 16;
		break;
	case 2:
		nStart = 7;
		nStep = 8;
		break;
	case 3:
		nStart = 3;
		nStep = 4;
		break;
	case 4:
		nStart = 1;
		nStep = 2;
		break;
	case 5:
		nStart = 0;
		nStep = 1;
		break;
	default:	// 0
		return;
	}
}

void EnConVis::DrawBands(int nLevel) {
	int nRegionSize = pow(2, nLevel-1);
	double dbOpacity = getBandOpacity(nRegionSize);

	int nIndex = 0;
	glColor4f(ColorMap::GetCategory10D(nIndex, 0)
		, ColorMap::GetCategory10D(nIndex, 1)
		, ColorMap::GetCategory10D(nIndex, 2)
		, dbOpacity);

//	_listTesser[_listTesser.length() - 1].Draw();

	int nStart, nStep;
	GetStartAndStep(nStart, nStep, nLevel);
	for (size_t i = nStart,length=_listTesser.length()-1; i < length; i+= nStep) _listTesser[i].Draw();




}

void EnConVis::DrawBands_C(int nLevel, int nCluster) {
	// calculate opacity
	/*
	double dbOpacity = _scdbBandOpacity;
	for (size_t i = 1; i < nLevel; i++)
	{
		dbOpacity /= 1.5;
	}
	*/
	int nRegionSize = pow(2, nLevel - 1);
	double dbOpacity = getBandOpacity(nRegionSize);

	// calculate level
	int nStart, nStep;


	GetStartAndStep(nStart, nStep, nLevel);
	for (int i = 0; i < _nClusters; i++)
	{
		bool bTempEffect = true;
		if (bTempEffect) {
			// these lines are temply used for effect

			// 20201208
			switch (i) {
			case 0:
				nLevel = 4;
				break;
			case 1:
				nLevel = 2;
				break;
			case 2:
				nLevel = 2;
				break;
			case 3:
				nLevel = 2;
				break;
			default:
				nLevel = 0;
				break;
			}
			GetStartAndStep(nStart, nStep, nLevel);
		}
		
		// end
		if (nCluster == 0 || nCluster == i + 1)
		{
			glColor4f(ColorMap::GetCategory10D(i, 0)
				, ColorMap::GetCategory10D(i, 1)
				, ColorMap::GetCategory10D(i, 2)
				, dbOpacity
			);

			for (size_t j = nStart, length = _arrTesser_C[i].length(); j < length; j += nStep)
			{
				_arrTesser_C[i][j].Draw();
			}



			// draw representatives
			glColor4f(ColorMap::GetCategory10D(i, 0)
				, ColorMap::GetCategory10D(i, 1)
				, ColorMap::GetCategory10D(i, 2)
				, 1
			);
			glLineWidth(3.0);
//			drawContours(_listContour[_arrRepresentativeIndices_C[i]]);

		}
	}



}

void EnConVis::sortBuf(const double* pS, double* pD) {
	for (int i = 0; i < _nGrids; i++) {								// i: index of grid points
		for (int j = 0; j < _nEnsembleLen; j++) {					// j: index of ensemble member
			double dbValue = pS[j*_nGrids + i];						// dbValue: get value of ensemble j
			int k = 0;												// k: finds the first value bigger than dbValue at k
			while (k < j && pD[k*_nGrids + i] < dbValue) k++;
			int l = j;
			while (l > k) {											// move the value behind k one step further
				pD[l*_nGrids + i] = pD[(l - 1)*_nGrids + i];
				l--;
			}
			pD[k*_nGrids + i] = dbValue;							// set new value at k
		}
	}
	// vector version
	/*
		for (int i = 0; i < _nGrids; i++) {
		std::vector<double> vecValues;
		for (int j = 0; j < _nEnsembleLen; j++) {
			vecValues.push_back(pS[j*_nGrids + i]);
		}
		std::sort(vecValues.begin(), vecValues.end());
		for (int j = 0; j < _nEnsembleLen; j++) {
			pD[j*_nGrids + i] = vecValues[j];
		}
	}
	*/
}

const double c_dbK = 1.0 / sqrt(PI2d);

inline double KernelFun(double para) {
	return c_dbK * exp(-para * para / 2.0);
}

/*
	rule of thumb formula from paper 'Uncertainty-Aware Multidimensional Ensemble Data Visualization and Exploration'
*/
void resampleBasedOnKDE(double* dbInput, double* dbOutput, int nInputLen, int nOutputLen) {
	double dbMean = 0;
	double dbVar = 0;
	MT::Statistics(dbInput, nInputLen, dbMean, dbVar);
	
	if (dbVar == 0) dbVar = .00000001;	// in case there's only one member for clusters

	//double dbH = dbVar * pow(4.0 / 3.0 / nInputLen, -1.0 / 5);

	double dbH = 1.06*dbVar * pow(nInputLen, -1.0 / 5);
	dbH /= 100.0;

	/*
	if (g_bTest) {
		qDebug() << "variance" << dbVar;
		for (size_t i = 0; i < nInputLen; i++)
		{
			qDebug() << dbInput[i];
		}
	}
	*/

	int resampleGridLen = 1000;
	double dbRange = dbInput[nInputLen - 1] - dbInput[0] + 6 * dbH;
	double dbMin = dbInput[0] - dbH * 3;
	double dbMax = dbMin + dbRange;
	double dbStep = dbRange / (resampleGridLen + 1);

	// 0.allocate resource
	double* pDensity = new double[resampleGridLen];		// density field

	//if (g_bTest) qDebug() << "density";
	// 1.calculate density function
	double dbAccum = 0.0;								// accumulated density
	for (int i = 0; i < resampleGridLen; i++)
	{
		double x = dbMin + dbStep * i;
		double y = 0;
		for (int j = 0; j < nInputLen; j++)
		{
			y += KernelFun((x - dbInput[j]) / dbH) / dbH;
		}
		pDensity[i] = y;
		//if (g_bTest) qDebug() << y;
		dbAccum += y;
	}
	/*
	if (g_bTest) qDebug() << "per1:" << pDensity[0] / dbAccum;
	if (g_bTest) qDebug() << "per2:" << pDensity[resampleGridLen-1] / dbAccum;
	*/

	double dbThreshold = 0.0001;

	// 2.resampling
	double dbCurrentAccum = 0.0;							// current accumulated density
	double dbDensityStep = dbAccum / (nOutputLen - 1);		// steps for the result density
	for (int i = 0, j = 0; i < resampleGridLen; i++)
	{
		double x = dbMin + dbStep * i;
		dbCurrentAccum += pDensity[i];

		//if (bShow) qDebug() << dbCurrentAccum << ",j:" << j << ",accum:" << j * dbDensityStep;
		if (j == 0) {
			if (dbCurrentAccum > dbThreshold) {

				dbOutput[j] = x;
				//if (bShow) qDebug() << x;
				j++;
			}
		}
		else if (j == nOutputLen - 1) {
			if (dbCurrentAccum < 1 - dbThreshold)
			{
				dbOutput[j] = x;
				break;
			}
		}
		else if (dbCurrentAccum > j*dbDensityStep) {
			dbOutput[j] = x;
			//if (bShow) qDebug() << x;
			j++;
		}
		if (j == nOutputLen)
		{
			break;
		}
	}

	//if (g_bTest) qDebug() << "\n";
	// 3.clean
	delete[] pDensity;
}

void resampleBasedOnUniform(double* dbInput, double* dbOutput, int nInputLen, int nOutputLen) {
	int resampleGridLen = 1000;
	double dbMin = dbInput[0];
	double dbMax = dbInput[nInputLen - 1];
	double dbRange = dbMax - dbMin;
	double dbStep = dbRange / (nOutputLen - 1);

	// 2.resampling
	for (int i = 0; i < nOutputLen; i++)
	{
		dbOutput[i] = dbMin + i * dbStep;
	}
}

// fix step of gaussian sampling, from m-3v(0.3%) to m+3v(99.7%);
void resampleBasedOnGaussian(double* dbInput, double* dbOutput, int nInputLen, int nOutputLen) {
	int resampleGridLen = 1000;


	// 1.calculate gaussian (m and v)
	double dbMean = 0;
	double dbVar = 0;
	MT::Statistics(dbInput, nInputLen, dbMean, dbVar);
	double dbVar2 = dbVar * dbVar;


	double dbMin = dbMean - 3 * dbVar;
	double dbMax = dbMean + 3 * dbVar;
	double dbRange = dbMax - dbMin;

	double dbStep = dbRange / (resampleGridLen + 1);

	// 0.allocate resource
	double* pDensity = new double[resampleGridLen];		// density field

	// 2.calculate density function

	double dbAccum = 0.003;								// accumulated density
	for (size_t i = 0; i < resampleGridLen; i++)
	{
		double x = dbMin + dbStep * i;
		double dbBias = x - dbMean;
		double y = exp(-dbBias * dbBias / 2 / dbVar2) / sqrt(PI2d) / dbVar;
		pDensity[i] = y;
		dbAccum += y;
	}

	// 3.resampling
	double dbCurrentAccum = 0.0;							// current accumulated density
	double dbDensityStep = dbAccum / (nOutputLen + 1);		// steps for the result density
	for (size_t i = 0, j = 0; i < resampleGridLen; i++)
	{
		double x = dbMin + dbStep * i;
		dbCurrentAccum += pDensity[i];


		//if (bShow) qDebug() << dbCurrentAccum << ",j:" << j << ",accum:" << j * dbDensityStep;
		if (dbCurrentAccum > (j + 1)*dbDensityStep) {
			dbOutput[j] = x;
			//if (bShow) qDebug() << x;
			j++;
		}
		if (j == nOutputLen)
		{
			break;
		}
	}

	// 3.clean
	delete[] pDensity;
}

// resample buffer
void EnConVis::resampleBuf(const double* pSortedBuf, double* pResampledBuf, int nResampleLen) {
	double* dbSample = new double[_nEnsembleLen];
	double* dbResample = new double[nResampleLen];

	// calculate resampled SDF for each grid point
	for (int i = 0; i < _nGrids; i++)
	{
		// 1.set the buffer for resample
		for (size_t l = 0; l < _nEnsembleLen; l++)
		{
			dbSample[l] = pSortedBuf[l*_nGrids + i];
		}

		if (i == 15 * _nWidth + 20) g_bTest = true;
		//if (i < 1000) g_bTest = true;

		// 2.resample


		resampleBasedOnKDE(dbSample, dbResample, _nEnsembleLen, nResampleLen);
		//resampleBasedOnGaussian(dbSample, dbResample, _nEnsembleLen, nResampleLen);
		//resampleBasedOnUniform(dbSample, dbResample, _nEnsembleLen, nResampleLen);

		g_bTest = false;

		// 3.record the sampled buffer
		for (size_t l = 0; l < nResampleLen; l++)
		{
			pResampledBuf[l*_nGrids + i] = dbResample[l];
		}
	}

	delete[] dbSample;
	delete[] dbResample;
}

// resample buffer
void EnConVis::resampleBuf_C(const double* pUnSortedBuf, const int* pLabels, double* pResampledBuf, int nClusters, int nResampleLen) {
	double* dbSample = new double[_nEnsembleLen];
	double* dbResample = new double[nResampleLen];


	for (int indexC = 0; indexC < nClusters; indexC++) {	// foreach Cluster

		// 0.get index list of this cluster
		std::vector<int> vecClusterMemberIndex;
		for (int l = 0; l < _nEnsembleLen; l++)
		{
			if (pLabels[l] == indexC) vecClusterMemberIndex.push_back(l);
		}

		// 1.calculate resampled SDF for each grid point
		for (int i = 0; i < _nGrids; i++)
		{
			int nClusterMemberLen = vecClusterMemberIndex.size();

			std::vector<double> vecValues;
			for (int l = 0; l < nClusterMemberLen; l++)
			{
				vecValues.push_back(pUnSortedBuf[vecClusterMemberIndex[l] * _nGrids + i]);
			}
			std::sort(vecValues.begin(), vecValues.end());

			for (int l = 0; l < nClusterMemberLen; l++)
			{
				dbSample[l] = vecValues[l];
			}

			resampleBasedOnKDE(dbSample, dbResample, nClusterMemberLen, nResampleLen);

			for (int l = 0; l < nResampleLen; l++)
			{
				pResampledBuf[l*_nGrids + i] = dbResample[l];
			}
		}
		pResampledBuf += _nGrids * nResampleLen;
	}


	delete[] dbSample;
	delete[] dbResample;
}

