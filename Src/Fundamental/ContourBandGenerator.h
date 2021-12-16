#pragma once

#include <QQueue>
#include "BasicStruct.h"
#include "UnCertaintyArea.h"

/*
	used to generate contour band
	Mingdong
	2017/03/28
*/
class ContourBandGenerator
{
public:
	ContourBandGenerator(bool bConvex =true);
	~ContourBandGenerator();
public:
	// generate the uncertainty areas
	void Generate(QList<UnCertaintyArea*>& areas
		,const QList<ContourLine>& contourMin
		, const QList<ContourLine>& contourMax
		, int nWidth
		, int nHeight
		);
private:
	/*
		add this contour to the contour list,
		closed or unclosed checked by the property of the contour
	*/
	void addContourToList(ContourLine contour);
	// generate uncertainty areas from the unclosed contours
	void generateFromUnClosedContours(QList<UnCertaintyArea*>& areas);
	// insert the closed contours as embedded areas
	void insertClosedContours(QList<UnCertaintyArea*>& areas);
	// generate embedded areas from the built structure
	void generateEmbeddedArea(QList<UnCertaintyArea*>& areas);
	// generate the list of endpoints
	void generateEndPointsList();
	// handle an area in queue, if queue is empty, return false;
	bool handleAnAreaInQueue(QQueue<UnCertaintyArea*>& queueAreas, QList<UnCertaintyArea*>& areas);

	/*
		generate contour areas 
		params:
			root: the current root of the tree
			areas: store the generated area list
			nParentState: the state of the parent:0:lower;1:uncertainty;2:higher;-1:not clear
			ptParent: one of the point of its parent, used for the leaf to decide which side is its area
			bRoot: if this is the root of the tree
			return the state of the generated area
	*/
	int generateArea(const ContourNode& root, QList<UnCertaintyArea*>& areas, UnCertaintyArea* pParent=NULL);


	// build the contour tree
	void buildContourTree();

	/*
		generate a closed contour from a point list, find the corresponding border
		params:
			contour: store the result contour
			pts: the given point list
			ptRef: the reference point, which is outside this area and on the border.
	*/
	void generateLeafArea(ClosedContour& contour, const QList<QPointF>& pts, QPointF ptRef);



	/*
		generate a closed contour from a point list, find the corresponding border
		no reference point
		params:
			contour: store the result contour
			pts: the given point list		
			bLeft: if the left part
	*/
	void generateLeafArea(ClosedContour& contour, const QList<QPointF>& pts,bool bLeft=false);

	/*
		generate the uncertainty area which is not leaf
	*/
	void generateNoLeafArea(ClosedContour& contour, const ContourNode& node);

	/*
		check which border of the point on: 0,1,2,3:left, bottom, right, top
	*/
	int CheckBorderPoint(const QPointF& pt);

	// calculate the relative distance of the point from the corner point 0
	double PointDistance(const QPointF& pt);

	/*
		return the point in front using our topology order,0 or 1
	*/
	int PointCompare(const QPointF& pt0, const QPointF& pt1);
private:
	QList<ContourLine> _listClosed;				// list of closed contours
	QList<ContourLine> _listUnClosed;			// list of unclosed contours
	QList<ContourNode> _listTreeNode;		// tree node of the end point
	int _nWidth;								// width of the region
	int _nHeight;								// height of the region	
	QPointF _arrCorners[4];						// array of corner points

	bool _bConvex;								// whether the field is convex or concave



	// represent an end point
	struct EndPoint {
		int _nContourIndex;		// index of the contour it belong to
		int _nIndex;			// index of the EndPoint: 0:first, 1:last
		double _dbDis;			// distance from the base corner
		bool operator<(const EndPoint& pt) const {
			return _dbDis < pt._dbDis;
		}
		//EndPoint() {}
		EndPoint(int nContourIndex, int nIndex, double dbDis) :_nContourIndex(nContourIndex), _nIndex(nIndex), _dbDis(dbDis) {}
	};
};