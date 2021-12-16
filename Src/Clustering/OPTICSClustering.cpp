#include "stdafx.h"
#include "OPTICSClustering.h"
#include <algorithm>

namespace CLUSTER {

	OPTICSClustering::OPTICSClustering()
	{
	}


	OPTICSClustering::~OPTICSClustering()
	{
	}

	void OPTICSClustering::run() {
		// 0.Initialization
		for (size_t i = 0; i < _n; i++)
		{
			_points[i]._dbRDis = UNDEFINED;
		}

		// 1.optics
		for (size_t i = 0; i < _n; i++)
		{
			// for each unprocessed point p of DB
			if (_points[i].cluster_id == UNCLASSIFIED) {
				// 1.try to get the eps neighbors of this point
				epsilon_neighbours_t *seeds = get_epsilon_neighbours(i);
				_points[i].cluster_id = PROCESSED;
				_vecOrderedList.push_back(i);

				if (calculateCoreDis(i,seeds)>0)	// coreDis != UNDEFINED
				{
					std::vector<unsigned int> vecPriority;
					update(seeds, i, vecPriority);
					// update
					/*
					Seeds = empty priority queue
						update(N, p, Seeds, eps, Minpts)
						for each next q in Seeds
							N' = getNeighbors(q, eps)
							mark q as processed
							output q to the ordered list
							if (core - distance(q, eps, Minpts) != UNDEFINED)
								update(N', q, Seeds, eps, Minpts)
								*/
				}
			}
		}

	}

	void OPTICSClustering::update(epsilon_neighbours_t *neighbors, unsigned int uiIndex, std::vector<unsigned int>& vecPriority) {
		double dbCDis = calculateCoreDis(uiIndex, neighbors);
		node_t* h = neighbors->head;
		while (h != neighbors->tail) {
			if (_points[h->index].cluster_id == PROCESSED) {
				double dbNewRDis = std::max(dbCDis, euclidean_dist(_points + uiIndex, _points + h->index));
				if (_points[h->index]._dbRDis < 0 /*==UNDEFINED*/) // o is not in Seeds
				{
					_points[h->index]._dbRDis = dbNewRDis;
					vecPriority.push_back(h->index);
				}
				else               // o in Seeds, check for improvement
					if (dbNewRDis < _points[h->index]._dbRDis)
					{
						_points[h->index]._dbRDis = dbNewRDis;
					}
			}
			h = h->next;
		}
	}
}