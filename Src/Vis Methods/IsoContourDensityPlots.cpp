#include "IsoContourDensityPlots.h"

#include "DataField.h"
#include <MathTypes.hpp>
#include <QDebug>

#include "Switch.h"


IsoContourDensityPlots::IsoContourDensityPlots()
{
}


IsoContourDensityPlots::~IsoContourDensityPlots()
{
	if(_pICD) delete[] _pICD;
	if (_dataTexture) delete[] _dataTexture;
}

void IsoContourDensityPlots::GenerateTexture() {

	int _nTexW = (_nWidth - 1) * _nDetailScale + 1;
	int _nTexH = (_nHeight - 1) * _nDetailScale + 1;
	glGenTextures(2, _uiTexID);


	glBindTexture(GL_TEXTURE_2D, _uiTexID[0]);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _nTexW, _nTexH, 0, GL_RGBA, GL_UNSIGNED_BYTE, _dataTexture);
}
void IsoContourDensityPlots::doStatistics() {


}
void IsoContourDensityPlots::generateBands() {

}
void IsoContourDensityPlots::generateContours() {

}
void IsoContourDensityPlots::draw() {
	double dbLeft = -1.8;
	double dbRight = 1.8;
	double dbBottom = -0.9;
	double dbTop = 0.9;
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, _uiTexID[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(dbLeft, dbBottom);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(dbRight, dbBottom);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(dbRight, dbTop);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(dbLeft, dbTop);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
void IsoContourDensityPlots::init() {
	if (!g_bICD) return;
	calculateICD();
	_dataTexture = new GLubyte[4 * _nGrids*_nDetailScale*_nDetailScale];


	int _nTexW = (_nWidth - 1) * _nDetailScale + 1;
	int _nTexH = (_nHeight - 1) * _nDetailScale + 1;
	const double* pData = _pICD;	// the data

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
// 感觉这个方法实现的还是不太对啊
void IsoContourDensityPlots::calculateICD() {
	_pICD = new double[((_nWidth - 1)*_nDetailScale + 1)*((_nHeight - 1)*_nDetailScale + 1)];
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
		double dbVar = 0;
		for (size_t l = 0; l < _nEnsembleLen; l++)
		{
			double dbValue = _pData->GetData(l, i);
			double dbBias = dbValue - dbMean;
			dbVar += dbBias * dbBias;
		}
		pMeans[i] = dbMean;
		pVari2[i] = dbVar;
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