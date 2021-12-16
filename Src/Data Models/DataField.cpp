#include "DataField.h"

#include <QList>
#include <QDebug>

#include <math.h>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "MyPCA.h"
#include "Switch.h"

double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0)
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2)
		return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (y - py) * (y - py));
}

DataField::DataField(int w, int h, int l)
{
	_nWidth = w;
	_nHeight = h;
	_nEnsembleLen = l;
	_nGrids = w * h;

	_pBuf = new double[w*h*l];
	_gridVari = new double[w*h];
	_gridMean = new double[w*h];
	_gridUMax = new double[w*h];
	_gridUMin = new double[w*h];




}

DataField::~DataField()
{
	delete[] _gridVari;
	delete[] _gridMean;
	delete[] _gridUMax;
	delete[] _gridUMin;
	delete[] _pBuf;



	for (size_t i = 0; i < _nSmooth; i++)
	{
		delete[] _gridVarSmooth[i];
	}


}

const double* DataField::GetLayer(int l) {
	return _pBuf + l* _nGrids;
}

double* DataField::GetEditableLayer(int l) {
	return _pBuf + l* _nGrids;
}

const double* DataField::GetVari(int nSmooth) {
	if (nSmooth==0)
	{
		return _gridVari;
	}
	else {
		for (size_t i= _nSmooth; i < nSmooth; i++)
		{
			smoothVar(i);
		}
		_nSmooth = nSmooth;
		return _gridVarSmooth[nSmooth-1];
	}
}

const double* DataField::GetEOF(int nSeq) {
	return NULL;// _gridEOF[nSeq];
}

const double* DataField::GetMean() { return _gridMean; }

void DataField::SetData(int l, int bias, double dbValue) {
	_pBuf[l*_nGrids + bias] = dbValue;
}

void DataField::SetData(int l, int y, int x, double dbValue) {
	_pBuf[l*_nGrids + y*_nWidth + x] = dbValue;
}

double DataField::GetData(int l, int bias) {
	return _pBuf[l*_nGrids + bias];
}

// get the range of the data
void DataField::GetDataRange(double& dbMin, double& dbMax) {
	dbMax = -10000;
	dbMin = 10000;
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		for (size_t j = 0; j < _nGrids; j++)
		{
			double dbSDF = _pBuf[i*_nGrids + j];
			if (dbSDF > dbMax) dbMax = dbSDF;
			if (dbSDF < dbMin) dbMin = dbSDF;
		}
	}
}

double DataField::GetData(int l, int r, int c) {
	return _pBuf[l*_nGrids + r* _nWidth + c];
}

void DataField::DoStatistic() {

	// 3.for each grid point
	for (int i = 0; i < _nGrids; i++)
	{
		// 3.1.calculate mean, min, and max
		double fMean = 0;
		double dbMin = 100000;
		double dbMax = -100000;
		for (int j = 0; j < _nEnsembleLen; j++)
		{
			double dbData= this->GetData(j, i);
			fMean += dbData;
			if (dbData > dbMax) dbMax = dbData;
			if (dbData < dbMin) dbMin = dbData;
		}

		fMean /= _nEnsembleLen;
		_gridMean[i] = fMean;

		_gridUMin[i] = dbMin;
		_gridUMax[i] = dbMax;
		
		// 3.2.calculate variance
		double fVariance = 0;
		for (int j = 0; j < _nEnsembleLen; j++)
		{
			double fBias = this->GetData(j, i) - fMean;
			fVariance += fBias*fBias;
		}
		_gridVari[i] = sqrt(fVariance / _nEnsembleLen);


	}


}

void DataField::GenerateClusteredData(const QList<int> listClusterLens, const int* arrLabels, QList<DataField*>& arrData) {
	// number of clusters
	int nClusters = listClusterLens.length();
	// generate data field according to the length of element in each cluster
	QList<int> listCurrentIndex;
	for (size_t i = 0; i < nClusters; i++)
	{
		arrData.push_back(new DataField(_nWidth, _nHeight, listClusterLens[i]));
		listCurrentIndex.push_back(0);
	}
	// set data for each new field
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		int nLabel = arrLabels[i];
		for (size_t j = 0; j < _nGrids; j++)
		{
			arrData[nLabel]->SetData(listCurrentIndex[nLabel], j, this->GetData(i, j));
		}
		listCurrentIndex[nLabel]++;
	}
	// do statistic for each new field
	for (size_t i = 0; i < nClusters; i++)
	{
		arrData[i]->DoStatistic();
	}
}

void DataField::smoothVar(int nSmooth) {
	const double* pVar = (nSmooth == 0) ? _gridVari : _gridVarSmooth[nSmooth - 1];

	double* _pVarNew= _gridVarSmooth[nSmooth] = new double[_nGrids];

	// smooth
	for (size_t i = 1; i < _nHeight-1; i++)
	{
		for (size_t j = 1; j < _nWidth-1; j++)
		{
			double dbVar = 0;
			for (int ii = -1; ii < 2; ii++)
			{
				for (int jj = -1; jj < 2; jj++) {
					dbVar += pVar[(i + ii)*_nWidth + j + jj];
				}
			}
			int nIndex = i* _nWidth + j;

			_pVarNew[nIndex] = dbVar / 9.0;
		}
	}

	// zero border
	for (size_t i = 0; i < _nHeight; i++)
	{
		_pVarNew[i*_nWidth + 0] = 0;
		_pVarNew[i*_nWidth + _nWidth -1] = 0;
	}
	for (size_t j = 1; j < _nWidth - 1; j++)
	{
		_pVarNew[j] = 0;
		_pVarNew[(_nHeight - 1)*_nWidth + j] = 0;
	}

}

DataField* DataField::GenerateSDF(double dbIsoValue
	, int nEnsembleLen
	, int nWidth
	, int nHeight
	, QList<QList<ContourLine>> listContour) {
	DataField* pSDF = new DataField(_nWidth, _nHeight, _nEnsembleLen);

	for (size_t l = 0; l < nEnsembleLen; l++) {
		// calculate sdf
		const double* arrData = this->GetData(l);
		double* arrSDF = pSDF->GetData(l);
		QList<ContourLine> contour = listContour[l];
		for (size_t i = 0; i < nHeight; i++)
		{
			for (size_t j = 0; j < nWidth; j++)
			{
				double dbMinDis = nHeight * nWidth;
				for (size_t k = 0, lenK = contour.length(); k < lenK; k++)
				{
					for (size_t l = 0, lenL = contour[k]._listPt.length() - 1; l < lenL; l++)
					{
						double dbDis = PointToSegDist(j, i
							, contour[k]._listPt[l].x(), contour[k]._listPt[l].y()
							, contour[k]._listPt[l + 1].x(), contour[k]._listPt[l + 1].y());
						if (dbDis < dbMinDis)
						{
							dbMinDis = dbDis;
						}

					}
				}
				if (arrData[i*nWidth + j] < dbIsoValue) dbMinDis = -dbMinDis;	// sign
				arrSDF[i*nWidth + j] = dbMinDis;
				//if (l == 0) qDebug() << dbMinDis;
			}
		}
	}
	pSDF->DoStatistic();
	return pSDF;
}

void DataField::ReadDataFromText(QString filename) {
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	// every ensemble member
	for (int i = 0; i < _nEnsembleLen; i++)
	{
		// skip first line
		QString line = in.readLine();
		// every grid
		for (int j = 0; j < _nGrids; j++)
		{
			QString line = in.readLine();
			SetData(i, j, line.toFloat());
		}
	}

	file.close();
}


