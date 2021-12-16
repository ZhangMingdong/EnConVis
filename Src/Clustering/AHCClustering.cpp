#include "stdafx.h"
#include "AHCClustering.h"
#include <math.h>

#include <iostream>


namespace CLUSTER {


	AHCClustering::AHCClustering()
	{
	}


	AHCClustering::~AHCClustering()
	{
	}

	void AHCClustering::doCluster() {
		// 0.initialize the distance matrix
		_disMatrix = new double*[_n];
		buildDistanceMatrix(_disMatrix);
		// 1.initialized the centroid
		_arrCentroid = new double[_n * _m];
		for (size_t i = 0; i < _n*_m; i++)
		{
			_arrCentroid[i] = _arrData[i];
		}

		// 2.clustering
		for (int i = 0; i < _n; i++)
		{
			_arrLabels[i] = i;
		}
		// state of each grid point
		_arrState = new int[_n];
		for (int i = 0; i < _n; i++) _arrState[i] = 1;	// 0 means this grid has been merged to other cluster

		bool bNewImp = true;
		if (bNewImp)
		{
			doClusterImp();
		}
		else {
			doClusterImpMaxOrMin();
		}


		// 3.release the resource
		clean();

		//	realign the label
		AlignLabels(_arrLabels, _n);
		/*
		int nBias = 0;
		for (int i = 0; i < _n; i++)
		{
			// if i is a valid label
			bool bValidLabel = false;
			for (int j = 0; j < _n; j++)
			{
				if (_arrLabels[j] == i) {
					_arrLabels[j] -= nBias;
					bValidLabel = true;
				}
			}
			if (!bValidLabel) nBias++;
		}*/
	}

	void AHCClustering::doClusterImpMaxOrMin(bool bMax) {
		// start from every grid point as a cluster
		int nClusters = _n;

		while (nClusters > _k)
		{
			// 2.1.Find the nearest points pair nI,nJ
			int nI, nJ;
			double fMinDis = 10000.0;
			for (int i = 0; i < _n; i++)
			{
				if (_arrState[i] == 0) continue;

				for (int j = 0; j < i; j++)
				{
					if (_arrState[j] == 0) continue;
					if (_arrLabels[i] == _arrLabels[j]) continue;
					if (_disMatrix[i][j] < fMinDis)
					{
						nI = i;
						nJ = j;
						fMinDis = _disMatrix[i][j];
					}
				}
			}

			int nSourceLabel = _arrLabels[nI];
			int nTargetLabel = _arrLabels[nJ];


			// 2.2.search all the point with source label, reset distance and state
			for (int i = 0; i < _n; i++)
			{
				if (_arrLabels[i] == nSourceLabel)
				{
					// 2.2.1.reset the label
					_arrLabels[i] = nTargetLabel;
					if (_arrState[i] == 1)
					{
						// 2.2.2.reset distance
						for (int j = 0; j < _n; j++)
						{
							if (j == i || j == nJ) continue;
							// as we merge i to nJ
							// for each j, if dis(i,j)>dis(nJ,j), we use dis(i,j) to replace dis(nJ,j)
							double fDisIJ;
							if (i < j) fDisIJ = _disMatrix[j][i];
							else fDisIJ = _disMatrix[i][j];
							if (j < nJ) {
								_disMatrix[nJ][j] = (bMax ? fmax(_disMatrix[nJ][j], fDisIJ) : fmin(_disMatrix[nJ][j], fDisIJ));
							}
							else {
								_disMatrix[j][nJ] = (bMax ? fmax(_disMatrix[j][nJ], fDisIJ) : fmin(_disMatrix[j][nJ], fDisIJ));
							}
						}
						_arrState[i] = 0;
					}
				}
			}
			nClusters--;
		}
	}

	void AHCClustering::findNearest(int& nI, int& nJ) {
		double fMinDis = 1000000.0;
		for (int i = 0; i < _n; i++)
		{
			if (_arrState[i] == 0) continue;

			for (int j = 0; j < i; j++)
			{
				if (_arrState[j] == 0) continue;
				if (_arrLabels[i] == _arrLabels[j]) continue;
				if (_disMatrix[i][j] < fMinDis)
				{
					nI = i;
					nJ = j;
					fMinDis = _disMatrix[i][j];
				}
			}
		}
	}

