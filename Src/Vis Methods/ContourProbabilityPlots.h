#pragma once

#include "GeneralPlots.h"

#include <QList>

class DataField;

/*
	this class used to perform the Contour Probability plot
	by Mingdong
	2019/06/18
*/
class ContourProbabilityPlots :public GeneralPlots {

	// tesser
private:

	double* _gridUpperPercentage;				// percentage of upper members of each grid point


	// according to the original papers
	static const int c_nContours = 7;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		,0.1
		,0.25
		,0.5
		,0.75
		,0.9
		,0.999 };



	/*
	static const int c_nContours = 11;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		,0.1
		,0.2
		,0.3
		,0.4
		,0.5
		,0.6
		,0.7
		,0.8
		,0.9
		,0.999 };

		*/
/*
	static const int c_nContours = 21;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		,0.05
		,0.1
		,0.15
		,0.2
		,0.25
		,0.3
		,0.35
		,0.4
		,0.45
		,0.5
		,0.55
		,0.6
		,0.65
		,0.7
		,0.75
		,0.8
		,0.85
		,0.9
		,0.95
		,0.999 };										// isovalues
*/

	/*
	static const int c_nContours = 33;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		,1/32.0
		,2/32.0
		,3/32.0
		,4/32.0
		,5/32.0
		,6/32.0
		,7/32.0
		,8/32.0
		,9/32.0
		,10/32.0
		,11 / 32.0
		,12 / 32.0
		,13 / 32.0
		,14 / 32.0
		,15 / 32.0
		,16 / 32.0
		,17 / 32.0
		,18 / 32.0
		,19 / 32.0
		,20 / 32.0
		,21 / 32.0
		,22 / 32.0
		,23 / 32.0
		,24 / 32.0
		,25 / 32.0
		,26 / 32.0
		,27 / 32.0
		,28 / 32.0
		,29 / 32.0
		,30 / 32.0
		,31 / 32.0
		,0.999 };
*/

	/*
	static const int c_nContours = 17;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		 ,1 / 16.0
		 ,2 / 16.0
		 ,3 / 16.0
		 ,4 / 16.0
		 ,5 / 16.0
		 ,6 / 16.0
		 ,7 / 16.0
		 ,8 / 16.0
		 ,9 / 16.0
		,10 / 16.0
		,11 / 16.0
		,12 / 16.0
		,13 / 16.0
		,14 / 16.0
		,15 / 16.0
		,0.999 };
		*/
/*

	static const int c_nContours = 9;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		 ,1 / 8.0
		 ,2 / 8.0
		 ,3 / 8.0
		 ,4 / 8.0
		 ,5 / 8.0
		 ,6 / 8.0
		 ,7 / 8.0
		,0.999 };
*/

/*
	static const int c_nContours = 5;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.001
		 ,1 / 4.0
		 ,2 / 4.0
		 ,3 / 4.0
		,0.999 };
*/

/*
	static const int c_nContours = 3;				// number of contours
	double _arrIsovalues[c_nContours] = { 0.25
		 ,.5
		,.75 };
*/

	QList<ContourLine> _arrlistContours[c_nContours];				// array of contours
	QList<UnCertaintyArea*> _arrlistRegions[c_nContours/2];		// array of regions



	int  _nSubdividedW = 0;						// width of subdivided grids
	int _nSubdividedH = 0;						// height of subdivided grids

	bool _bSubdividedGrids = true;				// subdivide or not
	int _nSubdivideNumber = 10;					// number of subdivide for each grids

protected:
	// use pointwise statistics
	virtual void doStatistics();
	virtual void generateBands();
	virtual void generateContours();
	virtual void draw();
	virtual void init();

public:
	~ContourProbabilityPlots();
	// build tessers
	void BuildTess();
	//


};