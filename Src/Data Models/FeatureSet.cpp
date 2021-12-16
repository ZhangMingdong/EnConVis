#include "FeatureSet.h"
#include "DataField.h"
#include "ContourGenerator.h"

#include "MyPCA.h"
#include <KMeansClustering.h>
#include <AHCClustering.h>

#include <qDebug>

#include <Eigen/Core>
#include <mathtoolbox/classical-mds.hpp>
#include <iostream>

#include "Switch.h"
#include "ColorMap.h"

#include <fstream>

#include <gl/GLU.h>

#include <MathTool.h>

#include "Timer.h"


const bool g_bDomainRCPlot = false;
const bool g_bClustering = true;

// use for test 95 data, add distance between ensembles
const bool bTest95 = true;

using namespace Eigen;

int FeatureSet::GetLabel(int l) { 
	if(g_bClustering)
		return _arrLabels[l];
	else return 0;
}

FeatureSet::FeatureSet(DataField* pData, double dbIsoValue, int nWidth, int nHeight, int nEnsembleLen):
	_pData(pData)
	,_dbIsoValue(dbIsoValue)
{

	MyTimer::Time("FeatureSet()");

	_nWidth = nWidth;
	_nHeight = nHeight;
	_nGrids = nWidth * nHeight;
	_nEnsembleLen = nEnsembleLen;
	_nClusters = GlobalSetting::g_nClusters;


	// 0.allocate resource
	_pICDVX = new double[_nGrids * _nEnsembleLen];
	_pICDVY = new double[_nGrids * _nEnsembleLen];
	_pICDVW = new double[_nGrids * _nEnsembleLen];
	_pICD_LineKernel = new double[_nGrids];
	_pICDX = new double[_nGrids];
	_pICDY = new double[_nGrids];
	_pICDZ = new double[_nGrids];
	_pICD = new double[((_nWidth - 1) * _nDetailScale + 1) * ((_nHeight - 1) * _nDetailScale + 1)];
	_pGridDiverse = new bool[_nGrids];


	_arrLabels = new int[_nEnsembleLen];
	for (size_t i = 0; i < _nEnsembleLen; i++) _arrLabels[i] = 0;
	_arrMergeTarget = new int[_nEnsembleLen];
	_arrMergeSource = new int[_nEnsembleLen];
	_arrPC = new double[_nEnsembleLen * _nPCLen];


	// 1.initialize domain EnConVis(it should be first generated for the spaghettis are from it	
	if (g_bDomainRCPlot) _rcPlotsDomain.Initialize(_nWidth, _nHeight, _nEnsembleLen, _dbIsoValue, _pData);

	_spaghettiPlot.Initialize(_nWidth, _nHeight, _nEnsembleLen, _dbIsoValue, _pData);


	MyTimer::Time("spaghettiplot initialized");

	// 2.generate contours of mean, min, and max
	if(g_bGenerateContours) generateContours();

	//MyTimer::Time("contour generated");


	// 3.SDFs, sorted SDFs, contours from SDFs and sorted SDFs
	if(g_bCalculateSDF)
		_pSDFData = _pData->GenerateSDF(_dbIsoValue, _nEnsembleLen, _nWidth, _nHeight, _spaghettiPlot.GetContours());


	MyTimer::Time("sdf generated");

	// output sdf
	if(false)
	{
		std::ofstream sdf_file("ensemble_X.txt");
		for (size_t i = 0; i < _nEnsembleLen; i++)
		{
			for (size_t j = 0; j < _nGrids; j++)
			{
				sdf_file << _pSDFData->GetData(i,j)<<" ";
			}
			sdf_file << "\n";
		}
	}

	// 4.initialize other vis method
	if (g_bCBP) _cbPlots.Initialize(_nWidth, _nHeight, _nEnsembleLen, 0, _pSDFData);
	MyTimer::Time("cbp initialized");
	if (g_bCVP) _vbPlots.Initialize(_nWidth, _nHeight, _nEnsembleLen, 0, _pSDFData);
	MyTimer::Time("cvp initialized");
	if (g_bCPP) _cpPlots.Initialize(_nWidth, _nHeight, _nEnsembleLen, 0, _pSDFData);
	MyTimer::Time("cpp initialized");
	if (g_bRCP) _rcPlotsSDF.Initialize(_nWidth, _nHeight, _nEnsembleLen, 0, _pSDFData);
	MyTimer::Time("rcp initialized");


	// 5. calculate iso-contour density
	if (g_bICD) {
		_cdPlots.Initialize(_nWidth, _nHeight, _nEnsembleLen, 0, _pSDFData);
		calculateICD();
		//buildICD_LineKernel();
		//buildICD_Vector();
		//buildICDV();
		MyTimer::Time("icd initialized");
	}


	// 6.Clustering
	if (g_bClustering)
	{

		// 10.1.calculate diversity
		calculateDiverse();
		// 10.2.calculate PCA

		//calculatePCA();
		//calculatePCA_Whole();
		//calculatePCA_MDS();
		//calculatePCA_MDS_Set();
		calculatePCA_MDS_Whole();
		//calculatePCA_MDS_Whole_Density();
		//calculatePCA_MutualInformation();
		//calculatePCA_Fofonov();

		// 10.3.clustering
		doPCAClustering();
		//doClustering();


		// output pcs
		if (false) outputPC();

		MyTimer::Time("clustered");

		// .5.set clustering result for the vis methods
		_spaghettiPlot.SetLabels(_nClusters, _arrLabels);
		//MyTimer::Time("spaghetti plot set labels");
		if (g_bCBP) _vbPlots.SetLabels(_nClusters, _arrLabels);
		MyTimer::Time("cvp set labels");
		if (g_bDomainRCPlot) _rcPlotsDomain.SetLabels(_nClusters, _arrLabels);
		//MyTimer::Time("domain rcplot set labels");
		if (g_bRCP) _rcPlotsSDF.SetLabels(_nClusters, _arrLabels);
		MyTimer::Time("rcp set labels");
			



		// output labels
		if (false) outputLabel();
	}


	//MyTimer::Time("clustered");

	//calculatePCABox();

	//calculatePCARecovery();
}

