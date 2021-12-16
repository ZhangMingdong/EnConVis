#pragma once

/*
	Basic math operations
	by Mingdong
	2020/06/23
*/
namespace MT {

	// calculate mean and variance
	void Statistics(double* arrSamples, int nLen, double& dbMean, double& dbVar);

	// get interpolated Data at (x,y)
	double GetInterpolatedData(const double* pBuf, int nWidth, int nHeight, double dbX, double dbY);
}
