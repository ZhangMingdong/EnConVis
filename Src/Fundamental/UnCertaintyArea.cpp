#include "UnCertaintyArea.h"
#include <QDebug>
#include "def.h"

bool ClosedContour::rayCasting_on(const ClosedContour& contour){
	// using ray casting
	// 1.check whether this point on x grid or y grid
	QPointF ptRef = contour._listPt[0];
	double fX = qMin(ptRef.x() - (int)ptRef.x(), (int)ptRef.x() + 1 - ptRef.x());
	double fY = qMin(ptRef.y() - (int)ptRef.y(), (int)ptRef.y() + 1 - ptRef.y());
	bool bX = fX < fY;		// on a y grid line
	int nCount = 0;
	int nLen = _listPt.size();

	// 2.check intersection with point on contour line
	for (int i = 0; i < nLen; i++)
	{
		if (bX)
		{
			if (abs(_listPt[i].x() - ptRef.x())<g_fDifference && _listPt[i].y()>ptRef.y()) nCount++;
		}
		else{
			if (abs(_listPt[i].y() - ptRef.y())<g_fDifference && _listPt[i].x()>ptRef.x()) nCount++;
		}
	}

	// 3.check intersection with border
	int i = 0;
	QPointF ptLast = _listPt[nLen - 1];
	for each (QPointF pt in _listPt)
	{
		if (bX)
		{
			if (abs(ptLast.y() - pt.y())<g_fDifference			// border 
				&& pt.y() > ptRef.y()						// the right direction
				&& ((ptLast.x()<ptRef.x() && pt.x()>ptRef.x()) || (ptLast.x() > ptRef.x() && pt.x() < ptRef.x()))) // intersect with this border
				nCount++;				
		}
		else{
			if (abs(ptLast.x() - pt.x())<g_fDifference			// border 
				&& pt.x() > ptRef.x()						// the right direction
				&& ((ptLast.y()<ptRef.y() && pt.y()>ptRef.y()) || (ptLast.y() > ptRef.y() && pt.y() < ptRef.y()))) // intersect with this border
				nCount++;
			
		}
		ptLast = pt;
	}
	return nCount % 2 == 1;
}
bool ClosedContour::rayCasting_intersection(const ClosedContour& contour){

	// 2.using ray casting
	QPointF ptRef = contour._listPt[0];
	double fX = ptRef.x() - (int)ptRef.x();
	double fY = ptRef.y() - (int)ptRef.y();
	bool bX = fX > fY;		// check x-axis
	int nCount = 0;
	int nLen = _listPt.size();
	int nLastState;
	int nState;													// state of the last point and the reference point
	if (bX)
	{
		if (_listPt[nLen - 1].x() > ptRef.x()) nLastState = 1;
		else if (_listPt[nLen - 1].x() < ptRef.x()) nLastState = -1;
		else nLastState = 0;
	}
	else{
		if (_listPt[nLen - 1].y() > ptRef.y()) nLastState = 1;
		else if (_listPt[nLen - 1].y() < ptRef.y()) nLastState = -1;
		else nLastState = 0;

	}
	for (int i = 0; i < nLen; i++)
	{
		if (bX)
		{
			if (ptRef.y()>_listPt[i].y()) continue;					// one direction only
			if (_listPt[i].x() > ptRef.x()) nState = 1;
			else if (_listPt[i].x() < ptRef.x()) nState = -1;
			else nState = 0;
		}
		else{
			if (ptRef.x()>_listPt[i].x()) continue;					// one direction only
			if (_listPt[i].y() > ptRef.y()) nState = 1;
			else if (_listPt[i].y() < ptRef.y()) nState = -1;
			else nState = 0;
		}

		if (nLastState == 0) nCount++;					// find a point on
		else if (nLastState*nState == -1) nCount++;		// find a segment intersection
		nLastState = nState;
	}
	return nCount % 2 == 1;
}
bool ClosedContour::Contain(const ClosedContour& contour){
	// 1.check the bounding box, if not contain, return false
	if (!(_fX0 < contour._fX0
		&& _fX1 > contour._fX1
		&& _fY0 < contour._fY0
		&& _fY1 > contour._fY1))
	{
		return false;
	}
	return rayCasting_on(contour);
}

UnCertaintyArea::UnCertaintyArea() :_pParent(NULL)
{
}


UnCertaintyArea::~UnCertaintyArea()
{
}

/*
	two remaining problem
		do not set the state
		when insert child into area, if child also have child...
*/
int UnCertaintyArea::InsertContour(UnCertaintyArea* pArea){
	// 1.check if area can be contained by the top contour
	if (!_contour.Contain(pArea->_contour)) return 0;


	// 2.check if area can be inserted into a child
	int nEmbeddedLen = _listEmbeddedArea.length();
	for (int i = 0; i < nEmbeddedLen;i++)
	{
		if (_listEmbeddedArea[i]->InsertContour(pArea)) return 1;
	}

	// 3.check if this contour can contain any of the children
	for (int i = _listEmbeddedArea.size() - 1; i >= 0; i--)
	{
		if (pArea->_contour.Contain(_listEmbeddedArea[i]->_contour))
		{
			pArea->InsertContour(_listEmbeddedArea[i]);
			_listEmbeddedArea.removeAt(i);
		}
	}

	// 4.put this contour into the embedded list
	_listEmbeddedArea.push_back(pArea);
	pArea->_pParent = this;
	return 2;
}


int UnCertaintyArea::ContourSize(){
	int nSize = 1;
	for each (UnCertaintyArea* pArea in _listEmbeddedArea)
	{
		nSize += pArea->ContourSize();
	}
	return nSize;
}


void UnCertaintyArea::Release(){
	delete this;
}


void UnCertaintyArea::Scale(double dbScaleX, double dbScaleY) {
	_contour._fX0 *= dbScaleX;
	_contour._fX1 *= dbScaleX;
	_contour._fY0 *= dbScaleY;
	_contour._fY1 *= dbScaleY;
	int nLen = _contour._listPt.size();
	for (size_t i = 0; i < nLen; i++)
	{
		_contour._listPt[i] = QPointF(_contour._listPt[i].x()*dbScaleX, _contour._listPt[i].y()*dbScaleY);
	}
	return;	// do not need to scale the embedded regions, because thay were put into the top level before tess
	for each (UnCertaintyArea* pEmbedded in _listEmbeddedArea)
	{
		pEmbedded->Scale(dbScaleX, dbScaleY);
	}
}