void FeatureSet::outputPC() {

	std::ofstream sdf_file("pc_X.txt");
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			sdf_file << _arrPC[i + _nPCLen + j] << " ";
		}
		sdf_file << "\n";
	}

}

void FeatureSet::outputLabel() {
	std::ofstream file_labels("ensemble_labels.txt");
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		file_labels << _arrLabels[i] + 1 << "\n";
	}

}

/*
	calculate similarity between two members;
	using mutual information
	write these function according to paper "Isosurface Similarity Maps"
	date: 2019/01/27
	paras:
		l1,l2: index of the two member
		min,max: min and max of the sdf
*/
double FeatureSet::calculateSimilarityMI(int l1, int l2,double dbMin, double dbMax) {
	const int nBin = 128;
	int arrBin[nBin][nBin];
	int arrBinX[nBin];
	int arrBinY[nBin];
	double dbRange = dbMax - dbMin + 1;

	for (size_t i = 0; i < nBin; i++) {
		arrBinX[i] = 0;
		arrBinY[i] = 0;
		for (size_t j = 0; j < nBin; j++) arrBin[i][j] = 0;
	}

	// calculate bin
	for (size_t i = 0; i < _nGrids; i++)
	{
		int nIndex1 = (_pSDFData->GetData(l1, i) - dbMin) / dbRange * 128;
		int nIndex2 = (_pSDFData->GetData(l2, i) - dbMin) / dbRange * 128;
		arrBin[nIndex1][nIndex2]++;
		arrBinX[nIndex1]++;
		arrBinY[nIndex2]++;
	}

	// calculate mutual information
	double dbHx = 0;
	double dbHy = 0;
	double dbHxy = 0;
	for (size_t i = 0; i < nBin; i++)
	{
		if (arrBinX[i]) {
			double dbPx = arrBinX[i] / (double)_nGrids;
			dbHx += dbPx * log(dbPx);
		}
		if(arrBinY[i]){
			double dbPy = arrBinY[i] / (double)_nGrids;
			dbHy += dbPy * log(dbPy);
		}
		for (size_t j = 0; j < nBin; j++)
		{
			if(arrBin[i][j]){
				double dbPxy = arrBin[i][j] / (double)_nGrids;
				dbHxy += dbPxy * log(dbPxy);
			}
		}
	}
	return -(dbHx + dbHy - dbHxy);
}

/*
	calculate distance between two members;
	using monte carlo
	date: 2019/12/28
	paras:
		l1,l2: index of the two member
	results:
		0-1;
*/
double FeatureSet::calculateDistanceMC(int l1, int l2) {
	unsigned long ulMCLen = 100000;
	int nUnion = 0;
	int nIntersection = 0;
	for (size_t i = 0; i < ulMCLen; i++)
	{
		double dbX = rand()*(_nWidth - 1) / (double)RAND_MAX;
		double dbY = rand()*(_nHeight - 1) / (double)RAND_MAX;

		double dbSDF1 = MT::GetInterpolatedData(_pSDFData->GetData(l1), _nWidth, _nHeight, dbX, dbY);
		double dbSDF2 = MT::GetInterpolatedData(_pSDFData->GetData(l2), _nWidth, _nHeight, dbX, dbY);

		if (dbSDF1 > 0 && dbSDF2 > 0)
			nIntersection++;
		if (dbSDF1 > 0 || dbSDF2 > 0)
			nUnion++;					   
	}
	return nIntersection / (double)nUnion;
}

FeatureSet::~FeatureSet()
{
	delete[] _pICDVX;
	delete[] _pICDVY;
	delete[] _pICDVW;
	delete[] _pICD_LineKernel;
	delete[] _pICDX;
	delete[] _pICDY;
	delete[] _pICDZ;
	delete[] _pICD;
	delete[] _pGridDiverse;
	delete[] _arrLabels;
	delete[] _arrMergeTarget;
	delete[] _arrMergeSource;

	delete[] _arrPC;

	if (_pSDFData) delete _pSDFData;
}

