#include "stdafx.h"
#include "Clustering.h"
#include <math.h>




namespace CLUSTER {

	Clustering::Clustering()
	{
	}


	Clustering::~Clustering()
	{
	}

	int Clustering::DoCluster(int n, int m, int k, const double *arrData, int* arrLabels, int *arrMergeSource, int* arrMergeTarget) {
		_n = n;
		_m = m;
		_k = k;
		_arrData = arrData;
		_arrLabels = arrLabels;
		_arrMergeSource = arrMergeSource;
		_arrMergeTarget = arrMergeTarget;
		doCluster();
		return _k;
	}

	void Clustering::AlignLabels(int* arrLabesl, int nLen) {
		int nBias = 0;
		for (int i = 0; i < nLen; i++)
		{
			// if i is a valid label
			bool bValidLabel = false;
			for (int j = 0; j < nLen; j++)
			{
				if (arrLabesl[j] == i) {
					arrLabesl[j] -= nBias;
					bValidLabel = true;
				}
			}
			if (!bValidLabel) nBias++;
		}
	}

	// add vector v to vector vSum
	void vectorAdd(double* vSum, const double* v, int nLen) {
		for (size_t i = 0; i < nLen; i++)
		{
			vSum[i] += v[i];
		}
	}

	// divide a vector
	void vectorDiv(double* v, double dbScale, int nLen) {
		for (size_t i = 0; i < nLen; i++)
		{
			v[i] /= dbScale;
		}
	}

	// reset a vector
	void vectorReset(double*v, int nLen) {
		for (size_t i = 0; i < nLen; i++)
		{
			v[i] = 0;
		}
	}

	// calculate distance between the two vectors
	double vectorDis(const double *v1, const double* v2, int nLen) {
		double sum = 0;

		for (size_t i = 0; i < nLen; i++)
		{
			double bias = v1[i] - v2[i];
			sum += bias*bias;
		}
		return sqrt(sum);
	}


	// copy vector v1 to v0
	void vectorCopy(double* v0, const double* v1, int nLen) {
		for (size_t i = 0; i < nLen; i++)
		{
			v0[i] = v1[i];
		}
	}
}