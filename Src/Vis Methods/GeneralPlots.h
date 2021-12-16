#pragma once

#include "BandTesser.h"

#include <QList>

class DataField;

/*
	class for the general plots, which derived contour boxplots and variability plots
	by Mingdong
	2020/06/16
*/
class GeneralPlots {
protected:
	static const double _scdbBandOpacity;
	static const double _scdbLineOpacity;
protected:
	int _nWidth = 0;
	int _nHeight = 0;
	double _dbIsoValue = 0;
	int _nGrids;
	int _nEnsembleLen;
	DataField* _pData;
	
	QList<BandTesser> _listTesser;						// tessers for the uncertain bands
	QList<ContourLine> _listContourMean;				// list of contours of mean
	bool _bInitialized = false;							// if initialized

protected:	// virtual operation
	virtual void doStatistics() = 0;
	virtual void generateBands() = 0;
	virtual void generateContours() = 0;
	virtual void draw() {};
	virtual void init() = 0;

protected:
	void generateContourImp(const QList<ContourLine>& contourMin
		, const QList<ContourLine>& contourMax
		, QList<UnCertaintyArea*>& areas
		, int nWidth=0
		, int nHeight=0
	);

	void drawContour(const ContourLine& contour);
	void drawContours(const QList<ContourLine>& contours);
	// set opacity keep color
	void setOpacity(double dbOpacity);
	// smooth contours
	QList<ContourLine> smoothContours(QList<ContourLine>& contours);
	ContourLine smoothContour(const ContourLine& contour);
	// get band opacity of nBands
	double getBandOpacity(int nBands);
public:
	// draw uncertain bands
	void DrawBands();
	void Initialize(int w, int h, int l, double isovalue, DataField*  pData);
};