#include "ContourBandGenerator.h"
#include <QDebug>
#include "def.h"



/*
	check which border of the point on: 0,1,2,3:left, bottom, right, top
*/
int ContourBandGenerator::CheckBorderPoint(const QPointF& pt){
	double fX = pt.x();
	double fY = pt.y();
	if (abs(fX) < g_fDifference) return 0;
	else if (abs(fY) < g_fDifference) return 1;
	else if (abs(fX - _nWidth + 1) < g_fDifference) return 2;
	else if (abs(fY - _nHeight + 1) < g_fDifference) return 3;
	else{		
		qDebug() << "error";
		return -1;
	}
}

// calculate the relative distance of the point from the corner point 0
double ContourBandGenerator::PointDistance(const QPointF& pt){

	double dbX = pt.x();
	double dbY = pt.y();
	double dbBiasX = dbX / (_nWidth - 1);
	double dbBiasY = dbY / (_nHeight - 1);
	if (dbBiasX < g_fDifference) return 1 - dbBiasY;
	else if (dbBiasY < g_fDifference) return 1 + dbBiasX;
	else if (abs(dbBiasX - 1) < g_fDifference) return 2 + dbBiasY;
	else if (abs(dbBiasY - 1) < g_fDifference) return 4 - dbBiasX;
	else{
		qDebug() << "error ContourBandGenerator::PointDistance(const QPointF& pt)";
		return -1;
	}
}

/*
	return the point in front using our topology order,0 or 1
*/
int ContourBandGenerator::PointCompare(const QPointF& pt0, const QPointF& pt1){
	// first check which border they are
	int nState0 = CheckBorderPoint(pt0);
	int nState1 = CheckBorderPoint(pt1);
	double fX0 = pt0.x();
	double fY0 = pt0.y();
	double fX1 = pt1.x();
	double fY1 = pt1.y();

	if (nState0 < nState1) return 0;
	else if (nState0 > nState1) return 1;
	else{
		switch (nState0)
		{
		case 0:
			if (fY0 > fY1) return 0;
			else return 1;
		case 1:
			if (fX0 < fX1) return 0;
			else return 1;
		case 2:
			if (fY0 < fY1) return 0;
			else return 1;
		case 3:
			if (fX0 > fX1) return 0;
			else return 1;
		default:
			return -1;
		}
	}
}

ContourBandGenerator::ContourBandGenerator(bool bConvex):_bConvex(bConvex)
{
}

ContourBandGenerator::~ContourBandGenerator()
{
}

void ContourBandGenerator::addContourToList(ContourLine contour) {
	if (contour._nClosed) _listClosed.push_back(contour);
	else _listUnClosed.push_back(contour);
}
void ContourBandGenerator::Generate(QList<UnCertaintyArea*>& areas
	, const QList<ContourLine>& contourMin
	, const QList<ContourLine>& contourMax
	, int nWidth
	, int nHeight){
	// 0.set value
	_nWidth = nWidth;
	_nHeight = nHeight;

	_arrCorners[0] = QPointF(0, _nHeight - 1);
	_arrCorners[1] = QPointF(0, 0);
	_arrCorners[2] = QPointF(0 + _nWidth - 1, 0);
	_arrCorners[3] = QPointF(0 + _nWidth - 1, 0 + _nHeight - 1);

	// 1.put the contours into two lists
	for each (ContourLine contour in contourMin)
	{
		contour._nState = -1;
		addContourToList(contour);
	}

	for each (ContourLine contour in contourMax)
	{
		contour._nState = 1;
		addContourToList(contour);
	}

	if (_listUnClosed.length() == 1) {
		qDebug() << "error";
	}

	// 2.handle the unclosed contours
	generateFromUnClosedContours(areas);

	// 3.handle the closed contours
	insertClosedContours(areas);

	// 4.handle the embedded area
	generateEmbeddedArea(areas);

}

