#include "MeteModel.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

#include "ContourGenerator.h"
#include "ColorMap.h"

#include "AHCClustering.h"
#include "KMeansClustering.h"
#include "MyPCA.h"
#include "DataField.h"
#include "FeatureSet.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>
#include <sstream>

#include "def.h"

#include "DBSCANClustering.h"

#include "ArtificialModel.h"

#include "Switch.h"
#include "timer.h"


using namespace std;

// 判断点在线段左侧还是右侧，网上找的公式，来不及仔细想了
bool checkLeft(double x, double y, double x1, double y1, double x2, double y2) {
	double dx1 = x2 - x1;
	double dy1 = y2 - y1;
	double dx0 = x1 - x;
	double dy0 = y1 - y;
	return dx0 * dy1 - dy0 * dx1 > 0;
}

MeteModel::MeteModel()
{
	_nWidth = GlobalSetting::g_nWidth;
	_nHeight = GlobalSetting::g_nHeight;
	_nWest = GlobalSetting::g_nWest;
	_nEast = GlobalSetting::g_nEast;
	_nNorth = GlobalSetting::g_nNorth;
	_nSouth = GlobalSetting::g_nSouth;

	_nGrids = _nWidth*_nHeight;

	//readObsData();

	for (size_t i = 0; i < 62; i++)
	{
		_arrTimeSteps[i] = 0;
	}
}

MeteModel::~MeteModel()
{
	for (size_t i = 0; i < 62; i++)
	{
		if (_arrTimeSteps[i]) delete _arrTimeSteps[i];
	}

	if (_dataTexture) delete[] _dataTexture;




	ContourGenerator::Release();
}

void MeteModel::InitModel(int nEnsembleLen, int nWidth, int nHeight
	, bool bBinary, int nWest, int nEast, int nSouth, int nNorth) {
	// 0.record states variables
	_nEnsembleLen = nEnsembleLen;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_nGrids = nHeight*nWidth;

	_nWest = nWest;
	_nEast = nEast;
	_nSouth = nSouth;
	_nNorth = nNorth;

	// 5.specializaed initialization
	initializeModel();
}

void MeteModel::readDipValue(char* strFileName) {


}

void MeteModel::readDipValueG(char* strFileName) {

}

void MeteModel::readDataFromTextG() {
	QFile file(_strFile);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);


	int nWidth = _nWidth - 1;				// width of a scalar field in the file
	int nLen = nWidth*_nHeight;				// length of a scalar field in the file

	int nStep = 4;
	for (size_t t = 0; t < nStep; t++)
	{
		// every ensemble member
		for (int i = 0; i < _nEnsembleLen; i++)
		{
			QString line = in.readLine();
			// every grid
			for (int j = 0; j < nLen; j++)
			{
				QString line = in.readLine();
				int r = j / nWidth;
				int c = j % nWidth;
				int nIndex = r*_nWidth + c + 1;
				_pTimeStep->_pData->SetData(i, nIndex, line.toFloat());
				if (c == nWidth - 1)
				{
					int nIndex = r*_nWidth;
					_pTimeStep->_pData->SetData(i, nIndex, line.toFloat());
				}
			}
		}

	}

	file.close();
}

void MeteModel::buildTextureThresholdVariance() {
	const double* pData = _pTimeStep->_pData->GetVari(_nSmooth);
	_nThresholdedGridPoints = 0;
	// color map
	ColorMap* colormap = ColorMap::GetInstance();
	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {
			int nIndex = i*_nWidth + j;
			if (pData[nIndex]>_dbVarThreshold)
			{
				_dataTexture[4 * nIndex + 0] = ColorMap::GetThresholdColorI(0);
				_dataTexture[4 * nIndex + 1] = ColorMap::GetThresholdColorI(1);
				_dataTexture[4 * nIndex + 2] = ColorMap::GetThresholdColorI(2);
				_dataTexture[4 * nIndex + 3] = (GLubyte)255;

				_nThresholdedGridPoints++;
			}
			else {
				_dataTexture[4 * nIndex + 0] = (GLubyte)255;
				_dataTexture[4 * nIndex + 1] = (GLubyte)255;
				_dataTexture[4 * nIndex + 2] = (GLubyte)255;
				_dataTexture[4 * nIndex + 3] = (GLubyte)255;

			}
		}
	}
}

