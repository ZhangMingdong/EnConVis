#pragma once
#include <QGLWidget>
#include "ContourBandGenerator.h"
#include <MathTypes.hpp>
#include "def.h"

#include "GridFrame.h"

#include "TimeStep.h"

class DataField;
class FeatureSet;

class TimeStep;



/*
	model of the meteorology data
	Mingdong
	2017/05/05
*/
class MeteModel :public GridFrame
{
	Q_OBJECT
public:
	MeteModel();
	virtual ~MeteModel();

protected:
	// 0.io related	
	QString _strFile;				// file name of the data	

	TimeStep* _pTimeStep;			// current time step
	TimeStep* _arrTimeSteps[62];	// time step list

	// 2.basic contours and areas
	QList<QList<ContourLine>> _listContourBrushed;		// list of brushed contours of ensemble members
	QList<QList<ContourLine>> _listContourNotBrushed;	// list of not brushed contours of ensemble members
	QList<QList<ContourLine>> _listMemberContour[g_nEnsemblesMax];			// list of contours of each ensemble member
	QList<QList<ContourLine>> _listEnsClusterContour[g_nClusterMax];			// list of contours of each ensemble cluster
	QList<QList<ContourLine>> _listContourMinEG;
	QList<QList<ContourLine>> _listContourMaxEG;

	QList<double> _listIsoValues;						// list of iso values

	// 3.cluster related
	int _nUncertaintyRegions = 6;				// number of uncertainty regions
	double _dbVarThreshold = 1.5;				// threshold of the variance	
	int _nSmooth = 1;							// smooth level 1~5	
	int _nMember = 0;							// current focused member
	int _nEnsCluster = 0;						// current ensemble cluster

	GLubyte* _dataTexture = NULL;				// data of the texture
	int _nTexW;
	int _nTexH;

	// grid points of thresholded variance
	int _nThresholdedGridPoints = 0;


	// 4.cluster the ensemble member directly
	QList<DataField*> _arrEnsClusterData;	// data of each cluster

public:
	// initialize the model
	virtual void InitModel(int nEnsembleLen, int nWidth, int nHeight
		, bool bBinary = false
		, int nWest = -180, int nEast = 180, int nSouth = -90, int nNorth = 90);
	// generate color mapping texture
	virtual GLubyte* GenerateTexture();


	virtual DataField* GetData() { return _pTimeStep->_pData; }

	virtual QList<ContourLine> GetContourMin(int isoIndex = 0);
	virtual QList<ContourLine> GetContourMax(int isoIndex = 0);
	virtual QList<ContourLine> GetContourMean(int isoIndex = 0);
	virtual void DrawOutliers(int isoIndex = 0);
	virtual QList<QList<ContourLine>> GetContourBrushed();
	virtual QList<QList<ContourLine>> GetContourNotBrushed();

	virtual void BuildTess(int nTimeIndex = 0, int isoIndex = 0);
	virtual void DrawVariabilityPlots(bool bCluster,int nTimeIndex=0, int isoIndex = 0);
	virtual void DrawContourBoxplots(int nTimeIndex = 0, int isoIndex = 0);
	virtual void DrawContourProbabilityPlots(int nTimeIndex = 0, int isoIndex = 0);
	virtual void DrawIsoContourDensityPlots(int nTimeIndex = 0, int isoIndex = 0);

	virtual int GetUncertaintyAreas() { return _nUncertaintyRegions; }
	virtual int GetLabel(int l);
	virtual void GetMerge(int l, int& nSource, int& nTarget);
	virtual double GetPC(int l, int nIndex);
	virtual int GetClusters();

	// get a vector of the sorted variance;
	virtual std::vector<double> GetVariance();
	virtual void SetTransparency(int nT, int isoIndex = 0);



	virtual void DrawContour(bool bCluster,int isoIndex = 0);
	virtual void DrawContourSDF(bool bCluster, int isoIndex = 0);

	virtual void DrawContourSorted(int isoIndex = 0);

	virtual void DrawContourSortedSDF(int isoIndex = 0);