void ContourBandGenerator::generateFromUnClosedContours(QList<UnCertaintyArea*>& areas){
	// 1.generate the sorted endpoint lists
	generateEndPointsList();
	// 2.generate the contour tree
	buildContourTree();
	// 3.generate the area
	if (_listTreeNode.isEmpty()) {
		// 3.1. if areas are empty, add a new area of the region
		UnCertaintyArea* pAreaNew = new UnCertaintyArea;
		pAreaNew->_contour._listPt.append(QPointF(0, _nHeight - 1));
		pAreaNew->_contour._listPt.append(QPointF(0, 0));
		pAreaNew->_contour._listPt.append(QPointF(_nWidth - 1, 0));
		pAreaNew->_contour._listPt.append(QPointF(_nWidth - 1, _nHeight - 1));
		pAreaNew->_contour.GenerateBounding();
		pAreaNew->_nState = _bConvex ? 1 : -1;
		areas.append(pAreaNew);
	}
	/*
	else if (_listTreeNode.length() == 1) {
		// 3.1. if areas are empty, add a new area of the region
		UnCertaintyArea* pAreaNew = new UnCertaintyArea;
		pAreaNew->_contour._listPt= _listUnClosed[_listTreeNode[0]._nContourIndex]._listPt;
		pAreaNew->_contour.GenerateBounding();
		pAreaNew->_nState = _bConvex ? 1 : -1;
		areas.append(pAreaNew);
	}
	*/
	else {
		generateArea(_listTreeNode[0], areas);
	}

}

void ContourBandGenerator::insertClosedContours(QList<UnCertaintyArea*>& areas){

	// 1.handle each closed contour
	for each (ContourLine contour in _listClosed)
	{
		// 1.generate a new area
		UnCertaintyArea* pAreaNew=new UnCertaintyArea;
		for each (QPointF pt in contour._listPt)
		{
			pAreaNew->_contour._listPt.append(pt);
		}
		pAreaNew->_contour._listPt.removeLast();	// the last one is duplicate
		pAreaNew->_contour.GenerateBounding();
		pAreaNew->_nState = contour._nState*2;		// set the state of the new area to the state of the contour, this will be used when this area generate to a standalone area
	
		// 2.search every area (which can cover the whole space), put the closed contour into the one it belong to
		for (int i = 0, len = areas.length(); i < len; i++)
		{
			int nState = areas[i]->InsertContour(pAreaNew);
			if (nState)
			{
			//	if (nState == 2) {
			//		areas.append(pAreaNew);
			//	}
				break;
			}
		}
	}
}

void ContourBandGenerator::generateEmbeddedArea(QList<UnCertaintyArea*>& areas){
	// 1.put all the areas in a queue
	QQueue<UnCertaintyArea*> queueAreas;
	for each (UnCertaintyArea* area in areas)
	{
		queueAreas.enqueue(area);
	}
	// 2.handle the areas in the queue, until it return false, which means it is empty
	while (handleAnAreaInQueue(queueAreas, areas));
}

bool ContourBandGenerator::handleAnAreaInQueue(QQueue<UnCertaintyArea*>& queueAreas, QList<UnCertaintyArea*>& areas){

	// 1.finished if the queue is empty
	if (queueAreas.empty()) return false;

	// 2.dequeue an area and handle its embedded areas 
	UnCertaintyArea* area = queueAreas.dequeue();

	bool bNewUncertain = false;
	for each (UnCertaintyArea* embedded in area->_listEmbeddedArea)
	{
		// if the outer area is uncertainty, this embedded area can be decide by the state of its contour, 
		// which has already done
		if (area->_nState == 1 || area->_nState==-1) embedded->_nState = 0;
		else if (area->_nState == 0) embedded->_nState/=2;
		else if (area->_nState != embedded->_nState) bNewUncertain = true;
		areas.append(embedded);
		queueAreas.enqueue(embedded);
	}
	if (bNewUncertain){
		area->_nState = 0;
		for each (UnCertaintyArea* embedded in area->_listEmbeddedArea)
		{
			embedded->_nState /= 2;
		}
	}

	return true;
}