void MeteModel::buildTextureSDF() {
	const double* pData = _pTimeStep->_listFeature[0]->GetSDF(_nMember? _nMember-1: _nMember);
	ColorMap* colormap = ColorMap::GetInstance(ColorMap::CP_EOF);


	double dbMax = -100000;
	double dbMin = 100000;	
	for (int i = 0; i < _nGrids; i++)
	{
		if (pData[i] > dbMax) dbMax = pData[i];
		if (pData[i] < dbMin) dbMin = pData[i];
	}
	colormap->SetRange(dbMin, dbMax);
	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			MYGLColor color = colormap->GetColor(pData[nIndex]);
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = color._rgb[0];
			_dataTexture[4 * nIndex + 1] = color._rgb[1];
			_dataTexture[4 * nIndex + 2] = color._rgb[2];
			_dataTexture[4 * nIndex + 3] = (GLubyte)255;
		}
	}
}

void MeteModel::buildTextureICDVX() {
	const double* pData = _pTimeStep->_listFeature[0]->GetICDVX(_nMember ? _nMember - 1 : _nMember);
	ColorMap* colormap = ColorMap::GetInstance(ColorMap::CP_EOF);


	double dbMax = -1000;
	double dbMin = 1000;	
	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			MYGLColor color = colormap->GetColor(pData[nIndex]);
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = color._rgb[0];
			_dataTexture[4 * nIndex + 1] = color._rgb[1];
			_dataTexture[4 * nIndex + 2] = color._rgb[2];
			_dataTexture[4 * nIndex + 3] = (GLubyte)255;
			if (pData[nIndex] > dbMax) dbMax = pData[nIndex];
			if (pData[nIndex] < dbMin) dbMin = pData[nIndex];
		}
	}
}

void MeteModel::buildTextureICDVY() {
	const double* pData = _pTimeStep->_listFeature[0]->GetICDVY(_nMember ? _nMember - 1 : _nMember);
	ColorMap* colormap = ColorMap::GetInstance(ColorMap::CP_EOF);


	double dbMax = -1000;
	double dbMin = 1000;	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			MYGLColor color = colormap->GetColor(pData[nIndex]);
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = color._rgb[0];
			_dataTexture[4 * nIndex + 1] = color._rgb[1];
			_dataTexture[4 * nIndex + 2] = color._rgb[2];
			_dataTexture[4 * nIndex + 3] = (GLubyte)255;
			if (pData[nIndex] > dbMax) dbMax = pData[nIndex];
			if (pData[nIndex] < dbMin) dbMin = pData[nIndex];
		}
	}
}

void MeteModel::buildTextureICDVW() {
	const double* pData = _pTimeStep->_listFeature[0]->GetICDVW(_nMember ? _nMember - 1 : _nMember);
	ColorMap* colormap = ColorMap::GetInstance(ColorMap::CP_EOF);


	double dbMax = -1000;
	double dbMin = 1000;	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			MYGLColor color = colormap->GetColor(pData[nIndex]);
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = color._rgb[0];
			_dataTexture[4 * nIndex + 1] = color._rgb[1];
			_dataTexture[4 * nIndex + 2] = color._rgb[2];
			_dataTexture[4 * nIndex + 3] = (GLubyte)255;
			if (pData[nIndex] > dbMax) dbMax = pData[nIndex];
			if (pData[nIndex] < dbMin) dbMin = pData[nIndex];
		}
	}
}

void MeteModel::buildTextureICD() {
	qDebug() << "buildTextureICD";
	int nDetaiScale = _pTimeStep->_listFeature[0]->GetDetailScale();
	_nTexW = (_nWidth - 1) * nDetaiScale + 1;
	_nTexH = (_nHeight - 1) * nDetaiScale + 1;
	const double* pData= _pTimeStep->_listFeature[0]->GetICD();	// the data

	qDebug()<<_nTexW << _nTexH;
	for (int i = 0; i < _nTexH; i++) {
		for (int j = 0; j < _nTexW; j++) {

			int nIndex = i * _nTexW + j;
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = 100;
			_dataTexture[4 * nIndex + 1] = 100;
			_dataTexture[4 * nIndex + 2] = 200;
			_dataTexture[4 * nIndex + 3] = (GLubyte)(pData[nIndex] * 255);
		}
	}
}

