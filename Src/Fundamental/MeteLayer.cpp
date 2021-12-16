#include "MeteLayer.h"
#include "LayerLayout.h"

#include "ColorMap.h"

#include <iostream>
#include <math.h>

ILayerCB* MeteLayer::_pCB = NULL;

MeteLayer::MeteLayer() :_bShow(true), _pModel(NULL)
{
}

MeteLayer::~MeteLayer()
{
}

void MeteLayer::DrawLayer(DisplayStates states){
	if (_bShow) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		this->draw(states);
		glPopMatrix();
		glPopAttrib();
	}
}

void MeteLayer::InitLayer(const LayerLayout* pLayout, double fScaleW, double fScaleH){
	_pLayout = pLayout;

	_fScaleW = fScaleW;
	_fScaleH = fScaleH;

	this->init();
}

// set color according to cluster index
void MeteLayer::SetClusterColor(int nIndex) {
	if (nIndex<10)
	{
		glColor3f(ColorMap::GetCategory10D(nIndex, 0, 1)
			, ColorMap::GetCategory10D(nIndex, 1, 1)
			, ColorMap::GetCategory10D(nIndex, 2, 1));
	}
}

void MeteLayer::SetRegionColor(int nIndex) {
	if (nIndex<10)
	{
		glColor3f(ColorMap::GetCategory10D(nIndex, 0)
			, ColorMap::GetCategory10D(nIndex, 1)
			, ColorMap::GetCategory10D(nIndex, 2));
	}
}

void MeteLayer::drawCircle(double x, double y, double dbRadius, bool bFill) {
	int nSegs = 20;
	if (bFill) glBegin(GL_POLYGON);
	else glBegin(GL_LINE_LOOP);

	for (size_t i = 0; i < nSegs; i++)
	{
		double dbAngle = 2.0*M_PI*i / nSegs;
		double dbX = x + dbRadius*cos(dbAngle);
		double dbY = y + dbRadius*sin(dbAngle);
//		std::cout << dbX << ": "<<dbY << std::endl;
		glVertex3f(dbX, dbY, 0);
	}

	glEnd();
}