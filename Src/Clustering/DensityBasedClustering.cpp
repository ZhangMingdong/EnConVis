#include "stdafx.h"
#include "DensityBasedClustering.h"
#include <stdlib.h>

#include <math.h>
#include <vector>
#include <algorithm>


namespace CLUSTER {
	int sortFun(node_t* n1, node_t* n2) {
		return n2->_dbCDis - n1->_dbCDis;
	}

	int epsilon_neighbours_t::append(unsigned int uiIndex, double dbCoreDis)
	{
		node_t *n = new node_t(uiIndex);
		n->_dbCDis = dbCoreDis;

		if (this->head == NULL) {
			this->head = n;
			this->tail = n;
		}
		else {
			this->tail->next = n;
			this->tail = n;
		}
		++(this->num_members);
		return SUCCESS;
	}

	epsilon_neighbours_t::~epsilon_neighbours_t()
	{
		node_t *t, *h = this->head;
		while (h) {
			t = h->next;
			delete h;
			h = t;
		}
	}

	double euclidean_dist(point_t *a, point_t *b)
	{
		return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
	}


	DensityBasedClustering::DensityBasedClustering()
	{
	}


	DensityBasedClustering::~DensityBasedClustering()
	{
	}

	void DensityBasedClustering::doCluster() {
		_points = new point_t[_n];

		for (size_t i = 0; i < _n; i++)
		{
			if (_bWeighted)
			{
				_points[i].x = _arrData[(_m + 1)*i];
				_points[i].y = _arrData[(_m + 1)*i + 1];
				_points[i].z = _arrData[(_m + 1)*i + 2];

			}
			else {
				_points[i].x = _arrData[_m*i];
				_points[i].y = _arrData[_m*i + 1];
			}
			_points[i].cluster_id = UNCLASSIFIED;
		}


		run();

		_k = 0;
		for (size_t i = 0; i < _n; i++)
		{
			_arrLabels[i] = _points[i].cluster_id;
			if (_arrLabels[i] > _k) _k = _arrLabels[i];
		}
		_k++;

		delete[] _points;
	}


	epsilon_neighbours_t *DensityBasedClustering::get_epsilon_neighbours(unsigned int uiIndex)
	{
		epsilon_neighbours_t *en = new epsilon_neighbours_t;

		for (int i = 0; i < _n; ++i)
			if (i != uiIndex) {
				double dbCoreDis = euclidean_dist(&_points[uiIndex], &_points[i]);
				if (dbCoreDis < _dbEps)
					en->append(i, dbCoreDis);
			}
		return en;
	}


	double DensityBasedClustering::calculateCoreDis(unsigned int uiIndex, epsilon_neighbours_t *neighbors) {
		if (neighbors->num_members < _nMinPts) return UNDEFINED;

		std::vector<node_t*> list;
		node_t* h = neighbors->head;
		while (h) {
			list.push_back(h);
			h = h->next;
		}

		std::sort(list.begin(), list.end(), sortFun);
		return list[_nMinPts - 1]->_dbCDis;
	}

}