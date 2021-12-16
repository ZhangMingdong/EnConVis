#pragma once
#include <QList>
#include <QPointF>
//#include <QGLWidget>
/*
	Basic struct used in this project
	by mingdong
	2017
*/



/*
	struct of a contour line
*/
struct ContourLine
{
	int _nState;			// -1:min;1:max, used in area calculating
	int _nClosed;			// 0:unclosed;1:closed
	QList<QPointF> _listPt;	// point list

	// used for refined cpp
	void Scale(double dbScaleX, double dbScaleY) {
		int nLen = _listPt.size();
		for (size_t i = 0; i < nLen; i++)
		{
			_listPt[i] = QPointF(_listPt[i].x() * dbScaleX, _listPt[i].y() * dbScaleY);
		}
	}

};

/*
	The line segment used in marching squares
	i,j is the 1st and 2nd coordinate of the point
	11,12 is the grid points of the edge of the first point
	21,22 is the grid points of the edge of the second point
*/
struct LineSeg
{
	
	int _arrIndices[8];		// i11,j11,i12,j12,i21,j21,i22,j22
	LineSeg(int i11, int j11, int i12, int j12, int i21, int j21, int i22, int j22);

	/*
		this line segment is AB, parameter is CD
		return 1 if ABD and B=C
		return 2 if ABC and B=D
		return 3 if DAB and A=C
		return 4 if CAB and A=D
		return 0 else;
	*/
	int StartOrEndWith(int arrIndices[8]);
};

/*
	a node in the construction of the contour
	each stand for a contour component
*/
struct ContourNode 
{	
	QList<ContourNode> _children;		// list of its children	
	int _nContourIndex;					// index in the contour list	
	int _nReverse;						// 0: not reverse;1:reverse	
	int _nMatched;						// 0: unmatched;1:matched
};

/*
	structure for the end point used in contour strip generating
*/
struct EndPointRaw
{
	int _nContourIndex;			 // index of contour
	int _nEndPointIndex;		 // index of endpoint, 0:start;1:end
	QPointF _ptEndPoint;		 // point	
	EndPointRaw(int nContourIndex, int nEndPointIndex, QPointF pt) :_nContourIndex(nContourIndex), _nEndPointIndex(nEndPointIndex), _ptEndPoint(pt) {}
};