void ContourBandGenerator::generateEndPointsList(){
	// 1.sort the end point
	QList<EndPointRaw> listEndPoints;			// list of endpoints
	for (int i = 0, len = _listUnClosed.size(); i < len; i++)
	{
		// get the contour
		ContourLine contour = _listUnClosed[i];

		// get the endpoints of the contour
		QPointF endpoints[2] = {
			contour._listPt[0]
			, contour._listPt[contour._listPt.size() - 1]
		};

		for (int j = 0; j < 2; j++)
		{
			QPointF pt = endpoints[j];
			int insertPos = listEndPoints.size();
			while (insertPos>0 && PointCompare(pt, listEndPoints[insertPos - 1]._ptEndPoint) == 0)
				insertPos--;
			listEndPoints.insert(insertPos, EndPointRaw(i,j,pt));
		}
	}
	// 2.build contourtree node from the sorted endpoints
	for (int i = 0, len = listEndPoints.size(); i < len; i++)
	{
		ContourNode treeNode;
		treeNode._nContourIndex = listEndPoints[i]._nContourIndex;
		treeNode._nReverse = listEndPoints[i]._nEndPointIndex;
		treeNode._nMatched = 0;
		_listTreeNode.push_back(treeNode);
	}
}

int ContourBandGenerator::generateArea(const ContourNode& root
	, QList<UnCertaintyArea*>& areas
	, UnCertaintyArea* pParent){

	UnCertaintyArea* pArea = new UnCertaintyArea();
	pArea->_pParent = NULL;

	if (pParent==NULL){	
		// for root, another area should be generated
		if (root._children.empty()) {
			// for a root as leaf
			// 这种情况下，状态应该是无法确定的。
			// 1.generate the root area
			ContourLine contour = _listUnClosed[root._nContourIndex];
			generateLeafArea(pArea->_contour
				, contour._listPt);
			pArea->_nState = contour._nState;


			UnCertaintyArea* pLeftArea = new UnCertaintyArea();
			generateLeafArea(pLeftArea->_contour
				, contour._listPt,true);
			pLeftArea->_nState = contour._nState;
			pLeftArea->_pParent = pArea;
			pArea->_listChildren.append(pLeftArea);

		}
		else {
			// 1.generate the root area
			ContourLine contour = _listUnClosed[root._nContourIndex];
			generateLeafArea(pArea->_contour
				, contour._listPt
				, _listUnClosed[root._children[0]._nContourIndex]._listPt[0]);
			pArea->_nState = contour._nState * 2;

			// 2.recursively generate from the root
			int nChildState = generateArea(root, areas, pArea);

			// 3.set the state of the root area based on the state of the area of the root
			if (nChildState == 0) pArea->_nState = contour._nState;
			else if (nChildState == 2 || nChildState == -2) pArea->_nState = contour._nState * 2;
			else pArea->_nState = 0;
		}
	}
	else if (root._children.empty()){
		// generate leaf area

		// 1.generate the area
		ContourLine contour = _listUnClosed[root._nContourIndex];
		generateLeafArea(pArea->_contour
			, contour._listPt
			,pParent->_contour._listPt[0]);

		// 2.set the region in the children list of the parent
		pParent->_listChildren.append(pArea);

		// 3.set the state of the root area based on the state of the area of the root
		if (pParent->_nState == 0) pArea->_nState = contour._nState;
		else if (pParent->_nState == 2 || pParent->_nState == -2) pArea->_nState = contour._nState * 2;
		else pArea->_nState = 0;
	}
	else{	// generate an uncertainty area between more than one contours
		
		// 1.generate the area
		pParent->_listChildren.append(pArea);
		generateNoLeafArea(pArea->_contour, root);

		// 2.calculate state
		ContourLine contour0 = _listUnClosed[root._nContourIndex];
		bool bUncertain = false;
		for each (ContourNode child in root._children)
		{
			if (_listUnClosed[child._nContourIndex]._nState != contour0._nState)
			{
				bUncertain = true;
			}
		}
		if (bUncertain) pArea->_nState = 0;								// uncertain if two contours have different state
		else if (pParent->_nState == 0) pArea->_nState = contour0._nState;	// as the state of the contour is the parent is uncertain
		else if (pParent->_nState == 2 || pParent->_nState == -2) pArea->_nState = contour0._nState * 2;
		else pArea->_nState = 0;											// can not decide yet

		// 3.generate the children area, and check whether there's child with uncertainty state
		bool bUncertainChild = false;
		for each (ContourNode node in  root._children)
		{
			int nChildState = generateArea(node, areas, pArea);
			if (nChildState == 0) bUncertainChild = true;
		}
		if (pArea->_nState == -2||pArea->_nState==2)
		{
			if (bUncertainChild)
			{
				// modify state according to children
				pArea->_nState = contour0._nState;
			}
		}
	}

	areas.push_back(pArea);
	return pArea->_nState;
}