void FeatureSet::generateContours() {


	ContourGenerator::GetInstance()->Generate(_pData->GetUMin(), _listContourMinE, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate(_pData->GetUMax(), _listContourMaxE, _dbIsoValue, _nWidth, _nHeight);
	ContourGenerator::GetInstance()->Generate(_pData->GetMean(), _listContourMeanE, _dbIsoValue, _nWidth, _nHeight);


}

/*
	calculate pca using diverse grids
*/
void FeatureSet::calculatePCA() {
	// 1.set parameter
	int mI = _nDiverseCount;
	int mO = _nPCLen;
	int n = _nEnsembleLen;
	// 2.allocate input and output buffer
	double* arrInput = createDiverseArray();

	// 3.pca
	MyPCA pca;
	pca.DoPCA(arrInput, _arrPC, n, mI, mO, true);

	delete[] arrInput;
}

/*
	calculate pca using whole grids
*/
void FeatureSet::calculatePCA_Whole() {
	// 1.set parameter
	int mI = _nGrids;
	int mO = _nPCLen;
	int n = _nEnsembleLen;
	// 2.allocate input and output buffer
	//double* arrInput = _pSDF;
	double* arrInput = _pSDFData->GetData();

	// 3.pca
	MyPCA pca;

	unsigned long t1 = GetTickCount();

	pca.DoPCA(arrInput, _arrPC, n, mI, mO, true);
	unsigned long t2 = GetTickCount();
	qDebug() << "Time of calculatePCA_Whole: " << t2-t1;

	delete[] arrInput;
}

/*
	calculate PCA using MDS for the diverse grids
*/
void FeatureSet::calculatePCA_MDS() {

	// 2.allocate input buffer
	double* arrInput = createDiverseArray();

	int n = _nEnsembleLen;
	std::vector<VectorXd> points(n);
	for (size_t i = 0; i < n; i++)
	{
		points[i] = VectorXd(_nDiverseCount);
		for (size_t j = 0; j < _nDiverseCount; j++)
		{
			points[i](j) = arrInput[i*_nDiverseCount + j];
		}
	}

	MatrixXd D(n, n);
	for (unsigned i = 0; i < n; ++i)
	{
		for (unsigned j = i; j < n; ++j)
		{
			const double d = (points[i] - points[j]).norm();
			D(i, j) = d;
			D(j, i) = d;
		}
	}

	// Compute metric MDS (embedding into a 2-dimensional space)
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, _nPCLen);
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			_arrPC[i*_nPCLen + j] = X(j, i);
		}
	}

	for (size_t i = 0; i < n; i++)
	{
		qDebug() << X(0, i);
	}
	qDebug() << "======================";
	for (size_t i = 0; i < n; i++)
	{
		qDebug() << X(1, i);
	}
	delete[] arrInput;
}


/*
	calculate PCA using MDS
	using SDFs.
*/
void FeatureSet::calculatePCA_MDS_Whole() {

	int n = _nEnsembleLen;

	// 1.build vectors
	std::vector<VectorXd> points(n);
	for (size_t l = 0; l < n; l++)
	{
		points[l] = VectorXd(_nHeight*_nWidth);
		for (size_t i = 0; i < _nHeight; i++)
		{
			for (size_t j = 0; j < _nWidth; j++)
			{
				points[l](i*_nWidth + j) = _pSDFData->GetData(l, i, j);
			}
		}
	}

	// 2.calculate distance matrix of vectors
	MatrixXd D(n, n);
	for (unsigned i = 0; i < n; ++i)
	{
		for (unsigned j = i; j < n; ++j)
		{
			double d = (points[i] - points[j]).norm();
			if(bTest95)
			{
				if (
					!(
						(i < 50 && j < 50)
						|| (i > 49 && i < 64 && j>49 && j < 64)
						|| (i > 63 && i < 78 && j>63 && j < 78)
						|| (i > 77 && j > 77)
						)
					)
				{
					d += 1000;
				}
			}
			D(i, j) = d;
			D(j, i) = d;
		}
	}

	// 3.Compute metric MDS (embedding into a 2-dimensional space)	
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, _nPCLen);
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			_arrPC[i*_nPCLen + j] = X(j, i);
		}
	}
}

/*
	calculate PCA usinng MDS and density
*/
void FeatureSet::calculatePCA_MDS_Whole_Density() {
	int nClusterWidth = _nWidth;// / 4;
	int nStartX = 0;
	//nClusterWidth = 10;
	//nStartX = 20;
	qDebug() << "Width:" << _nWidth;
	int n = _nEnsembleLen;
	std::vector<VectorXd> points(n);
	for (size_t l = 0; l < n; l++)
	{
		points[l] = VectorXd(_nHeight*nClusterWidth*2);
		//X
		for (size_t i = 0; i < _nHeight; i++)
		{
			for (size_t j = 0; j < nClusterWidth; j++)
			{
				points[l](i*nClusterWidth + j) = _pICDVX[l*_nWidth*_nHeight + i * _nWidth + nStartX + j];
			}
		}
		//Y
		for (size_t i = 0; i < _nHeight; i++)
		{
			for (size_t j = 0; j < nClusterWidth; j++)
			{
				points[l](_nHeight*nClusterWidth+i*nClusterWidth + j) = _pICDVY[l*_nWidth*_nHeight + i * _nWidth + nStartX + j];
			}
		}
	}

	MatrixXd D(n, n);
	for (unsigned i = 0; i < n; ++i)
	{
		for (unsigned j = i; j < n; ++j)
		{
			const double d = (points[i] - points[j]).norm();
			D(i, j) = d;
			D(j, i) = d;
		}
	}

	// Compute metric MDS (embedding into a 2-dimensional space)
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, _nPCLen);
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			_arrPC[i*_nPCLen + j] = X(j, i);
		}
	}
	/*
	for (size_t i = 0; i < n; i++)
	{
		qDebug() << X(0, i);
	}
	qDebug() << "======================";
	for (size_t i = 0; i < n; i++)
	{
		qDebug() << X(1, i);
	}
	*/
}