void MeteModel::buildTextureICD_LineKernel(){
	qDebug() << "line kernel" << endl;
	const double* pData = _pTimeStep->_listFeature[0]->GetICD_LineKernel();


	double dbMax = -1000;
	double dbMin = 1000;	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = 100;
			_dataTexture[4 * nIndex + 1] = 100;
			_dataTexture[4 * nIndex + 2] = 200;
			_dataTexture[4 * nIndex + 3] = (GLubyte)(pData[nIndex] * 255);
		}
	}
}

void MeteModel::buildTextureICDX() {
	qDebug() << "line kernel X" << endl;
	const double* pData = _pTimeStep->_listFeature[0]->GetICDX();


	double dbMax = -1000;
	double dbMin = 1000;	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = 100;
			_dataTexture[4 * nIndex + 1] = 100;
			_dataTexture[4 * nIndex + 2] = 200;
			_dataTexture[4 * nIndex + 3] = (GLubyte)(pData[nIndex] * 255);
		}
	}
}

void MeteModel::buildTextureICDY() {
	qDebug() << "line kernel Y" << endl;
	const double* pData = _pTimeStep->_listFeature[0]->GetICDY();


	double dbMax = -1000;
	double dbMin = 1000;	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = 100;
			_dataTexture[4 * nIndex + 1] = 100;
			_dataTexture[4 * nIndex + 2] = 200;
			_dataTexture[4 * nIndex + 3] = (GLubyte)(pData[nIndex] * 255);
		}
	}
}

void MeteModel::buildTextureICDZ() {
	qDebug() << "line kernel Z" << endl;
	const double* pData = _pTimeStep->_listFeature[0]->GetICDZ();


	double dbMax = -1000;
	double dbMin = 1000;	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i * _nWidth + j;
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = 100;
			_dataTexture[4 * nIndex + 1] = 100;
			_dataTexture[4 * nIndex + 2] = 200;
			_dataTexture[4 * nIndex + 3] = (GLubyte)(pData[nIndex] * 255);
		}
	}
}

void MeteModel::buildTextureThresholdDipValue() {

}

// generate texture of clustered variance
void MeteModel::buildTextureClusteredVariance() {

}

void MeteModel::generateRegions() {

}

// generate texture of colormap of mean or variance
void MeteModel::buildTextureColorMap() {
	const double* pData;	// the data
	ColorMap* colormap;		// color map function

	switch (_bgFunction)
	{
	case MeteModel::bg_mean:
		if (_nMember)
		{
			pData = _pTimeStep->_pData->GetLayer(_nMember - 1);
		}
		else {
			pData = _pTimeStep->_pData->GetMean();
		}
		break;
	case MeteModel::bg_vari:
		pData = _pTimeStep->_pData->GetVari();
		break;
	case MeteModel::bg_vari_smooth:
		pData = _pTimeStep->_pData->GetVari(_nSmooth);
		colormap = ColorMap::GetInstance();
		break;
	case MeteModel::bg_dipValue:
		break;

	case MeteModel::bg_err:
		break;
	default:
		break;
	}

	colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
	double dbMin = 100000;
	double dbMax = -100000;
	for (size_t i = 0; i < _nGrids; i++)
	{
		if (pData[i] > dbMax) dbMax = pData[i];
		if (pData[i] < dbMin) dbMin = pData[i];
	}

	qDebug() << "min:" << dbMin;
	qDebug() << "max:" << dbMax;
	colormap->SetRange(dbMin, dbMax);

	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {

			int nIndex = i*_nWidth + j;
			MYGLColor color = colormap->GetColor(pData[nIndex]);
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndex + 0] = color._rgb[0];
			_dataTexture[4 * nIndex + 1] = color._rgb[1];
			_dataTexture[4 * nIndex + 2] = color._rgb[2];
			_dataTexture[4 * nIndex + 3] = (GLubyte)255;
		}
	}
//	qDebug() << "Max: " << dbMax;
//	qDebug() << "Min: " << dbMin;
}

vector<double> MeteModel::GetVariance() {

	const double* pData = _pTimeStep->_pData->GetVari(_nSmooth);
	vector<double> vecVar;
	for (int i = 0; i < _nHeight; i++) {
		for (int j = 0; j < _nWidth; j++) {
			int nIndex = i*_nWidth + j;
			vecVar.push_back(pData[nIndex]);
		}
	}
	std::sort(vecVar.begin(), vecVar.end());
	return vecVar;
}

