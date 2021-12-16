#pragma once
#include <QList>
#include <QPointF>
#include "BasicStruct.h"


const double g_fDifference = 0.0001;

/*
	struct to represent a closed contour
	by Mingdong
	2017
*/
struct ClosedContour
{	
	QList<QPointF> _listPt;		// point list
	double _fX0;
	double _fX1;
	double _fY0;
	double _fY1;

	// generate the bounding box
	void GenerateBounding();	

	// check if contain contour
	bool Contain(const ClosedContour& contour);			

	// ray casting algorithm based on on the gridline
	bool rayCasting_on(const ClosedContour& contour);			

	// ray casting algorithm based on intersection
	bool rayCasting_intersection(const ClosedContour& contour);	
};

/*
	 struct to represent the uncertainty area
	 by Mingdong
	 2017
*/
class UnCertaintyArea
{
public:
	UnCertaintyArea();
	~UnCertaintyArea();

	/*
		state of this area: 
			-1:	lower;
			0:	uncertain;
			1:	higher;
			-2:	not decided, with max border
			2:	not decided, with min border
	*/
	int _nState;

	ClosedContour _contour;						// closed contour of this area	
	QList<UnCertaintyArea*> _listEmbeddedArea;	// list of embedded area	
	QList<UnCertaintyArea*> _listChildren;		// the list of the children area, as in the tree of the contour	
	UnCertaintyArea* _pParent = NULL;			// the parent area
public:
	/*
		Try to insert area in this instance.
		return value:
			0: cannot insert
			1: inserted not top level
			2: inserted into the top level
	*/
	int InsertContour(UnCertaintyArea* area);
	// return the size of all the contours
	int ContourSize();
	// release resource
	void Release();
public:
	// scale the result
	void Scale(double dbScaleX, double dbScaleY);
};

