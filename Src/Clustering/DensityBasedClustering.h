#pragma once
#include "Clustering.h"

namespace CLUSTER {
#define UNCLASSIFIED -1
#define NOISE -2
#define PROCESSED -3

#define CORE_POINT 1
#define NOT_CORE_POINT 0

#define SUCCESS 0
#define FAILURE -3

#define UNDEFINED -1

	struct point_t {
		double x, y, z;
		int cluster_id = UNCLASSIFIED;
		double _dbRDis = UNDEFINED;			// used for OPTICS
	};


	struct node_t {
		unsigned int index;
		node_t *next = 0;
		node_t(unsigned int uiIndex) { index = uiIndex; }
		double _dbCDis = UNDEFINED;		// core distance

	};


	struct epsilon_neighbours_t {
		unsigned int num_members = 0;
		node_t *head = 0;
		node_t *tail = 0;
		int append(unsigned int uiIndex,double dbCoreDis=UNDEFINED);
		~epsilon_neighbours_t();
	};

	double euclidean_dist(point_t *a, point_t *b);

	class DensityBasedClustering :
		public Clustering
	{
	public:
		DensityBasedClustering();
		virtual ~DensityBasedClustering();
	protected:
		// clustering parameters
		int _nMinPts = 100;
		double _dbEps = .5;
		// using weighted points
		bool _bWeighted = false;
	public:
		virtual void SetDBSCANParams(int minPts, double eps, bool bWeighted = false) { _nMinPts = minPts; _dbEps = eps; _bWeighted = bWeighted; };
	protected:
		// data used in implementation
		point_t *_points;
	protected:
		virtual void doCluster();
	protected:
		// implementation of algorithm
		virtual void run() = 0;
	protected:
		// get epsilon neighbor of the index
		epsilon_neighbours_t *get_epsilon_neighbours(unsigned int uiIndex);
		// calculate core distance for the point of index
		double calculateCoreDis(unsigned int uiIndex, epsilon_neighbours_t *neighbors);
	};
}

