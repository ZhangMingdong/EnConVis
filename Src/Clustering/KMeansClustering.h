// ============================================================
// k-means clustering
// Written by Mingdong
// 2017/05/21
// ============================================================
#pragma once
#include "Clustering.h"
namespace CLUSTER {
	class KMeansClustering : public Clustering
	{
	public:
		KMeansClustering();
		virtual ~KMeansClustering();
	protected:
		virtual void doCluster();
	private:
		// cluster centers
		double* _arrCenters;
		// length of each cluster
		int* _arrClusterLen;
	};

}