#pragma once
#include "DensityBasedClustering.h"

#include <vector>

namespace CLUSTER {

	class OPTICSClustering :
		public DensityBasedClustering
	{
	public:
		OPTICSClustering();
		virtual ~OPTICSClustering();
	protected:
		// implementation of dbscan algorithm
		virtual void run();

	protected:
		// ordered list
		std::vector<unsigned int> _vecOrderedList;
		std::vector<int> _vecPriority;
	private:
		// update in the algorithm
		void update(epsilon_neighbours_t *neighbors, unsigned int uiIndex, std::vector<unsigned int>& vecPriority);
	};

}
