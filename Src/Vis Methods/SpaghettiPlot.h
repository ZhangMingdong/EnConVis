#pragma once
#include "GeneralPlots.h"
#include "def.h"
/*
	this class used to perform the spaghetti plots
	by Mingdong
	2020/07/02
*/
class SpaghettiPlot :
	public GeneralPlots
{
public:
	SpaghettiPlot();
	virtual ~SpaghettiPlot();

protected:

	QList<QList<ContourLine>> _listContour;

	int _nTransparency = 1;
	int  _nClusters = 5;					// number of clusters
	const int* _cpLabels = NULL;											// labels of clustering
protected:

	virtual void doStatistics();
	virtual void generateBands();
	virtual void generateContours();
	virtual void draw();
	virtual void init();
public:
	// draw original contours, given member or cluster
	void DrawContours(int nMember, int nCluster, bool bCluster);
	QList<QList<ContourLine>> GetContours() { return _listContour; }

	void SetTransparency(int nT) { _nTransparency = nT; };
	void SetLabels(int nClusters, const int* cpLabels);
};