/*
	calculate PCA using mutual information
*/
void FeatureSet::calculatePCA_MutualInformation() {
	// calculate the range of sdf;
	double dbMin, dbMax;
	_pSDFData->GetDataRange(dbMin, dbMax);

	MatrixXd D(_nEnsembleLen, _nEnsembleLen);
	for (unsigned i = 0; i < _nEnsembleLen; ++i)
	{
		for (unsigned j = i; j < _nEnsembleLen ; ++j)
		{
			double dbSimilarity = calculateSimilarityMI(i, j, dbMin, dbMax);
			D(i, j) = 1 / dbSimilarity;
			D(j, i) = 1 / dbSimilarity;
		}
	}

	// Compute metric MDS (embedding into a 2-dimensional space)
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, _nPCLen);
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			_arrPC[i*_nPCLen + j] = X(j, i);
		}
	}
}

/*
	calculate PCA using Fofonov's method
	2019/12/28
*/
void FeatureSet::calculatePCA_Fofonov() {
	MatrixXd D(_nEnsembleLen, _nEnsembleLen);
	for (unsigned i = 0; i < _nEnsembleLen; ++i)
	{
		for (unsigned j = i; j < _nEnsembleLen; ++j)
		{
			double dbDistance = calculateDistanceMC(i, j);
			D(i, j) = dbDistance;
			D(j, i) = dbDistance;
		}
	}

	// Compute metric MDS (embedding into a 2-dimensional space)
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, _nPCLen);
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			_arrPC[i*_nPCLen + j] = X(j, i);
		}
	}
}

/*
	calculate pca using MDS by set differences
*/
void FeatureSet::calculatePCA_MDS_Set() {
	/*
	int n = _nEnsembleLen;

	MatrixXd D(n, n);
	for (unsigned i = 0; i < n; ++i)
	{
		for (unsigned j = i; j < n; ++j)
		{
			int dis = 0;
			for (size_t k = 0; k < _nGrids; k++)
			{
				if (_pUpperSet[i*_nGrids + k] != _pUpperSet[j*_nGrids + k])
					dis++;
			}
			D(i, j) = dis;
			D(j, i) = dis;
		}
	}

	// Compute metric MDS (embedding into a 2-dimensional space)
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, _nPCLen);
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < _nPCLen; j++)
		{
			_arrPC[i*_nPCLen + j] = X(j, i);
		}
	}

	for (size_t i = 0; i < n; i++)
	{
		qDebug() << X(0, i);
	}
	qDebug() << "======================";
	for (size_t i = 0; i < n; i++)
	{
		qDebug() << X(1, i);
	}
	*/
}

/*
	calculate diverse grids and diverse count
	using UpperSet
*/
void FeatureSet::calculateDiverse() {
	/*
	_nDiverseCount = 0;
	for (size_t i = 0; i < _nGrids; i++)
	{
		bool bDiverse = false;

		for (size_t l = 1; l < _nEnsembleLen; l++)
		{
			if (_pUpperSet[l*_nGrids + i] != _pUpperSet[(l - 1)*_nGrids + i]) {
				bDiverse = true;
				_nDiverseCount++;
				break;
			}
		}
		_pGridDiverse[i] = bDiverse;

	}
	qDebug() << "Diverse count: " << _nDiverseCount;
	*/
}

double* FeatureSet::createDiverseArray() {
	double* arrBuf = new double[_nEnsembleLen*_nDiverseCount];
	for (size_t l = 0; l < _nEnsembleLen; l++)
	{
		for (size_t i = 0, j = 0; i < _nGrids; i++)
		{
			if (_pGridDiverse[i]) {
				arrBuf[l * _nDiverseCount + j] = _pSDFData->GetData(l, i);// _pSDF[l*_nGrids + i];
				j++;
			}
		}
	}
	return arrBuf;
}

/*
	cluster using diverse grids
*/
void FeatureSet::doClustering() {
	// 1.create cluster instance
	CLUSTER::Clustering* pClusterer = new CLUSTER::AHCClustering();
	int nN = _nEnsembleLen;			// number of data items
	int nK = 1;

	// 2.clustering
	bool bUseDiversity = false;		// whether using diversity
	if (bUseDiversity) {
		// 2.cluster
		int nM = _nDiverseCount;		// dimension
		double* arrBuf = createDiverseArray();

		pClusterer->DoCluster(nN, nM, nK, arrBuf, _arrLabels, _arrMergeSource, _arrMergeTarget);

		delete[] arrBuf;
	}
	else {
		int nM = _nGrids;
		//pClusterer->DoCluster(nN, nM, nK, _pSDF, _arrLabels, _arrMergeSource, _arrMergeTarget);
		pClusterer->DoCluster(nN, nM, nK, _pSDFData->GetData(), _arrLabels, _arrMergeSource, _arrMergeTarget);
	}

	// 3.reset labels
	resetLabels();

	// 4.release cluster instance
	delete pClusterer;
}

/*
	cluster using PCA
*/
void FeatureSet::doPCAClustering() {

	// 1.cluster
	CLUSTER::Clustering* pClusterer = new CLUSTER::AHCClustering();
	int nN = _nEnsembleLen;			// number of data items
	int nM = _nPCLen;		// dimension
	int nK = 1;						// clusters
	double* arrBuf = _arrPC;

	pClusterer->DoCluster(nN, nM, nK, arrBuf, _arrLabels,_arrMergeSource,_arrMergeTarget);
	
	// 2.reset label
	resetLabels();

	// 3.release the resouse
	delete pClusterer;
}

