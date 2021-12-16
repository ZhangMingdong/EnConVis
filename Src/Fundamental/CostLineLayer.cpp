#include "CostLineLayer.h"


#include <gl/GLU.h>
#include <iostream>
#include "../Shapefile/Shapefile.h"
#include "LayerLayout.h"

using namespace std;

CostLineLayer::CostLineLayer()
{
}


CostLineLayer::~CostLineLayer()
{
}

void CostLineLayer::draw(DisplayStates states){

	drawCostline();
}
void CostLineLayer::init(){

	_gllist = glGenLists(1);	// generate the display lists
	readCostline();
}



void CostLineLayer::drawCostline(){
	glColor3f(0, 0, 0);
	glCallList(_gllist);

	// draw the frame
	glBegin(GL_LINE_LOOP);
	glVertex2f(_pLayout->_dbLeft, _pLayout->_dbTop);
	glVertex2f(_pLayout->_dbRight, _pLayout->_dbTop);
	glVertex2f(_pLayout->_dbRight, _pLayout->_dbBottom);
	glVertex2f(_pLayout->_dbLeft, _pLayout->_dbBottom);

	glEnd();
}

void CostLineLayer::readCostline(){

	//const char* path = "D:\\Mingdong\\Meteorology\\coastline\\ne_10m_coastline\\ne_10m_coastline.shp";
	const char* path = "..\\..\\coastline\\ne_10m_coastline\\ne_10m_coastline.shp";

	FILE* pShapefile = open_shapefile(path);

	if (pShapefile == 0) {
		return;
	}
	double fWidth = _pLayout->_dbRight - _pLayout->_dbLeft;
	double fHeight = _pLayout->_dbTop - _pLayout->_dbBottom;

	SFShapes* pShapes = read_shapes(pShapefile);

	glNewList(_gllist, GL_COMPILE);
	for (uint32_t x = 0; x < pShapes->num_records; ++x) {
		const SFShapeRecord* record = pShapes->records[x];
		SFPolyLine* polyline = get_polyline_shape(pShapefile, record);
//		printf("Polyline %d: num_parts = %d, num_points = %d\n", x, polyline->num_parts, polyline->num_points);
		/* Do things with the polygon. */
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < polyline->num_points; i++)
		{
			double x = polyline->points[i].x*_fScaleW;
			double y = polyline->points[i].y*_fScaleH;
			glVertex2f(x, y);
		}
		glEnd();

		free_polyline_shape(polyline);
		polyline = 0;

		fflush(stdout);
	}
	glEndList();

	free_shapes(pShapes);
	close_shapefile(pShapefile);

	cout << "read cost line" << endl;

}