	virtual void DrawContourResampled(int isoIndex = 0);
	virtual void DrawContourResampledSDF(int isoIndex = 0); 
	virtual void DrawContoursResampledForClusters(int isoIndex = 0);
	virtual void DrawEnConVisBands(int isoIndex = 0);
	virtual void DrawEnConVisBands_C(int isoIndex = 0);
protected:
	// specialized model initialization
	virtual void initializeModel();
	virtual void updateTimeStep();				// update according to current time step
protected:
	// read the dip value
	virtual void readDipValue(char* strFileName);
	virtual void readDipValueG(char* strFileName);

	// read data from text file for global area, set the left line according to the right line.
	virtual void readDataFromTextG();


	// read data from binary file
	void readData();
public:
	enum enumBackgroundFunction
	{
		bg_mean,				// mean
		bg_vari,				// variance
		bg_cluster,				// spatial cluster
		bg_varThreshold,		// thresholded variance
		bg_vari_smooth,			// smooth variance
		bg_dipValue,			// variance of variance
		bg_dipValueThreshold,	// thresholded dip value
		bg_EOF,					// EOF
		bg_Obs,					// obs
		bg_err,					// error
		bg_SDF,					// SDF
		bg_LineKernel,			// SDF
		bg_LineKernelX,			// SDF
		bg_LineKernelY,			// SDF
		bg_LineKernelZ,			// SDF
		bg_ICDVX,				// SDF
		bg_ICDVY,				// SDF
		bg_ICDVW,				// SDF
		bg_IsoContourDensity	// density of the iso-contour
	};
protected:
	// using which background function
	enumBackgroundFunction _bgFunction = bg_mean;

private:
	// generate texture of clustered variance
	void buildTextureClusteredVariance();

	// generate regions for clustering in each region
	void generateRegions();

	// generate texture use threshold var
	void buildTextureThresholdVariance();


	// generate texture use thresholded dip value
	void buildTextureThresholdDipValue();

	// generate texture of colormap of mean or variance
	void buildTextureColorMap();


	// generate texture of signed distance function
	void buildTextureSDF();
	void buildTextureICDVX();
	void buildTextureICDVY();
	void buildTextureICDVW();

	void buildTextureICD_LineKernel();
	void buildTextureICDX();
	void buildTextureICDY();
	void buildTextureICDZ();

	// generate texture of iso-contour density
	void buildTextureICD();

	// generate new texture
	void regenerateTexture();


	// read observation data
	void readObsData();
public:
	// interface of the creation of model
	static MeteModel* CreateModel(bool bA = false);
public:
	// wrappers

	// set background function
	void SetBgFunction(enumBackgroundFunction f);
	// get the background function
	enumBackgroundFunction GetBgFunction() { return _bgFunction; }
	// brushing
	virtual void Brush(int nLeft, int nRight, int nTop, int nBottom);

	void SetVarThreshold(double dbThreshold);
	double GetVarThreshold() { return _dbVarThreshold; }



	void SetSmooth(int nSmooth);
	void SetMember(int nMember);
	void SetEnsCluster(int nEnsCluster);
	void SetEnsClusterLen(int nEnsClusterLen);
	int GetContourLevel() { return _arrContourLevel[0]; }

	int GetGridLength() { return _nGrids; }
	int GetThresholdedGridLength() { return _nThresholdedGridPoints; }



	void SetIsoValues(QList<double> listIsoValues);
	QList<double> GetIsoValues() { return _listIsoValues; }


	int GetTexW() { return _nTexW; }
	int GetTexH() { return _nTexH; }
	int GetEnsCluster() { return _nEnsCluster; }

private:
	// detail level of the contours, 0-1;1-3;2-7...
	int _arrContourLevel[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

public slots:


	void updateTimeStep(int nTS);

	void updateContourLevel0(int nLevel);
	void updateContourLevel1(int nLevel);
	void updateContourLevel2(int nLevel);
	void updateContourLevel3(int nLevel);
	void updateContourLevel4(int nLevel);
	void updateContourLevel5(int nLevel);
	void updateContourLevel6(int nLevel);
	void updateContourLevel7(int nLevel);
	void updateContourLevel8(int nLevel);
	void updateContourLevel9(int nLevel);

	void onOptimizing();
protected:
	int _nTime = 0;		// time step
signals:
	void UpdateView();
public:
	virtual int GetCurrentTimeIndex();

	void SetFocus(DPoint2 pt);
};