void ContourBandGenerator::buildContourTree(){
	// 1.match nodes, match the leaf nodes
	for (int i = _listTreeNode.size() - 1; i > 0; i--)
	{
		if (_listTreeNode[i]._nContourIndex == _listTreeNode[i - 1]._nContourIndex)
		{
			// match the two nodes
			_listTreeNode[i - 1]._nMatched = 1;
			_listTreeNode.removeAt(i);
		}
	}

	// 2.generate children
	int nBegin = -1;	// 需要匹配的结点位置
	int nState = 0;		// 1 means looking for the end point
	int i = 0;			// 当前查找位置
	while (i < _listTreeNode.length())
	{
		// 2.1.check the current state
		if (nState == 0)
		{
			// no begin node
			if (_listTreeNode[i]._nMatched == 0)
			{
				// find the left node
				nBegin = i;
				nState = 1;
			}
		}
		else{
			// already find begin node
			if (_listTreeNode[i]._nMatched == 0)
			{
				// find an unmatched node
				if (_listTreeNode[i]._nContourIndex== _listTreeNode[nBegin]._nContourIndex)
				{
					// the nodes matched
					// we have nodes nBegin...i
					// set the first to matched, and put the nodes between as its children, and remove the last one
					_listTreeNode[nBegin]._nMatched = 1;
					for (int j = nBegin + 1; j < i;j++)
					{
						_listTreeNode[nBegin]._children.append(_listTreeNode[j]);
					}
					for (int j = i; j > nBegin; j--)
					{
						_listTreeNode.removeAt(j);
					}
					i = 0;
					nBegin = -1;
					nState = 0;
					continue;
				}
				else{
					// can not match, restart
					nBegin = i;
				}
			}
		}
		i++;
	}

	// 3.set the other nodes as the children of the first leaf nodes
	while (_listTreeNode.length()>1)
	{
		// put the children of the first node into a new list
		QList<ContourNode> listNew;
		for (int i = _listTreeNode[0]._children.length() - 1; i >= 0; i--)
		{
			listNew.append(_listTreeNode[0]._children[i]);
			_listTreeNode[0]._children.removeAt(i);
		}
		// put all the nodes except the first one into the children list of the first node
		for (int i = _listTreeNode.length() - 1; i > 0; i--)
		{
			_listTreeNode[0]._children.append(_listTreeNode[i]);
			_listTreeNode.removeAt(i);
		}
		// append the first node to the new list
		listNew.append(_listTreeNode[0]);
		// use the new list to replace the original list
		_listTreeNode = listNew;
	}
}

void ContourBandGenerator::generateNoLeafArea(ClosedContour& contour, const ContourNode& node){
	// 1.generate the list of all the end points
	QList<EndPoint> listEnd;
	ContourLine contour0 = _listUnClosed[node._nContourIndex];

	listEnd.push_back(EndPoint(node._nContourIndex, 0, PointDistance(contour0._listPt[0])));
	listEnd.push_back(EndPoint(node._nContourIndex, 1, PointDistance(contour0._listPt[contour0._listPt.length() - 1])));

	for (int i = 0, len = node._children.length(); i < len; i++)
	{
		contour0 = _listUnClosed[node._children[i]._nContourIndex];

		listEnd.push_back(EndPoint(node._children[i]._nContourIndex, 0, PointDistance(contour0._listPt[0])));
		listEnd.push_back(EndPoint(node._children[i]._nContourIndex, 1, PointDistance(contour0._listPt[contour0._listPt.length() - 1])));
	}

	// 2.Sort the end point according to their distance from the base point
	qSort(listEnd);
	int i = 0;
	int len = listEnd.length();
	// if the last and the first is a pair, put the last in the front.
	EndPoint ptLast = listEnd[len - 1];
	if (ptLast._nContourIndex == listEnd[0]._nContourIndex)
	{
		listEnd.removeLast();
		listEnd.insert(listEnd.begin(), ptLast);
	}

	// 3.connect the contour lines
	for (int i = 0; i < len; i += 2)
	{
		Q_ASSERT(listEnd[i]._nContourIndex == listEnd[i + 1]._nContourIndex);
		if (i>0)
		{
			for (int j = int(listEnd[i - 1]._dbDis)+1; j <= int(listEnd[i]._dbDis); j++)
			{
				contour._listPt.append(_arrCorners[j]);
			}
		}
		// find an contour line, insert its points
		if (listEnd[i]._nIndex == 0)
		{
			for each (QPointF pt in _listUnClosed[listEnd[i]._nContourIndex]._listPt)
			{
				contour._listPt.append(pt);
			}
		}
		else{
			for (int j = _listUnClosed[listEnd[i]._nContourIndex]._listPt.length() - 1; j >= 0; j--)
			{
				contour._listPt.append(_listUnClosed[listEnd[i]._nContourIndex]._listPt[j]);
			}
		}
	}

	// 4.insert the last corner point
	int nLastPoint = listEnd[len - 1]._dbDis;
	int nFirstPoint = listEnd[0]._dbDis;
	if (nLastPoint != nFirstPoint)
	{
		int j = (nLastPoint + 1) % 4;
		while (true)
		{
			contour._listPt.append(_arrCorners[j]);
			if (j == nFirstPoint) break;
			j = (j + 1) % 4;
		}
	}

	// 5. generate bounding box
	contour.GenerateBounding();
}