void MeteModel::readData() {
	QFile file(_strFile);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	char* temp = new char[file.size()];
	file.read(temp, file.size());
	float* f = (float*)temp;
	int x = file.size();

	for (int l = 0; l < _nEnsembleLen; l++)
	{
		// 第一个集合之前多一个数据，之后的集合之前多两个数据
		if (l == 0) f++;
		else f += 2;

		for (int i = 0; i < _nHeight; i++)
		{
			for (int j = 0; j < _nWidth; j++)
			{
				double fT = *f;
				_pTimeStep->_pData->SetData(l, i, j, *f++);
				// 只取整度，过滤0.5度
//				if (_bFilter&&j < _nWidth - 1) f++;
			}
			// 只取整度，过滤0.5度
//			if (_bFilter&&i < _nHeight - 1) f += (_nWidth * 2 - 1);
		}
	}
}

void MeteModel::initializeModel() {
	_nTime = GlobalSetting::g_nTimeStart;
	while (_nTime <= GlobalSetting::g_nTimeEnd) {
		updateTimeStep();
		_nTime += 6;
		//break;
	}
}

void MeteModel::updateTimeStep() {


	int nTimeIndex = _nTime / 6;
	qDebug() << "nTimeIndex" << nTimeIndex;
	if (_arrTimeSteps[nTimeIndex]) {
		_pTimeStep = _arrTimeSteps[nTimeIndex];
	}
	else {

		_pTimeStep = _arrTimeSteps[nTimeIndex] = new TimeStep();
		_pTimeStep->Init(_nWidth, _nHeight, _nEnsembleLen);

		// 1.Read data
		_strFile = GlobalSetting::g_strPath + QString("-mb") + QString::number(_nTime) + QString(".txt");
		_pTimeStep->_pData->ReadDataFromText(_strFile);

		// 2.statistic
		_pTimeStep->_pData->DoStatistic();

		// 2.Set isovalues
		{
			QList<double> listIsoValue;
			for (int i = 0; i < GlobalSetting::g_nFeatureLen; i++)
			{
				listIsoValue.append(GlobalSetting::g_arrIso[i]);
			}

			SetIsoValues(listIsoValue);
		}

		// 4.generate feature;
		for each (double isoValue in _listIsoValues)
		{
			_pTimeStep->_listFeature.append(new FeatureSet(_pTimeStep->_pData, isoValue, _nWidth, _nHeight, _nEnsembleLen));
		}
	}

}

void MeteModel::Brush(int nLeft, int nRight, int nTop, int nBottom) {

}

QList<QList<ContourLine>> MeteModel::GetContourBrushed()
{
	return _listContourBrushed;
}

QList<QList<ContourLine>> MeteModel::GetContourNotBrushed()
{
	return _listContourNotBrushed;
}

void MeteModel::DrawContour(bool bCluster,int isoIndex)
{
	_pTimeStep->_listFeature[isoIndex]->DrawContours(_nMember,_nEnsCluster,bCluster);
}

void MeteModel::DrawContourSDF(bool bCluster, int isoIndex)
{
	_pTimeStep->_listFeature[isoIndex]->DrawSDFContours(_nMember, _nEnsCluster, bCluster);
}

void MeteModel::GetMerge(int l, int& nSource, int& nTarget) {
	nSource = _pTimeStep->_listFeature[0]->GetMergeSource(l);
	nTarget = _pTimeStep->_listFeature[0]->GetMergeTarget(l);
}

int MeteModel::GetLabel(int l) {
	return _pTimeStep->_listFeature[0]->GetLabel(l);
}

double MeteModel::GetPC(int l,int nIndex) {
	return _pTimeStep->_listFeature[0]->GetPC(l,nIndex);
}

int MeteModel::GetClusters() { 
	return _pTimeStep->_listFeature[0]->GetClusters();
}

void MeteModel::DrawOutliers(int isoIndex)
{
	_pTimeStep->_listFeature[isoIndex]->DrawOutliers();

}

// add item from source to target
void addContour(const QList<QList<ContourLine>>& source, QList<QList<ContourLine>>& target, int nIndex0, int nIndex1, int nLevel) {
	if (nIndex1 > nIndex0) {
		int nMedian = (nIndex1 + nIndex0) / 2;
		target.push_back(source[nMedian]);
		if (nLevel > 0) {
			addContour(source, target, nIndex0, nMedian, nLevel - 1);
			addContour(source, target, nMedian+1, nIndex1, nLevel - 1);
		}
	}
}

void MeteModel::DrawContourSorted(int isoIndex)
{
	_pTimeStep->_listFeature[isoIndex]->DrawSortedContours(_arrContourLevel[isoIndex]);
}