void FeatureSet::calculatePCARecovery() {
/*
	// 1.set parameter
	int mI = _nDiverseCount;
	int mO = 1;
	int n = _nEnsembleLen;
	// 2.allocate input and output buffer
	double* arrInput = createDiverseArray();
	double* arrOutput = new double[mO*n];

	// 3.pca
	MyPCA pca;
	pca.DoPCA(arrInput, arrOutput, n, mI, mO, true);

	

	int nTestLen = 10;
	double *arrRecoveredBuf = new double[nTestLen* _nDiverseCount];
	//pca.Recover(box, arrRecoveredBuf, 4,mO);

	pca.Recover(arrOutput, arrRecoveredBuf, nTestLen,mO);

	for (size_t l = 0; l < nTestLen; l++)
	{
		//pca.Recover(arrOutput + l * mO, arrRecoveredBuf + l * _nDiverseCount, 1, mO);
		for (size_t i = 0, j = 0; i < _nGrids; i++)
		{
			if (_pGridDiverse[i]) {
				//_pSDF[l*_nGrids + i] = arrRecoveredBuf[l * _nDiverseCount + j];
				_pSDFData->SetData(l, i, arrRecoveredBuf[l * _nDiverseCount + j]);
				j++;
			}
		}
		{
			QList<ContourLine> contour;
			ContourGenerator::GetInstance()->Generate(getSDF(l), contour, 0, _nWidth, _nHeight);
			_listContourSDF[l] = contour;
		}
	}

	// 5.release the buffer
	delete[] arrRecoveredBuf;

	delete[] arrOutput;
	delete[] arrInput;
*/
}

void FeatureSet::calculatePCABox() {
	/*
	// 1.set parameter
	int mI = _nDiverseCount;
	int mO = 2;
	int n = _nEnsembleLen;
	// 2.allocate input and output buffer
	double* arrInput = createDiverseArray();
	double* arrOutput = new double[mO*n];

	// 3.pca
	MyPCA pca;
	pca.DoPCA(arrInput, arrOutput, n, mI, mO, true);
	if(false)
		for (size_t i = 0; i < n; i++)
		{
			//qDebug() << arrOutput[i];
			qDebug() << arrOutput[i * 2] << "," << arrOutput[i * 2 + 1];
			//qDebug() << arrOutput[i * 3] << "," << arrOutput[i * 3 + 1] << "," << arrOutput[i * 3 + 2];
			//qDebug() << arrOutput[i * 4] << "," << arrOutput[i * 4 + 1] << "," << arrOutput[i * 4 + 2] << "," << arrOutput[i * 4 + 3];
		}

	double dbMax0 = -10000;
	double dbMax1 = -10000;
	double dbMin0 = 10000;
	double dbMin1 = 10000;
	// 4.generate points from the output
	for (size_t i = 0; i < n; i++)
	{
		if (arrOutput[i * 2] > dbMax0)
			dbMax0 = arrOutput[i * 2];
		if (arrOutput[i * 2] < dbMin0)
			dbMin0 = arrOutput[i * 2];
		if (arrOutput[i * 2 + 1] > dbMax1)
			dbMax1 = arrOutput[i * 2 + 1];
		if (arrOutput[i * 2 + 1] < dbMin1)
			dbMin1 = arrOutput[i * 2 + 1];
	}

	qDebug() << dbMin0 << "," << dbMin1 << "," << dbMax0 << "," << dbMax1;
	// recover
	double box[8] = {dbMin0, dbMin1
		, dbMin0, dbMax1
		, dbMax0, dbMin1
		, dbMax0, dbMax1 };


	double *arrRecoveredBuf = new double[4 * _nDiverseCount];
	pca.Recover(box, arrRecoveredBuf, 4,mO);


	for (size_t l = 0; l < 4; l++)
	{
		for (size_t i = 0, j = 0; i < _nGrids; i++)
		{
			if (_pGridDiverse[i]) {
			//	_pSDF[l*_nGrids + i] = arrRecoveredBuf[l * _nDiverseCount + j];
				_pSDFData->SetData(l, i, arrRecoveredBuf[l * _nDiverseCount + j]);
				j++;
			}			
		}
		{
			QList<ContourLine> contour;
			ContourGenerator::GetInstance()->Generate(getSDF(l), contour, 0, _nWidth, _nHeight);
			_listContourSDF[l]=contour;
		}
	}

	// 5.release the buffer
	delete[] arrRecoveredBuf;

	delete[] arrOutput;
	delete[] arrInput;
*/
}

