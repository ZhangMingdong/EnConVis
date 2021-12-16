// ============================================================
// DBSCAN clustering
// Written by Mingdong
// do not use clustering interface
// 2017/05/26
// ============================================================
#pragma once
#include "DensityBasedClustering.h"
namespace CLUSTER {
	class DBSCANClustering : public DensityBasedClustering
	{
	public:
		DBSCANClustering();
		virtual ~DBSCANClustering();



	protected:
		// implementation of dbscan algorithm
		virtual void run();
		/* 
			expand form a unclassified point, 
			input: the index of the point to expand, the curent cluster id
			return the state of this point
		*/
		int expand(unsigned int index, unsigned int cluster_id);

		// spread the cluster id for this index
		int spread(
			unsigned int index,
			epsilon_neighbours_t *seeds,
			unsigned int cluster_id);




		// get the weight of its neighbors
		double getNeighborWeight(unsigned uiIndex,epsilon_neighbours_t* neighbors);


	};

}