void MeteModel::DrawContourSortedSDF(int isoIndex)
{
	_pTimeStep->_listFeature[isoIndex]->DrawSortedSDFContours(_arrContourLevel[isoIndex]);

}

void MeteModel::DrawContourResampled(int isoIndex) {
	_pTimeStep->_listFeature[isoIndex]->DrawResampledContours(_arrContourLevel[isoIndex]);
}

void MeteModel::DrawContourResampledSDF(int isoIndex) {
	_pTimeStep->_listFeature[isoIndex]->DrawResampledSDFContours(_arrContourLevel[isoIndex]);

}

void MeteModel::DrawContoursResampledForClusters(int isoIndex)
{
	_pTimeStep->_listFeature[0]->DrawContoursResampledForClusters(_arrContourLevel[0],_nEnsCluster);
}

void MeteModel::DrawEnConVisBands(int isoIndex)
{
	int nLevel = _arrContourLevel[isoIndex];
	_pTimeStep->_listFeature[isoIndex]->DrawEnConVisBands(nLevel);
}

void MeteModel::DrawEnConVisBands_C(int isoIndex)
{
	int nLevel = _arrContourLevel[isoIndex];
	_pTimeStep->_listFeature[isoIndex]->DrawEnConVisBands_C(nLevel, _nEnsCluster);
}

MeteModel* MeteModel::CreateModel(bool bA) {
	// 1.Create model instance
	//MeteModel* pModel = new MeteModel();
	MeteModel* pModel = bA ? new ArtificialModel() : new MeteModel();

	pModel->InitModel(GlobalSetting::g_nMembers, GlobalSetting::g_nWidth, GlobalSetting::g_nHeight, false, GlobalSetting::g_nWest, GlobalSetting::g_nEast, GlobalSetting::g_nSouth, GlobalSetting::g_nNorth);

	return pModel;
}

void MeteModel::SetVarThreshold(double dbThreshold)
{ 
	if (abs(dbThreshold - _dbVarThreshold) < 0.0001) return;

	_dbVarThreshold = dbThreshold; 

	regenerateTexture();
}

void MeteModel::SetSmooth(int nSmooth) {
	if (nSmooth == _nSmooth) return;
	_nSmooth = nSmooth;
	regenerateTexture();
}

void MeteModel::SetMember(int nMember) {
	_nMember = nMember;
	regenerateTexture();
}

void MeteModel::SetEnsCluster(int nEnsCluster) {
	_nEnsCluster = nEnsCluster;
	regenerateTexture();
}

void MeteModel::SetEnsClusterLen(int nEnsClusterLen) {
	_pTimeStep->_listFeature[0]->SetClustersLen(nEnsClusterLen);
	regenerateTexture();
}

void MeteModel::regenerateTexture() {
	_nTexW = _nWidth;
	_nTexH = _nHeight;
	if (!_dataTexture)
	{
		double dbScale = 100;
		if (_pTimeStep->_listFeature.size())
			dbScale = _pTimeStep->_listFeature[0]->GetDetailScale();
		_dataTexture = new GLubyte[4 * _nGrids*dbScale*dbScale];
	}

	switch (_bgFunction)
	{
	case MeteModel::bg_mean:
	case MeteModel::bg_Obs:
	case MeteModel::bg_vari:
	case MeteModel::bg_vari_smooth:
	case MeteModel::bg_dipValue:
	case MeteModel::bg_EOF:
	case MeteModel::bg_err:
		buildTextureColorMap();
		break;
	case MeteModel::bg_cluster:
		//		return generateClusteredVarianceTexture_old();
		buildTextureClusteredVariance();
		break;
	case MeteModel::bg_varThreshold:
		buildTextureThresholdVariance();
		break;
	case MeteModel::bg_dipValueThreshold:
		buildTextureThresholdDipValue();
		break;
	case MeteModel::bg_SDF:
		buildTextureSDF();
		break;
	case MeteModel::bg_ICDVX:
		buildTextureICDVX();
		break;
	case MeteModel::bg_ICDVY:
		buildTextureICDVY();
		break;
	case MeteModel::bg_ICDVW:
		buildTextureICDVW();
		break;
	case MeteModel::bg_IsoContourDensity:
		buildTextureICD();
		break;
	case MeteModel::bg_LineKernel:
		buildTextureICD_LineKernel();
		break;
	case MeteModel::bg_LineKernelX:
		buildTextureICDX();
		break;
	case MeteModel::bg_LineKernelY:
		buildTextureICDY();
		break;
	case MeteModel::bg_LineKernelZ:
		buildTextureICDZ();
		break;
	default:
		break;
	}

}