// 感觉这个方法实现的还是不太对啊
void FeatureSet::calculateICD() {
	qDebug() << "calculateICD" << _dbIsoValue;
	double dbMax = 0;

	// 0.allocate resource
	double* pMeans = new double[_nGrids];
	double* pVari2 = new double[_nGrids];
	// 1.calculate means and variances
	for (size_t i = 0; i < _nGrids; i++)
	{
		double dbSum = 0;
		for (size_t l = 0; l < _nEnsembleLen; l++)
		{
			double dbValue = _pData->GetData(l, i);
			dbSum += dbValue;
		}
		double dbMean = dbSum / _nEnsembleLen;
		double dbVars = 0;
		for (size_t l = 0; l < _nEnsembleLen; l++)
		{
			double dbValue = _pData->GetData(l, i);
			double dbBias = dbValue - dbMean;
			dbVars += dbBias * dbBias;
		}
		pMeans[i] = dbMean;
		pVari2[i] = dbVars;
	}
	// 2.calculate probability
	int nWidth = (_nWidth - 1) * _nDetailScale + 1;
	int nHeight = (_nHeight - 1) * _nDetailScale + 1;
	for (size_t i = 0; i < nHeight; i++)
	{
		for (size_t j = 0; j < nWidth; j++)
		{
			int i0 = i / _nDetailScale;
			int j0 = j / _nDetailScale;
			int i1 = i0 + 1;
			int j1 = j0 + 1;
			int id = i % _nDetailScale;
			int jd = j % _nDetailScale;

			double dbi1 = id / (double)((_nDetailScale - 1) > 0 ? (_nDetailScale - 1) : 1);
			double dbi0 = 1.0 - dbi1;
			double dbj1 = jd / (double)((_nDetailScale - 1) > 0 ? (_nDetailScale - 1) : 1);
			double dbj0 = 1.0 - dbj1;

			double dbM00 = pMeans[i0*_nWidth + j0];
			double dbM01 = i1 < _nHeight ? pMeans[i1*_nWidth + j0] : 0;
			double dbM10 = j1 < _nWidth ? pMeans[i0*_nWidth + j1] : 0;
			double dbM11 = (i1 < _nHeight && j1 < _nWidth) ? pMeans[i1*_nWidth + j1] : 0;

			double dbV00 = pVari2[i0*_nWidth + j0];
			double dbV01 = i1 < _nHeight ? pVari2[i1*_nWidth + j0] : 0;
			double dbV10 = j1 < _nWidth ? pVari2[i0*_nWidth + j1] : 0;
			double dbV11 = (i1 < _nHeight && j1 < _nWidth) ? pVari2[i1*_nWidth + j1] : 0;

			double dbMean = dbM00 * dbi0*dbj0
				+ dbM01 * dbi1*dbj0
				+ dbM10 * dbi0*dbj1
				+ dbM11 * dbi1*dbj1;

			double dbVar2 = dbV00 * dbi0*dbj0
				+ dbV01 * dbi1*dbj0
				+ dbV10 * dbi0*dbj1
				+ dbV11 * dbi1*dbj1;

			double dbBias = _dbIsoValue - dbMean;
			double dbVar = sqrt(dbVar2);		// adjust the effect of variance
			//qDebug() << dbMean << "\t" << dbVar2 << "\t" << i << "\t" << j;
			//_pICD[i*nWidth + j] = pow(Ed, -dbBias * dbBias / 2 * dbVari);
			
			//_pICD[i*nWidth + j] = pow(Ed, -dbBias * dbBias / (2 * dbVar2)) /(sqrt(PI2d*dbVar2));

			_pICD[i*nWidth + j] = pow(Ed, -dbBias * dbBias / (2 * dbVar2)) / ((sqrt(PI2d))*dbVar);
			if (_pICD[i*nWidth + j] > dbMax) {
				dbMax = _pICD[i*nWidth + j];
				//qDebug() << "var:" << dbVar;
				//qDebug() << "dbBias:" << dbBias;
			}
		}
	}
	for (size_t i = 0; i < nWidth*nHeight; i++)
	{
		_pICD[i] /= dbMax;
	}
	qDebug() << "Max of ICD: " << dbMax;
	// free resource
	delete pMeans;
	delete pVari2;
}

void FeatureSet::buildICD_LineKernel() {
	for (size_t i = 0; i < _nGrids; i++)
	{
		_pICD_LineKernel[i] = 0;
	}
	int nRadius = 10;
	double dbH = 1.0;
	double dbMax = 0;
	for (size_t l = 0; l < _nEnsembleLen; l++) {
		for each (QList<ContourLine> contours in _spaghettiPlot.GetContours())
		{
			for each (ContourLine contour in contours)
			{
				for each (QPointF pt in contour._listPt)
				{
					double x = pt.x();
					double y = pt.y();
					for (int iX=x- nRadius;iX<x+ nRadius;iX++)
					{
						if (iX < 0 || iX >= _nWidth) continue;
						for (int iY = y - nRadius; iY < y + nRadius; iY++)
						{
							if (iY < 0 || iY >= _nHeight) continue;
							double dbX = iX - x;
							double dbY = iY - y;
							double dbDis2 = dbX*dbX + dbY * dbY;
							double dbV = pow(Ed, -dbDis2 / 2);
							_pICD_LineKernel[iY*_nWidth + iX] += dbV;
							if (_pICD_LineKernel[iY*_nWidth + iX] > dbMax)
								dbMax = _pICD_LineKernel[iY*_nWidth + iX];
						}
					}
				}
			}
		}
	}

	for (size_t i = 0; i < _nGrids; i++)
	{
		_pICD_LineKernel[i] /= (dbMax + 1);
	}
}

