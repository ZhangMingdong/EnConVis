#include "SpaghettiPlot.h"
#include "ContourGenerator.h"
#include "DataField.h"
#include "ColorMap.h"
#include "Switch.h"
//#include <MathTool.h>
#include <QDebug>
//
//
//#include<MathTypes.hpp>

SpaghettiPlot::SpaghettiPlot()
{
}

SpaghettiPlot::~SpaghettiPlot()
{

}
void SpaghettiPlot::generateContours()
{
	// 1.generate spaghetti and sorted
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		// spaghetti
		{
			QList<ContourLine> contour;
			ContourGenerator::GetInstance()->Generate(_pData->GetData(i), contour, _dbIsoValue, _nWidth, _nHeight);
			_listContour.push_back(contour);
		}
	}
}

void SpaghettiPlot::doStatistics() {

}
void SpaghettiPlot::generateBands() {

}
void SpaghettiPlot::draw() {

}
void SpaghettiPlot::init() {

}
void SpaghettiPlot::SetLabels(int nClusters, const int* cpLabels) {
	_nClusters = nClusters;
	_cpLabels = cpLabels;

}

void SpaghettiPlot::DrawContours(int nMember, int nCluster, bool bCluster) {
	QList<QList<ContourLine>> contours_to_draw = _listContour;

	double dbOpacity = _scdbLineOpacity * (10.0 - _nTransparency) / 10.0;
	setOpacity(dbOpacity);

	if (_cpLabels && bCluster) {
		// draw one cluster
		if (nMember&&nCluster) {
			// draw a member of this cluster
			for (size_t i = 0; i < _nEnsembleLen; i++)
			{
				if (_cpLabels[i] == nCluster - 1) nMember--;
				if (nMember == 0)
				{
					glColor4f(ColorMap::GetCategory10D(_cpLabels[i], 0)
						, ColorMap::GetCategory10D(_cpLabels[i], 1)
						, ColorMap::GetCategory10D(_cpLabels[i], 2)
						, dbOpacity
					);
					for each (ContourLine contour in contours_to_draw[i])
					{
						drawContour(contour);
					}
					break;
				}
			}
		}
		else {
			for (size_t i = 0; i < _nEnsembleLen; i++)
			{
				if (nCluster == 0 || (_cpLabels[i] == nCluster - 1))
				{
					glColor4f(ColorMap::GetCategory10D(_cpLabels[i], 0)
						, ColorMap::GetCategory10D(_cpLabels[i], 1)
						, ColorMap::GetCategory10D(_cpLabels[i], 2)
						, dbOpacity
					);
					// temporally set black
					for each (ContourLine contour in contours_to_draw[i])
					{
						drawContour(contour);
					}
				}
			}
		}
	}
	else if (nMember) {
		// draw one member
		for each (ContourLine contour in contours_to_draw[nMember - 1])
		{
			drawContour(contour);
		}
	}
	else {
		// draw all members
		for each (QList<ContourLine> contours in contours_to_draw)
		{
			for each (ContourLine contour in contours)
			{
				drawContour(contour);
			}
		}
	}

}
