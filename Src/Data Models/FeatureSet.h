#pragma once
#include <QList>
#include "BasicStruct.h"
#include "GridFrame.h"
#include "DataField.h"

// vis methods
#include "SpaghettiPlot.h"
#include "ContourBoxplots.h"
#include "VariabilityPlots.h"
#include "ContourProbabilityPlots.h"
#include "EnConVis.h"
#include "IsoContourDensityPlots.h"


class DataField;

/*
	This class used to capsulate the features generated by an isovalue
	Mingdong
	2018/12/04
*/
class FeatureSet:public GridFrame
{
public:
	FeatureSet(DataField* pData,double _dbIsoValue,int nWidth,int nHeight,int _nEnsembleLen);
	~FeatureSet();
private:
	// 1.setting
	double _dbIsoValue = 0;					// isovalue of this feature set;
	// 2.dataset
	DataField* _pData = NULL;				// reference to the data set
	DataField* _pSDFData = NULL;			// the sdf field
	// 3.PC
	int _nPCLen = 2;						// length of PC
	double* _arrPC;							// array of PCs
	// 4.cluster
	int  _nClusters;						// number of clusters, use profile setting
	int* _arrLabels;						// labels of each member
	int* _arrMergeTarget;					// array of the merge target
	int* _arrMergeSource;					// array of the merge source
	int _arrClusterLen[5];					// length of each cluster
	

	// 5.Vis Method

	QList<QList<ContourLine>> _listContour;									// list of contours	

	SpaghettiPlot _spaghettiPlot;
	ContourBoxplots _cbPlots;
	VariabilityPlots _vbPlots;
	ContourProbabilityPlots _cpPlots;
	EnConVis _rcPlotsSDF;
	EnConVis _rcPlotsDomain;
	IsoContourDensityPlots _cdPlots;
	// 6.heatmap
	int _nDetailScale = 10;					// scale of detail texture

	bool* _pGridDiverse;					// diversed grid points
	int _nDiverseCount = 0;					// count of diverse grids

	QList<ContourLine> _listContourMinE;	// list of contours of minimum of E
	QList<ContourLine> _listContourMaxE;	// list of contours of maximum of E
	QList<ContourLine> _listContourMeanE;	// list of contours of mean of E



public:
	QList<ContourLine>& GetContourMin() { return _listContourMinE; }
	QList<ContourLine>& GetContourMax() { return _listContourMaxE; }
	QList<ContourLine>& GetContourMean(){ return _listContourMeanE; }
	
	
	const double* GetSDF() { 
		return _pSDFData->GetData();
		//return _pSDF; 
	}



	int GetDetailScale() { return _nDetailScale; }
	int GetLabel(int l);
	int GetMergeTarget(int l) { return _arrMergeTarget[l]; }
	int GetMergeSource(int l) { return _arrMergeSource[l]; }
	double GetPC(int l,int nIndex) { return _arrPC[l * _nPCLen+nIndex]; }
	int GetClusters() { return _nClusters; }
	void SetClustersLen(int nClustersLen);

	void SetTransparency(int nT);

private:

	double* getSDF(int l) { 
		return _pSDFData->GetData(l);
		//return _pSDF + l * _nGrids; 
	}
// basic operation
private:
		
	void generateContours();
	/*
		calculate similarity between two members;
		using mutual information	
		write these function according to paper "Isosurface Similarity Maps"
		date: 2019/01/27
		paras:
			l1,l2: index of the two member
			min,max: min and max of the sdf
	*/
	double calculateSimilarityMI(int l1, int l2,double dbMin, double dbMax);	


	/*
		calculate distance between two members;
		using monte carlo
		date: 2019/12/28
		paras:
			l1,l2: index of the two member
		results:
			0-1;
	*/
	double calculateDistanceMC(int l1, int l2);
public:

// dimension-reduction and clustering
private:
	void calculateDiverse();			
	void calculatePCA();					// calculate pca
	void calculatePCA_Whole();				// calculate pca
	void calculatePCA_MDS();				// calculate pca using MDS
	void calculatePCA_MDS_Set();			
	void calculatePCA_MDS_Whole();			// calculate pca using MDS of the whole grids
	void calculatePCA_MDS_Whole_Density();	// calculate pca using MDS of the whole grids by density
	void calculatePCA_MutualInformation();	// calculate pca using mutual information
	void calculatePCA_Fofonov();			// calculate pca using Fofonov's method
	void doClustering();					// clustering
	void doPCAClustering();					// clustering based on pca result
	double* createDiverseArray();			// create diverse grids array, the result should be manually freed.
	void calculatePCARecovery();			// test, recovering,calculate the box of pca
	void calculatePCABox();					// test, recovering,calculate the box of pca


//=============================density================================
private:
	double* _pICD;					// data of iso-contour density, using kde
	double* _pICDVX;				// data of iso-contour density vector
	double* _pICDVY;				// data of iso-contour density vector
	double* _pICDVW;				// data of iso-contour density vector
	double* _pICD_LineKernel;		// data of iso-contour density using line kernel
	double* _pICDX;					// data of iso-contour density using line kernel X
	double* _pICDY;					// data of iso-contour density using line kernel Y
	double* _pICDZ;					// data of iso-contour density using line kernel Z

private:
	void calculateICD();			// calculate _pICD
	void buildICD_LineKernel();		// build iso-contour density using line kernel
	void buildICD_Vector();			// build iso-contour density using vector kernel
	void buildICDV();
		
	void resetLabels();				// reset labels after reset number of clusters
public:
	const double* GetICD() { return _pICD; }

	const double* GetICD_LineKernel() { return _pICD_LineKernel; }
	const double* GetICDX() { return _pICDX; }
	const double* GetICDY() { return _pICDY; }
	const double* GetICDZ() { return _pICDZ; }
	const double* GetSDF(int l) const { 
		return _pSDFData->GetData(l);
		//return _pSDF + l * _nGrids; 
	}
	const double* GetICDVX(int l) const { return _pICDVX + l * _nGrids; }
	const double* GetICDVY(int l) const { return _pICDVY + l * _nGrids; }
	const double* GetICDVW(int l) const { return _pICDVW + l * _nGrids; }

private:	// information loss measure

	// get the value of the point in the field
	double getFieldValue(int nX, int nY, double dbX, double dbY, const double* pField);	
	int _nTestSamples=50;
	int _arrRandomIndices[50];
	void generateRandomIndices();	
	void generateCentralIndices();
	void generateSequentialIndices();



	// get upper property of(x,y) in the nLen signed distance fields
	double getUpperProperty(double x,double y,const double* pSDF,int nLen,bool bUseIndices=false);

public:
	// build tessers
	void BuildTess();
	// draw uncertain bands
	void DrawContourBoxplots();
	void DrawVariabilityPlots(int nCluster = 0);
	void DrawContourProbabilityPlots();
	void DrawSDFContours(int nMember, int nCluster, bool bCluster);
	void DrawSortedSDFContours(int nLevel);
	void DrawContours(int nMember, int nCluster,bool bCluster);
	void DrawSortedContours(int nLevel);
	void DrawResampledContours(int nLevel);
	void DrawResampledSDFContours(int nLevel);
	void DrawEnConVisBands(int nLevel);
	void DrawEnConVisBands_C(int nLevel, int nCluster);
	void DrawContoursResampledForClusters(int nLevel,int nCluster);
	void DrawIsoContourDensityPlots();
	void DrawOutliers();
private:	// output information
	void outputPC();
	void outputLabel();
};
