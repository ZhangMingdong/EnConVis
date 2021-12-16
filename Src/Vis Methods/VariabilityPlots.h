#pragma once

#include "GeneralPlots.h"
#include "def.h"
#include <QList>

class DataField;

/*
	this class used to perform the contour variability plots
	by Mingdong
	2019/06/11
*/
class VariabilityPlots:public GeneralPlots {

	// tesser
private:

	double* _gridBeliefMax;								// maximum of half valid ensemble results
	double* _gridBeliefMin;								// minimum of half valid ensemble results

	QList<UnCertaintyArea*> _listAreaBelief;			// list of the beleaf area
	QList<ContourLine> _listContourMinBelief;			// list of contours of minimum of valid members
	QList<ContourLine> _listContourMaxBelief;			// list of contours of maximum of valid members

	// variables for clusters
	const int* _cpLabels = NULL;									// labels of clustering
	int _nClusters = 0;												// number of clusters
	int _arrClusterLen[g_nClusterMax] = { 0,0,0,0,0,0,0,0,0,0 };	// length of each cluster

	double* _arrGridBeliefMax[g_nClusterMax];			// maximum of half valid ensemble results of each cluster
	double* _arrGridBeliefMin[g_nClusterMax];			// minimum of half valid ensemble results of each cluster
	double* _arrGridMean[g_nClusterMax];				// mean of each cluster



	QList<UnCertaintyArea*> _listAreaBelief_C[g_nClusterMax];		
	QList<ContourLine> _listContourMinBelief_C[g_nClusterMax];
	QList<ContourLine> _listContourMaxBelief_C[g_nClusterMax];
	QList<ContourLine> _listContourMean_C[g_nClusterMax];


	BandTesser _arrTesser[g_nClusterMax];

protected:
	// use pointwise statistics
	virtual void doStatistics();
	virtual void generateBands();
	virtual void generateContours();
	virtual void draw();
	virtual void init();

public:
	~VariabilityPlots();
	// build tessers
	void BuildTess();
	//
	void SetLabels(int nClusters, const int* cpLabels);

public:
	void DrawCluster(int nIndex);
};