void FeatureSet::buildICD_Vector() {
	for (size_t i = 0; i < _nGrids; i++)
	{
		_pICDX[i] = 0;
		_pICDY[i] = 0;
		_pICDZ[i] = 0;
	}
	int nRadius = 40;
	double dbH = 1.0;
	double dbMaxX = 0;
	double dbMaxY = 0;
	double dbMaxZ = 0;
	for (size_t l = 0; l < _nEnsembleLen; l++) {
		for each (QList<ContourLine> contours in _spaghettiPlot.GetContours())
		{
			for each (ContourLine contour in contours)
			{
				int nLen = contour._listPt.length();
				for (size_t i = 1; i < nLen; i++)
				{
					double x1 = contour._listPt[i - 1].x();
					double y1 = contour._listPt[i - 1].y();
					double x2 = contour._listPt[i].x();
					double y2 = contour._listPt[i].y();

					for (int iX = x1 - nRadius; iX < x1 + nRadius; iX++)
					{
						if (iX < 0 || iX >= _nWidth) continue;
						for (int iY = y1 - nRadius; iY < y1 + nRadius; iY++)
						{
							if (iY < 0 || iY >= _nHeight) continue;
							double dbX = iX - x1;
							double dbY = iY - y1;
							double dbDis2 = dbX * dbX + dbY * dbY;
							double dbV = pow(Ed, -dbDis2 / 2);
							int nIndex = iY * _nWidth + iX;
							double dbVX = x2 - x1;
							double dbVY = y2 - y1;
							_pICDZ[nIndex] += abs(_pICDX[nIndex] * dbVY + _pICDY[nIndex] * dbVX)*dbV;

							_pICDX[nIndex] += dbVX * dbV;
							_pICDY[nIndex] += dbVY * dbV;

						//	if (_pICDX[nIndex] > dbMaxX) dbMaxX = _pICDX[nIndex];
						//	if (_pICDY[nIndex] > dbMaxY) dbMaxY = _pICDY[nIndex];
							if (_pICDZ[nIndex] > dbMaxZ) dbMaxZ = _pICDZ[nIndex];
						}
					}
				}
			}
		}
	}


	for (size_t i = 0; i < _nGrids; i++)
	{
		_pICDX[i] = sqrt(_pICDX[i] * _pICDX[i] + _pICDY[i] * _pICDY[i]);
		if (_pICDX[i] > .1) _pICDY[i] = _pICDZ[i] / _pICDX[i];
		else _pICDY[i] = 0;

		if (_pICDX[i] > dbMaxX) dbMaxX = _pICDX[i];
		if (_pICDY[i] > dbMaxY) dbMaxY = _pICDY[i];
	}
	for (size_t i = 0; i < _nGrids; i++)
	{
		_pICDX[i] /= (dbMaxX + 1);
		_pICDY[i] /= (dbMaxY + 1);
		_pICDZ[i] /= (dbMaxZ + 1);
	}
}

double Kernel(double x) {
	return 1 / SQRTP2d * pow(Ed, -x * x / 2);
}

void FeatureSet::buildICDV() {

	for (size_t i = 0, len = _nEnsembleLen*_nGrids; i < len; i++)
	{
		_pICDVX[i] = 0;
		_pICDVY[i] = 0;
	}
	int nRadius = 50;
	double dbH = 1;
	double dbMaxX = 0;
	double dbMaxY = 0;
	double dbMaxZ = 0;
	for (size_t l = 0; l < _nEnsembleLen; l++) {
		QList<ContourLine> contours  = _spaghettiPlot.GetContours()[l];

		for each (ContourLine contour in contours)
		{
			int nLen = contour._listPt.length();
			for (size_t i = 1; i < nLen; i++)
			{
				double x1 = contour._listPt[i - 1].x();
				double y1 = contour._listPt[i - 1].y();
				double x2 = contour._listPt[i].x();
				double y2 = contour._listPt[i].y();
				double x0 = (x1 + x2) / 2.0;
				double y0 = (y1 + y2) / 2.0;

				for (int iX = x1 - nRadius; iX < x1 + nRadius; iX++)
				{
					if (iX < 0 || iX >= _nWidth) continue;
					for (int iY = y1 - nRadius; iY < y1 + nRadius; iY++)
					{
						if (iY < 0 || iY >= _nHeight) continue;
						double dbX = iX - x0;
						double dbY = iY - y0;
						double dbDis = sqrt(dbX * dbX + dbY * dbY);
						double dbV = Kernel(dbDis / dbH) / dbH;

						double dbVX = x2 - x1;
						double dbVY = y2 - y1;
						int nIndex = l * _nGrids + iY * _nWidth + iX;

						_pICDVX[nIndex] += dbVX * dbV;
						_pICDVY[nIndex] += dbVY * dbV;
					}
				}
			}
		}
		for (size_t i = 0; i < _nGrids; i++)
		{
			_pICDVX[l * _nGrids + i] *= 20;
			_pICDVY[l * _nGrids + i] *= 20;
		}
	}
	for (size_t i = 0, len = _nEnsembleLen * _nGrids; i < len; i++)
	{
		_pICDVW[i] = sqrt(_pICDVX[i] * _pICDVX[i] + _pICDVY[i] * _pICDVY[i]);
	}

}

void FeatureSet::SetClustersLen(int nClustersLen) {
	//_nClusters = nClustersLen;
	resetLabels();
}

void FeatureSet::resetLabels() {
	// 1.initialize labels
	for (size_t i = 0; i < _nEnsembleLen; i++) {
		_arrLabels[i] = i;
	}

	// 2.merge
	for (int i = 0; i < _nEnsembleLen-_nClusters; i++)
	{
		int nSource = _arrMergeSource[i];
		int nTarget = _arrMergeTarget[i];

		// update source and target
		for (int j = 0; j < _nEnsembleLen; j++)
		{
			if (_arrLabels[j] == nSource) {
				_arrLabels[j] = nTarget;
			}
		}
	}

	// 3.align
	CLUSTER::Clustering::AlignLabels(_arrLabels, _nEnsembleLen);

	// 4.calculate length of each cluster
	for (int i = 0; i < _nClusters; i++) _arrClusterLen[i] = 0;
	for (int i = 0; i < _nEnsembleLen; i++) _arrClusterLen[_arrLabels[i]]++;

	qDebug() << "Length of each Cluster:";
	for (int i = 0; i < _nClusters; i++) qDebug() << _arrClusterLen[i];

}