	void AHCClustering::resetLabel(int nSourceLabel, int nTargetLabel) {
		// 2.search all the point with source label, reset the label
		for (int i = 0; i < _n; i++)
		{
			if (_arrLabels[i] == nSourceLabel)
			{
				// 2.2.1.reset the label
				_arrLabels[i] = nTargetLabel;
				_arrState[i] = 0;
			}
		}
	}
	void AHCClustering::resetDistanceByMinDis(int nTargetLabel) {
		for (int i = 0; i < _n; i++)
		{
			if (_arrState[i] > 0 && _arrLabels[i] != nTargetLabel) {
				int index1 = i;
				int index2 = nTargetLabel;
				if (i < nTargetLabel)
				{
					index1 = nTargetLabel;
					index2 = i;
				}
				double dbMinDis = 100000000000;
				for (int j = 0; j < _n; j++)
				{
					for (int k = 0; k < _n; k++) {
						if ((_arrLabels[j] == index1 && _arrLabels[k] == index2) || (_arrLabels[j] == index2 && _arrLabels[k] == index1)) {
							double dbDis = vectorDis(_arrData + j * _m, _arrData + k * _m, _m);
							if (dbDis < dbMinDis) dbMinDis = dbDis;
						}
					}
				}
				_disMatrix[index1][index2] = dbMinDis;
			}
		}
	}


	void AHCClustering::resetDistanceByMaxDis(int nTargetLabel) {
		for (int i = 0; i < _n; i++)
		{
			if (_arrState[i] > 0 && _arrLabels[i] != nTargetLabel) {
				int index1 = i;
				int index2 = nTargetLabel;
				if (i < nTargetLabel)
				{
					index1 = nTargetLabel;
					index2 = i;
				}
				double dbMaxDis = 0;
				for (int j = 0; j < _n; j++)
				{
					for (int k = 0; k < _n; k++) {
						if ((_arrLabels[j] == index1 && _arrLabels[k] == index2) || (_arrLabels[j] == index2 && _arrLabels[k] == index1)) {
							double dbDis = vectorDis(_arrData + j * _m, _arrData + k * _m, _m);
							if (dbDis > dbMaxDis) dbMaxDis = dbDis;
						}
					}
				}
				_disMatrix[index1][index2] = dbMaxDis;
			}
		}
	}

	void AHCClustering::resetDistanceByCentroid(int nTargetLabel)
	{
		// 1.calculate centroid
		vectorReset(_arrCentroid + nTargetLabel * _m, _m);
		int num = 0;
		for (int i = 0; i < _n; i++)
		{
			if (_arrLabels[i] == nTargetLabel)
			{
				vectorAdd(_arrCentroid + nTargetLabel * _m, _arrData + i * _m, _m);
				num++;
			}
		}
		vectorDiv(_arrCentroid + nTargetLabel * _m, num, _m);
		// 2.calculate distance
		for (int i = 0; i < _n; i++)
		{
			if (_arrState[i] > 0 && _arrLabels[i] != nTargetLabel) {
				int index1 = i;
				int index2 = nTargetLabel;
				if (i < nTargetLabel)
				{
					index1 = nTargetLabel;
					index2 = i;
				}
				_disMatrix[index1][index2] = vectorDis(_arrCentroid + _arrLabels[index1] * _m, _arrCentroid + _arrLabels[index2] * _m, _m);
			}
		}
	}

	void AHCClustering::doClusterImp() {
		// start from every grid point as a cluster
		int nClusters = _n;

		while (nClusters > _k)
		{
			// 1.Find the nearest points pair nI,nJ
			int nI = -1;
			int nJ = -1;
			findNearest(nI, nJ);

			// 2.Get the merge labels and record them
			int nSourceLabel = _arrLabels[nI];
			int nTargetLabel = _arrLabels[nJ];
			int nCurrentRount = _n - nClusters;
			_arrMergeSource[nCurrentRount] = nI;
			_arrMergeTarget[nCurrentRount] = nJ;
			//std::cout << nI << "(" << _arrLabels[nI] << ")"<<  "->" << nJ <<"("<< _arrLabels[nJ] <<")"<< std::endl;

			// 3.search all the point with source label, reset the label
			resetLabel(nSourceLabel, nTargetLabel);

			// 4.reset the distance
			//resetDistanceByMaxDis(nTargetLabel);
			//resetDistanceByMinDis(nTargetLabel);
			resetDistanceByCentroid(nTargetLabel);
			
			nClusters--;
		}
	}

	void AHCClustering::buildDistanceMatrix(double** disMatrix) {
		// 0.allocate resource
		for (int i = 0; i < _n; i++)
		{
			disMatrix[i] = new double[_n];
		}
		// 1.calculate the distance
		for (int i = 0; i < _n; i++)
		{
			for (int j = 0; j < i; j++)
			{

				disMatrix[i][j] = vectorDis(_arrData + i*_m, _arrData + j*_m, _m);
			}
		}
	}

	void AHCClustering::clean() {
		for (int i = 0; i < _n; i++)
		{
			delete[]_disMatrix[i];
		}
		delete[] _disMatrix;
		delete[] _arrState;
		delete[] _arrCentroid;
	}

}


