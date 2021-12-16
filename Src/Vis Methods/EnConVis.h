#pragma once
#include "GeneralPlots.h"
#include "def.h"
/*
	this class used to perform the reconplot
	by Mingdong
	2020/06/16
*/
class EnConVis :
	public GeneralPlots
{
public:
	EnConVis();
	virtual ~EnConVis();

protected:
	// use pointwise statistics
	virtual void doStatistics();
	virtual void generateBands();
	virtual void generateContours();
	virtual void draw();
	virtual void init();


	// sort source to target
	void sortBuf(const double* pS, double* pD);

	// resample buffer
	void resampleBuf(const double* pSortedBuf, double* pResampledBuf, int nResampleLen);

	// resample buffers for each cluster
	void resampleBuf_C(const double* pUnSortedBuf, const int* pLabels, double* pResampledBuf, int nClusters, int nResampleLen);	// resample buffer for each clusters


	double* getSortedData(int l) { return _pSortedBuf + l * _nGrids; };
	double* getResampledData(int l) { return _pResampledBuf + l * _nGrids; };
	double* getResampledForCluster(int nCluster, int nIndex) {
		return _arrResampledBuf_C + _nGrids * c_nResampleLen_C*nCluster + _nGrids * nIndex;
	}

private:
	const int  c_nResampleLen = 65;		// length of resample 2^6-1
	// transformed data
	double* _pSortedBuf = NULL;			// data sorted at each grid point
	double* _pResampledBuf = NULL;		// resampled buffer

	const int  c_nResampleLen_C = 65;	// length of resample of cluster 2^4-1
	double* _arrResampledBuf_C = NULL;	// resampled buffers for each cluster
private:
	QList<QList<ContourLine>> _listContourSorted;							// list of sorted contours
	QList<QList<ContourLine>> _listContourResampled;						// list of resampled contours
	QList<QList<ContourLine>> _listContourResampled_C[g_nClusterMax];		// list of resampled contours for each cluster,11 for each

	QList<QList<UnCertaintyArea*>> _listArea;								// uncertain bands				
	QList<QList<UnCertaintyArea*>> _listArea_C[g_nClusterMax];				// uncertain bans for each cluster

	QList<BandTesser> _arrTesser_C[g_nClusterMax];							// tesser for each cluster

	int _arrRepresentativeIndices_C[g_nClusterMax];							// representative indices for each clusters
	// variables for clusters
	const int* _cpLabels = NULL;											// labels of clustering
	int _nClusters = 0;														// number of clusters
	int _arrClusterLen[g_nClusterMax] = { 0,0,0,0,0,0,0,0,0,0 };			// length of each cluster


private:
	void drawResampledContours(const QList<QList<ContourLine>>& contours,int nIndex0, int nIndex1, int nLevel);
public:
	// draw sorted contours
	void DrawSortedContours(int nLevel);
	// draw resampled contours
	void DrawResampledContours(int nLevel);
	// draw uncertain bands
	void DrawBands(int nLevel);
	// draw uncertain bands for clusters
	void DrawBands_C(int nLevel, int nCluster);


	void SetLabels(int nClusters, const int* cpLabels);
	void GenerateContoursForCluster();

	void DrawContoursResampledForClusters(int nLevel, int nCluster);
	// build tessers
	void BuildTess();


};
