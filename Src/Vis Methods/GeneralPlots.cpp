#include "GeneralPlots.h"

#include "ContourBandGenerator.h"
#include "ContourGenerator.h"
#include "DataField.h"
#include "switch.h"
#include <ColorMap.h>
#include <QDebug>
#include "Timer.h"

const double GeneralPlots::_scdbBandOpacity=0.3;
const double GeneralPlots::_scdbLineOpacity=0.8;

/*
	y=kx+b
	x=32,y=0.9
	x=1,y=0.3;
*/
//const double c_dbK = 0.6 / 31;
//const double c_dbB = 0.3 - c_dbK;


/*
	y=kx+b
	x=4,y=0.9
	x=1,y=0.3;
*/
const double c_dbK = 0.2;
const double c_dbB = 0.1;

double GeneralPlots::getBandOpacity(int nBands) {
	//return c_dbK + c_dbB/ nBands;
	switch (nBands)
	{
	case 1:
		return 0.3;
	case 2:
		return 0.25;
	case 3:
		return 0.7 / 3;
	default:
		return 0.9 / nBands;
		break;
	}
}

void GeneralPlots::DrawBands() {
	if (!_bInitialized) return;
	double dbOpacity = getBandOpacity(_listTesser.size());

	int nIndex = 0;
	glColor4f(ColorMap::GetCategory10D(nIndex, 0)
		, ColorMap::GetCategory10D(nIndex, 1)
		, ColorMap::GetCategory10D(nIndex, 2)
		, dbOpacity);
	// draw bands
	for each (BandTesser te in _listTesser)
	{
		te.Draw();
	}

	// draw mean
	glColor4f(1, 0, 1,_scdbLineOpacity);
	for each(ContourLine contour in _listContourMean)
	{
		drawContour(contour);
	}

	// virtual function
	draw();
}

void GeneralPlots::generateContourImp(const QList<ContourLine>& contourMin
	, const QList<ContourLine>& contourMax
	, QList<UnCertaintyArea*>& areas
	, int nWidth
	, int nHeight
) {
	ContourBandGenerator generator;
	if(nWidth&&nHeight)
		generator.Generate(areas, contourMin, contourMax, nWidth, nHeight);
	else
		generator.Generate(areas, contourMin, contourMax, _nWidth, _nHeight);
}

void GeneralPlots::drawContours(const QList<ContourLine>& contours) {
	for each (const ContourLine& contour in contours)
	{
		drawContour(contour);
	}
}

void GeneralPlots::drawContour(const ContourLine& contour) {

	glBegin(GL_LINE_STRIP);
	for each (QPointF pt in contour._listPt)
	{
		double x = pt.x();// *_fScaleW;
		double y = pt.y();// *_fScaleH;
		glVertex2f(x, y);
	}
	glEnd();
}

void GeneralPlots::Initialize(int w, int h, int l, double isovalue, DataField*  pData) {
	_bInitialized = true;

	_pData = pData;
	_dbIsoValue = isovalue;
	_nWidth = w;
	_nHeight = h;
	_nEnsembleLen = l;
	_nGrids = w * h;

	// 1.initialize the instance
	init();
	//MyTimer::Time("1");

	// 2. do statistics
	doStatistics();
	//MyTimer::Time("2");

	
	// 3.generate contours
	generateContours();
	ContourGenerator::GetInstance()->Generate(_pData->GetMean(), _listContourMean, _dbIsoValue, _nWidth, _nHeight);
	doStatistics();
	//MyTimer::Time("3");

	// 4.generate regions
	generateBands();
	//MyTimer::Time("4");
		
}

void GeneralPlots::setOpacity(double dbOpacity) {
	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	currentColor[3] = dbOpacity;
	glColor4fv(currentColor);
}

double GetSegLength(QPointF pt1, QPointF pt2) {
	double dbX = pt1.x() - pt2.x();
	double dbY = pt1.y() - pt2.y();
	return sqrt(dbX*dbX + dbY * dbY);
}

/*
	smooth contours
	using contours
*/
ContourLine GeneralPlots::smoothContour(const ContourLine& contour) {
	double dbScale = .2;
	ContourLine contour_s;
	contour_s._nClosed = contour._nClosed;

	int nPoints = contour._listPt.length();
	if (contour._nClosed) {
		nPoints--;	// -1 is to remove the duplicate end point

		contour_s._listPt.append(contour._listPt[1]);	// add the duplicate point
		for (int i = 0; i < nPoints; i++)
		{
			int n1 = i;
			int n2 = (i + 1) % nPoints;
			int n3 = (i + 2) % nPoints;
			int n4 = (i + 3) % nPoints;
			QPointF pt1 = contour._listPt[n1];
			QPointF pt2 = contour._listPt[n2];
			QPointF pt3 = contour._listPt[n3];
			QPointF pt4 = contour._listPt[n4];			

			QPointF pt14 = (pt1 + pt4) / 2;
			QPointF pt23 = (pt2 + pt3) / 2;
			double dbScale2 = GetSegLength(pt2, pt3) / GetSegLength(pt1, pt4);
			QPointF pt = pt23 + (pt23 - pt14)*dbScale*dbScale2;
			contour_s._listPt.append(pt);
			contour_s._listPt.append(pt3);
		}
	}
	else {
		contour_s._listPt.append(contour._listPt[0]);
		contour_s._listPt.append(contour._listPt[1]);
		for (size_t i = 1; i < nPoints - 2; i++)
		{
			QPointF pt1 = contour._listPt[i - 1];
			QPointF pt2 = contour._listPt[i];
			QPointF pt3 = contour._listPt[i + 1];
			QPointF pt4 = contour._listPt[i + 2];

			QPointF pt14 = (pt1 + pt4) / 2;
			QPointF pt23 = (pt2 + pt3) / 2;
			double dbScale2 = GetSegLength(pt2, pt3) / GetSegLength(pt1, pt4);
			QPointF pt = pt23 + (pt23 - pt14)*dbScale*dbScale2;
			contour_s._listPt.append(pt);
			contour_s._listPt.append(pt3);
		}
		contour_s._listPt.append(contour._listPt[nPoints - 1]);
	}



	return contour_s;

}

QList<ContourLine> GeneralPlots::smoothContours(QList<ContourLine>& contours) {

	QList<ContourLine> contours_s;
	for each (ContourLine contour in contours)
	{
		int nPoints = contour._listPt.length();

		ContourLine contour_s = smoothContour(contour);
		contours_s.append(contour_s);
	}
	return contours_s;

}
