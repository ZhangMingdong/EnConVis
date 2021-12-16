// ============================================================
// agglomerative hierarchical clustering (AHC)
// Written by Mingdong
// 2017/05/12
// ============================================================
#pragma once
#include "Clustering.h"
namespace CLUSTER {
	class AHCClustering : public Clustering
	{
	public:
		AHCClustering();
		virtual ~AHCClustering();
	protected:
		virtual void doCluster();
		// build the distance matrix
		void buildDistanceMatrix(double** disMatrix);
	private:
		// use the max or min distance between clusters
		void doClusterImpMaxOrMin(bool bMax = false);
		/*
			use the distance of the cluster center
			An error is found today, the centroid is errorly used.
			A remedy is made, using the index of the label to store the centroids
		*/
		void doClusterImp();
		// clean the resource
		void clean();

		// find the current nearest pair, parameter return the indices
		void findNearest(int& nI, int& nJ);

		// reset labels of the items whose label used to be nSource to nTarget
		void resetLabel(int nSourceLabel, int nTargetLabel);

		// reset distance matrix of target label according to accord to centroid
		void resetDistanceByCentroid(int nTargetLabel);

		// reset distance matrix of target label according to accord to minimum distance
		void resetDistanceByMinDis(int nTargetLabel);

		// reset distance matrix of target label according to accord to maximum distance
		void resetDistanceByMaxDis(int nTargetLabel);
	private:
		// distance matrix
		double** _disMatrix;
		// array of states
		int* _arrState;
		// centroid of each cluster[x0,y0,x1,y1,...]
		double* _arrCentroid;
	};
}
