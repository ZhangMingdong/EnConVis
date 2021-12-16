#pragma once
#include<math.h>
/*
	Basic math operations
	by Mingdong
	2020/06/23
*/
namespace MT {

	// calculate mean and variance
	void Statistics(double* arrSamples, int nLen, double& dbMean, double& dbVar) {
		double dbSum = 0;
		double dbVar2 = 0;
		for (size_t i = 0; i < nLen; i++)
		{
			dbSum += arrSamples[i];
		}
		dbMean = dbSum / nLen;

		for (size_t i = 0; i < nLen; i++)
		{
			double dbBias = arrSamples[i] - dbMean;
			dbVar2 += dbBias * dbBias;
		}

		dbVar = sqrt(dbVar2 / nLen);
	}

	// get interpolated Data at (x,y)
	double GetInterpolatedData(const double* pBuf, int nWidth, int nHeight, double dbX, double dbY) {
		int nX = dbX;
		int nY = dbY;
		double dx = dbX - nX;
		double dy = dbY - nY;

		double dbResult = (1 - dx) * (1 - dy) * pBuf[nY * nWidth + nX];
		if (nX < nWidth - 1) dbResult += dx * (1 - dy) * pBuf[nY * nWidth + nX + 1]; 
		if (nY < nHeight - 1) dbResult += (1 - dx) * dy * pBuf[(nY + 1) * nWidth + nX];
		if (nX < nWidth - 1 && nY < nHeight - 1) dbResult += dx * dy * pBuf[(nY + 1) * nWidth + nX + 1];
		return dbResult;
	}
}