void FeatureSet::generateRandomIndices() {
	// generate a random indices list
	int arrIndices[50];
	for (int i = 0; i < 50; i++) arrIndices[i] = i;
	for (int l = 50; l > 0; l--)
	{
		// random choose an index
		int nIndex = rand() / (double)RAND_MAX*l;
		_arrRandomIndices[l - 1] = arrIndices[nIndex];
		arrIndices[nIndex] = arrIndices[l - 1];
	}
}

void FeatureSet::generateSequentialIndices() {
	for (int i = 0; i < 50; i++) _arrRandomIndices[i] = i;
}

void FeatureSet::generateCentralIndices() {
	int nBias = (_nEnsembleLen - _nTestSamples) / 2;
	for (size_t i = 0; i < _nTestSamples; i++)
	{
		_arrRandomIndices[i] = i + nBias;
	}
}

double FeatureSet::getUpperProperty(double x, double y, const double* pSDF, int nLen, bool bUseIndices) {
	int nX = x;
	int nY = y;
	double dbX = x - nX;
	double dbY = y - nY;

	int nCount = 0;

	for (size_t l = 0; l < nLen; l++)
	{
		if (getFieldValue(nX, nY, dbX, dbY, pSDF + (bUseIndices? _arrRandomIndices[l] :l) * _nGrids) > 0) nCount++;
	}
	return nCount / (double)nLen;
}

double FeatureSet::getFieldValue(int nX, int nY, double dbX, double dbY, const double* pField) {
	double dbV00 = pField[nY*_nWidth + nX];
	double dbV01 = pField[(nY+1)*_nWidth + nX];
	double dbV10 = pField[nY*_nWidth + nX+1];
	double dbV11 = pField[(nY+1)*_nWidth + nX+1];

	//qDebug() << dbV00 << dbV01 << dbV10 << dbV11;

	return dbX * dbY*dbV00
		+ dbX * (1 - dbY)*dbV01
		+ (1 - dbX) * dbY*dbV10
		+ (1 - dbX) * (1 - dbY)*dbV11;
}

void FeatureSet::BuildTess() {
		// tess
	if (g_bCBP) _cbPlots.BuildTess();
	if (g_bCVP) _vbPlots.BuildTess();
	if (g_bCPP) _cpPlots.BuildTess();
	if (g_bRCP) _rcPlotsSDF.BuildTess();
	if (g_bICD) _cdPlots.GenerateTexture();
}

void FeatureSet::DrawContourBoxplots() {
	_cbPlots.DrawBands();
}

void FeatureSet::DrawVariabilityPlots(int nCluster) {

	double dbOpacity = .3;
	
	if (nCluster>0) {
		// draw one cluster
		int nIndex = nCluster - 1;
		glColor3f(ColorMap::GetCategory10D(nIndex, 0)
			, ColorMap::GetCategory10D(nIndex, 1)
			, ColorMap::GetCategory10D(nIndex, 2));
		_vbPlots.DrawCluster(nIndex);
	}
	else if (nCluster == -1) {
		// draw non-cluster
		_vbPlots.DrawBands();
	}
	else {
		// draw all clusters
		for (size_t nIndex = 0; nIndex < _nClusters; nIndex++)
		{
			glColor3f(ColorMap::GetCategory10D(nIndex, 0)
				, ColorMap::GetCategory10D(nIndex, 1)
				, ColorMap::GetCategory10D(nIndex, 2));
			_vbPlots.DrawCluster(nIndex);
		}
	}

}

void FeatureSet::DrawContourProbabilityPlots() {
	_cpPlots.DrawBands();
}

void FeatureSet::DrawIsoContourDensityPlots() {
	_cdPlots.DrawBands();
}

void FeatureSet::DrawSortedSDFContours(int nLevel) {
	_rcPlotsSDF.DrawSortedContours(nLevel);
}

void FeatureSet::DrawContours(int nMember, int nCluster,bool bCluster) {
	_spaghettiPlot.DrawContours(nMember,nCluster, bCluster);
}

void FeatureSet::DrawSDFContours(int nMember, int nCluster, bool bCluster) {
	//_rcPlotsSDF.DrawContours(nMember, nCluster, bCluster);
}

void FeatureSet::DrawSortedContours(int nLevel) {
	if (g_bDomainRCPlot) _rcPlotsDomain.DrawSortedContours(nLevel);
}

void FeatureSet::DrawResampledContours(int nLevel) {
	if (g_bDomainRCPlot) _rcPlotsDomain.DrawResampledContours(nLevel);
}

void FeatureSet::DrawResampledSDFContours(int nLevel) {
	_rcPlotsSDF.DrawResampledContours(nLevel);
}

void FeatureSet::DrawEnConVisBands(int nLevel) {	
	_rcPlotsSDF.DrawBands(nLevel);
}

void FeatureSet::DrawEnConVisBands_C(int nLevel, int nCluster) {
	//qDebug() << "void FeatureSet::DrawEnConVisBands_C(int nLevel, int nCluster) {";
	//qDebug() << nLevel << nCluster;
	_rcPlotsSDF.DrawBands_C(nLevel,nCluster);
}

void FeatureSet::DrawContoursResampledForClusters(int nLevel, int nCluster) {
	_rcPlotsSDF.DrawContoursResampledForClusters(nLevel,nCluster);
}

void FeatureSet::DrawOutliers() {
	_cbPlots.DrawOutliers();
}

void FeatureSet::SetTransparency(int nT) {
	_spaghettiPlot.SetTransparency(nT);
}