void MeteModel::SetBgFunction(enumBackgroundFunction f) 
{ 
	_bgFunction = f; 
	regenerateTexture();
}

void MeteModel::SetTransparency(int nT, int isoIndex)
{ 
	_pTimeStep->_listFeature[isoIndex]->SetTransparency(nT);
}

GLubyte* MeteModel::GenerateTexture() {
	if (!_dataTexture) {
		regenerateTexture();
	}
	return _dataTexture;
}

void MeteModel::readObsData() {

}

QList<ContourLine> MeteModel::GetContourMin(int isoIndex)  { return _pTimeStep->_listFeature[isoIndex]->GetContourMin() ; }
QList<ContourLine> MeteModel::GetContourMax(int isoIndex)  { return _pTimeStep->_listFeature[isoIndex]->GetContourMax() ; }
QList<ContourLine> MeteModel::GetContourMean(int isoIndex) { return _pTimeStep->_listFeature[isoIndex]->GetContourMean(); }


void MeteModel::BuildTess(int nTimeIndex, int isoIndex) {
	_arrTimeSteps[nTimeIndex]->_listFeature[isoIndex]->BuildTess();
}

void MeteModel::DrawContourBoxplots(int nTimeIndex, int isoIndex) {
	_arrTimeSteps[nTimeIndex]->_listFeature[isoIndex]->DrawContourBoxplots();
}

void MeteModel::DrawVariabilityPlots(bool bCluster, int nTimeIndex, int isoIndex) {
	_arrTimeSteps[nTimeIndex]->_listFeature[isoIndex]->DrawVariabilityPlots(bCluster?_nEnsCluster:-1);
}

void MeteModel::DrawContourProbabilityPlots(int nTimeIndex, int isoIndex) {	
	_arrTimeSteps[nTimeIndex]->_listFeature[isoIndex]->DrawContourProbabilityPlots();
}

void MeteModel::DrawIsoContourDensityPlots(int nTimeIndex, int isoIndex) {
	_arrTimeSteps[nTimeIndex]->_listFeature[isoIndex]->DrawIsoContourDensityPlots();
}

void MeteModel::SetIsoValues(QList<double> listIsoValues) {
	_listIsoValues = listIsoValues;
}

void MeteModel::updateTimeStep(int nTS) {
	qDebug() << "MeteModel::updateTimeStep" << nTS;
	_nTime = nTS;
	updateTimeStep();
	UpdateView();
}

int MeteModel::GetCurrentTimeIndex() {
	return 0;
	return (_nTime - GlobalSetting::g_nTimeStart) / 6;
}

void MeteModel::updateContourLevel0(int nLevel) { _arrContourLevel[0] = nLevel; UpdateView();
qDebug() << "updateContourLevel0";
}
void MeteModel::updateContourLevel1(int nLevel) { _arrContourLevel[1] = nLevel; UpdateView();}
void MeteModel::updateContourLevel2(int nLevel) { _arrContourLevel[2] = nLevel; UpdateView();}
void MeteModel::updateContourLevel3(int nLevel) { _arrContourLevel[3] = nLevel; UpdateView();}
void MeteModel::updateContourLevel4(int nLevel) { _arrContourLevel[4] = nLevel; UpdateView();}
void MeteModel::updateContourLevel5(int nLevel) { _arrContourLevel[5] = nLevel; UpdateView();}
void MeteModel::updateContourLevel6(int nLevel) { _arrContourLevel[6] = nLevel; UpdateView();}
void MeteModel::updateContourLevel7(int nLevel) { _arrContourLevel[7] = nLevel; UpdateView();}
void MeteModel::updateContourLevel8(int nLevel) { _arrContourLevel[8] = nLevel; UpdateView();}
void MeteModel::updateContourLevel9(int nLevel) { _arrContourLevel[9] = nLevel; UpdateView();}

void MeteModel::onOptimizing() {
	//qDebug() << "MeteModel::onOptimizing()";
}


void MeteModel::SetFocus(DPoint2 pt) {
	// qDebug() << "MeteModel::SetFocus";

	int nX = pt.x * 100;
	int nY = pt.y * 100;
	
	qDebug() << nX - _nWest << "," << nY - _nSouth;
}