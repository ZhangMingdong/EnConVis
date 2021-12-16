#include "stdafx.h"
#include "KMeansClustering.h"




namespace CLUSTER {

	KMeansClustering::KMeansClustering()
	{
	}


	KMeansClustering::~KMeansClustering()
	{
	}

	void KMeansClustering::doCluster() {
		_arrCenters = new double[_m*_k];
		_arrClusterLen = new int[_k];
		// initiate the centers
		for (size_t i = 0; i < _k; i++)
		{
			vectorCopy(_arrCenters + i*_m, _arrData + i*_m, _m);
		}
		// loop
		int nLoops = 1000;
		for (size_t ii = 0; ii < nLoops; ii++)
		{
			// find nearest center for each item
			for (size_t i = 0; i < _n; i++)
			{
				int nNearestIndex = -1;
				double dbMinDistance = -1;
				for (size_t j = 0; j < _k; j++) {
					double dis = vectorDis(_arrCenters + j*_m, _arrData + i*_m, _m);
					if (nNearestIndex == -1 || dis < dbMinDistance)
					{
						nNearestIndex = j;
						dbMinDistance = dis;
					}
				}
				_arrLabels[i] = nNearestIndex;
			}
			// recalculate the center
			for (size_t i = 0; i < _k; i++)
			{
				vectorReset(_arrCenters + i*_m, _m);
				_arrClusterLen[i] = 0;
			}
			for (size_t i = 0; i < _n; i++)
			{
				int nClusterIndex = _arrLabels[i];
				_arrClusterLen[nClusterIndex]++;
				vectorAdd(_arrCenters + nClusterIndex*_m, _arrData + i*_m, _m);
			}
			for (size_t i = 0; i < _k; i++)
			{
				vectorDiv(_arrCenters + i*_m, _arrClusterLen[i], _m);
			}
		}

		delete[] _arrCenters;
		delete[] _arrClusterLen;
	}


}