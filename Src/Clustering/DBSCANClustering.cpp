#include "stdafx.h"
#include "DBSCANClustering.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>



namespace CLUSTER {


	DBSCANClustering::DBSCANClustering()
	{
	}

	DBSCANClustering::~DBSCANClustering()
	{
	}

	void DBSCANClustering::run()
	{
		// id start from 0
		unsigned int cluster_id = 0;
		// traverse the points
		for (unsigned int i = 0; i < _n; ++i) {
			// handle the unclassified points only
			if (_points[i].cluster_id == UNCLASSIFIED) {
				// expand this point, if it return to be a core point, increase the id
				if (expand(i, cluster_id) == CORE_POINT)
					++cluster_id;
			}
		}
	}

	/*
		expand form a unclassified point,
		input: the index of the point to expand, the curent cluster id
		return the state of this point
	*/
	int DBSCANClustering::expand(unsigned int index, unsigned int cluster_id)
	{
		int return_value = NOT_CORE_POINT;

		// 1.try to get the eps neighbors of this point
		epsilon_neighbours_t *seeds = get_epsilon_neighbours(index);

		// 2.check whether the point is core point or noise according to the number of its eps neighbors
		if (getNeighborWeight(index, seeds) < _nMinPts)
			_points[index].cluster_id = NOISE;
		else {
			// 2.1.Set id for the point
			_points[index].cluster_id = cluster_id;
			// 2.2.Set id for its eps neighbors
			node_t *h = seeds->head;
			while (h) {
				_points[h->index].cluster_id = cluster_id;
				h = h->next;
			}

			// 2.3.Continue to spread from its eps neighbors
			h = seeds->head;
			while (h) {
				spread(h->index, seeds, cluster_id);
				h = h->next;
			}

			return_value = CORE_POINT;
		}

		// 3.destroy the neighbor data structure
		delete seeds;
		return return_value;
	}

	int DBSCANClustering::spread(unsigned int index, epsilon_neighbours_t *seeds, unsigned int cluster_id)
	{
		// 0.get the epsilon neighbors of this index
		epsilon_neighbours_t *spread = get_epsilon_neighbours(index);

		// 1.handle if this point is not noise
		if (getNeighborWeight(index,spread) >= _nMinPts) {
			// handle its neighbors
			node_t *n = spread->head;
			while (n) {
				// get this neighbor
				point_t *d = _points + n->index;
				if (d->cluster_id == NOISE){					// for noise point
					// just set id for it
					d->cluster_id = cluster_id;
				}
				else if (d->cluster_id == UNCLASSIFIED) {		// for unclassified point
					// set id for it, and append it in the tail of the seeds
					seeds->append(n->index);
					d->cluster_id = cluster_id;
				}
				n = n->next;
			}
		}

		// 2.release the resource
		delete spread;
		return SUCCESS;
	}
	
	double DBSCANClustering::getNeighborWeight(unsigned uiIndex, epsilon_neighbours_t* neighbors) {
		if (_bWeighted)
		{
			double dbWeight = 0;
			node_t *n = neighbors->head;
			while (n) {
				dbWeight += _points[n->index].z;
				n = n->next;
			}
			dbWeight += _points[uiIndex].z;
			return dbWeight;
		}
		else return neighbors->num_members;
	}
}