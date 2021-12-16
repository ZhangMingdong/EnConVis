#pragma once
#include "MeteModel.h"

/*
	artificial models
	before 2020/12/01
*/
class ArtificialModel :
	public MeteModel
{
public:
	ArtificialModel();
	~ArtificialModel();
protected:	
	virtual void initializeModel();				// specialized model initialization
	virtual void updateTimeStep() {};				// update according to current time step
private:
	/*
		regenerate contours for generate data
	*/
	QList<QList<ContourLine>> regenerateData();		
	/*
		regenerate data from given contours
	*/
	void generateDataFromContour();					
	/*
		regenerate data from given field
		pattern1:
			5 high value and 5 low value plus 1 outlier control
	*/
	void generateDataFromField();				// regenerate data from given contours
	/*
		regenerate data from given field
		pattern2:
			one core
			circles
	*/
	void generateDataFromField_2();

	/*
		modified from generateDataFromField, just two high core and two low core
	*/
	void generateDataFromField_3();

	/*
		from 3
		reverse the modes
	*/
	void generateDataFromField_4();
	/*
		regenerate data from given field
		pattern2:
			3 core
			circles
	*/
	void generateDataFromField_5();

	/*
		modified from _5
		vary the three modes
	*/
	void generateDataFromField_6();

	void generateExampleForSmooth();
public:
	virtual void DrawVariabilityPlots(bool bCluster, int nTimeIndex = 0, int isoIndex = 0);
	virtual void DrawContourBoxplots(int nTimeIndex, int isoIndex);
	virtual void DrawContourProbabilityPlots(int nTimeIndex, int isoIndex);
	virtual void BuildTess(int nTimeIndex, int isoIndex);

public:
	int GetCurrentTimeIndex() {
		return 0;
	}
};