void ContourBandGenerator::generateLeafArea(ClosedContour& contour
	, const QList<QPointF>& pts
	, QPointF ptRef){

	// 0.check the contour
	QPointF pt0 = pts[0];
	QPointF pt1 = pts[pts.length() - 1];

	int nReverse = PointCompare(pt0, pt1);

	// 1.add the point list into the contour
	if (nReverse==0)
	{
		for each (QPointF pt in pts) contour._listPt.push_back(pt);
	}
	else{
		for (int i = pts.length() - 1; i >= 0;i--) contour._listPt.push_back(pts[i]);

		// exchange pt0 and pt1
		QPointF ptTemp = pt0; pt0 = pt1; pt1 = ptTemp;
	}


	int n0 = PointCompare(ptRef, pt0);
	int n1 = PointCompare(ptRef, pt1);
	int nBorder0 = CheckBorderPoint(pt0);
	int nBorder1 = CheckBorderPoint(pt1);

	// 2.add the border points
	if (n1 == n0)	{
		for (int i = nBorder1; i > nBorder0; i--) contour._listPt.push_back(_arrCorners[i]);
	}
	else{
		if (nBorder0!=nBorder1)
		{
			for (int i = nBorder1 + 1; i <= nBorder0 + 4; i++) contour._listPt.push_back(_arrCorners[i % 4]);
		}
	}

	// 3. generate bounding box
	contour.GenerateBounding();
}


void ContourBandGenerator::generateLeafArea(ClosedContour& contour
	, const QList<QPointF>& pts
	, bool bLeft) {

	// 0.check the contour
	QPointF pt0 = pts[0];
	QPointF pt1 = pts[pts.length() - 1];

	int nReverse = PointCompare(pt0, pt1);

	// 1.add the point list into the contour
	if (nReverse == 0)
	{
		for each (QPointF pt in pts) contour._listPt.push_back(pt);
	}
	else {
		for (int i = pts.length() - 1; i >= 0; i--) contour._listPt.push_back(pts[i]);

		// exchange pt0 and pt1
		QPointF ptTemp = pt0; pt0 = pt1; pt1 = ptTemp;
	}

	int nBorder0 = CheckBorderPoint(pt0);
	int nBorder1 = CheckBorderPoint(pt1);

	// 2.add the border points
	if (bLeft) {
		if (nBorder0 != nBorder1)
		{
			for (int i = nBorder1 + 1; i <= nBorder0 + 4; i++) contour._listPt.push_back(_arrCorners[i % 4]);
		}
	}
	else {
		for (int i = nBorder1; i > nBorder0; i--) contour._listPt.push_back(_arrCorners[i]);
	}

	// 3. generate bounding box
	contour.GenerateBounding();
}

void ClosedContour::GenerateBounding(){
	_fX0 = _fX1 = _listPt[0].x();
	_fY0 = _fY1 = _listPt[0].y();
	for each (QPointF pt in _listPt)
	{
		if (pt.x() < _fX0) _fX0 = pt.x();
		if (pt.x() > _fX1) _fX1 = pt.x();
		if (pt.y() < _fY0) _fY0 = pt.y();
		if (pt.y() > _fY1) _fY1 = pt.y();
	}
}