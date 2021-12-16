#pragma once

#include "GeneralPlots.h"

#include <QList>

class DataField;

/*
	this class used to perform the contour boxplots 
	by Mingdong
	2019/06/11
*/
class ContourBoxplots:public GeneralPlots {

	// tesser
private:

	double* _gridHalfMax;							// maximum of half valid ensemble results
	double* _gridHalfMin;							// minimum of half valid ensemble results
	double* _gridValidMax;							// maximum of valid ensemble results
	double* _gridValidMin;							// minimum of valid ensemble results

	QList<ContourLine> _listContourMedian;			// list of contours of median
	QList<QList<ContourLine>> _listContourOutlier;	// list of contours of outlier

	QList<ContourLine> _listContourMinValid;		// list of contours of minimum of valid members
	QList<ContourLine> _listContourMaxValid;		// list of contours of maximum of valid members
	QList<ContourLine> _listContourMinHalf;			// list of contours of minimum of half valid members
	QList<ContourLine> _listContourMaxHalf;			// list of contours of maximum of half valid members
	
	QList<UnCertaintyArea*> _listAreaValid;			// list of the uncertainty area of union of valid members
	QList<UnCertaintyArea*> _listAreaHalf;			// list of the uncertainty area of union of half valid members

	int _nOutlierThreshold = 1;						// threshold for outliars

	bool * _pUpperSet;								// m*n; if the grid point in the upper set
	int *_pSetBandDepth;							// sBandDepth of each member
	int *_pMemberType;								// type of each member.0:outlier,1-100%,2-50%.

	int _nMedianIndex = -1;							// index of median of the contour
private:

	// calculate whether a grid point is in upperset
	void calculateLevelSet();
	// new version, use the method in contour boxplot
	void calculateBandDepth();
	// calculate the type of each member: 0-outlier, 1-normal, 2-in 50%
	void calculateMemberType();
	// calculate the statistics, median, min, and max
	void calculateStatistics();


protected:
	// use pointwise statistics
	virtual void doStatistics();
	virtual void generateBands();
	virtual void generateContours();
	virtual void draw();
	virtual void init();
public:
	~ContourBoxplots();

	// build tessers
	void BuildTess();

	// draw the outliers
	void DrawOutliers();
};