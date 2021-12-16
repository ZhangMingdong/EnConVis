#include "ContourGenerator.h"

//#include "def.h"
#include <QDebug>

ContourGenerator* ContourGenerator::s_pInstance = NULL;

ContourGenerator* ContourGenerator::GetInstance() {
	if (!s_pInstance)
		s_pInstance = new ContourGenerator();
		//s_pInstance = new ContourGeneratorMS();
	return s_pInstance;
}

void ContourGenerator::Release() {
	if (s_pInstance)
		delete s_pInstance;
}



/*
	check if the two points are the same one
*/
bool checkPoints(int arrIndices1[4], int arrIndices2[4]) {
	return (arrIndices1[0] == arrIndices2[0]
		&& arrIndices1[1] == arrIndices2[1]
		&& arrIndices1[2] == arrIndices2[2]
		&& arrIndices1[3] == arrIndices2[3])
		||
		(arrIndices1[0] == arrIndices2[2]
			&& arrIndices1[1] == arrIndices2[3]
			&& arrIndices1[2] == arrIndices2[0]
			&& arrIndices1[3] == arrIndices2[1]);
}

LineSeg::LineSeg(int i11, int j11, int i12, int j12, int i21, int j21, int i22, int j22) {
	_arrIndices[0] = i11;
	_arrIndices[1] = j11;
	_arrIndices[2] = i12;
	_arrIndices[3] = j12;
	_arrIndices[4] = i21;
	_arrIndices[5] = j21;
	_arrIndices[6] = i22;
	_arrIndices[7] = j22;
}

int LineSeg::StartOrEndWith(int arrIndices[8]) {
	if ((_arrIndices[0] == arrIndices[0]
		&& _arrIndices[1] == arrIndices[1]
		&& _arrIndices[2] == arrIndices[2]
		&& _arrIndices[3] == arrIndices[3])
		||
		(_arrIndices[0] == arrIndices[2]
			&& _arrIndices[1] == arrIndices[3]
			&& _arrIndices[2] == arrIndices[0]
			&& _arrIndices[3] == arrIndices[1])
		)
	{
		return 3;
	}
	else if ((
		_arrIndices[4] == arrIndices[0]
		&& _arrIndices[5] == arrIndices[1]
		&& _arrIndices[6] == arrIndices[2]
		&& _arrIndices[7] == arrIndices[3])
		||
		(
			_arrIndices[4] == arrIndices[2]
			&& _arrIndices[5] == arrIndices[3]
			&& _arrIndices[6] == arrIndices[0]
			&& _arrIndices[7] == arrIndices[1])

		)
	{
		return 1;
	}
	else if ((
		_arrIndices[0] == arrIndices[4]
		&& _arrIndices[1] == arrIndices[5]
		&& _arrIndices[2] == arrIndices[6]
		&& _arrIndices[3] == arrIndices[7])
		||
		(
			_arrIndices[0] == arrIndices[6]
			&& _arrIndices[1] == arrIndices[7]
			&& _arrIndices[2] == arrIndices[4]
			&& _arrIndices[3] == arrIndices[5])

		)
	{
		return 4;
	}
	else if ((
		_arrIndices[4] == arrIndices[4]
		&& _arrIndices[5] == arrIndices[5]
		&& _arrIndices[6] == arrIndices[6]
		&& _arrIndices[7] == arrIndices[7])
		||
		(
			_arrIndices[4] == arrIndices[6]
			&& _arrIndices[5] == arrIndices[7]
			&& _arrIndices[6] == arrIndices[4]
			&& _arrIndices[7] == arrIndices[5])

		)
	{
		return 2;
	}
	return 0;
}



void ContourGenerator::Generate(const double* data
	, QList<ContourLine>& listContour
	, double fValue
	, int nWidth
	, int nHeight
) {
	// 0.set value
	_data = data;
	// _fValue = fValue + 1.4e-45; // using -45 may cause some contour missing segments
	_fValue = fValue + 1.4e-45;
	_nWidth = nWidth;
	_nHeight = nHeight;

	// 1.generate line strips
	generateLineStrip();

	// 2 calculate segment in domain
	combineLineStrip(listContour);
}

void ContourGenerator::generateLineStrip() {
	// 1.generate segment
	for (int i = 1; i < _nHeight; i++) {
		for (int j = 1; j < _nWidth; j++) {
			int grid[4][2] = {
				{ i - 1, j - 1 }
				, { i - 1, j }
				, { i, j }
				, { i, j - 1 }
			};
			double v[4] = {
				_data[((i - 1)) * _nWidth + j - 1],
				_data[((i - 1)) * _nWidth + j],
				_data[(i)* _nWidth + j],
				_data[(i)* _nWidth + j - 1],
			};
			int t = 0;
			for (int k = 0; k < 4; k++) if (v[k] > _fValue) t++;
			if (t == 1)
			{
				int corner = 0;
				while (v[corner] < _fValue) corner++;
				int c1 = (corner + 1) % 4;
				int c2 = (corner + 3) % 4;
				_listSeg.append(LineSeg(grid[corner][0], grid[corner][1]
					, grid[c1][0], grid[c1][1]
					, grid[corner][0], grid[corner][1]
					, grid[c2][0], grid[c2][1]
				));
			}
			else if (t == 2)
			{
				bool state[4] = {
					v[0] > _fValue
					, v[1] > _fValue
					, v[2] > _fValue
					, v[3] > _fValue
				};
				if (state[1] != state[2] && state[2] != state[3]) {
					// 1,3 : 2,4
					_listSeg.append(LineSeg(grid[0][0], grid[0][1]
						, grid[1][0], grid[1][1]
						, grid[1][0], grid[1][1]
						, grid[2][0], grid[2][1]
					));
					_listSeg.append(LineSeg(grid[2][0], grid[2][1]
						, grid[3][0], grid[3][1]
						, grid[3][0], grid[3][1]
						, grid[0][0], grid[0][1]
					));
				}
				else {
					int c0 = 0;
					while (state[c0] != state[c0 + 1]) c0++;
					int c1 = (c0 + 1) % 4;
					int c2 = (c0 + 2) % 4;
					int c3 = (c0 + 3) % 4;
					_listSeg.append(LineSeg(grid[c0][0], grid[c0][1]
						, grid[c3][0], grid[c3][1]
						, grid[c2][0], grid[c2][1]
						, grid[c1][0], grid[c1][1]
					));

				}
			}
			else if (t == 3)
			{
				int corner = 0;
				while (v[corner] > _fValue) corner++;
				int c1 = (corner + 1) % 4;
				int c2 = (corner + 3) % 4;
				_listSeg.append(LineSeg(grid[corner][0], grid[corner][1]
					, grid[c1][0], grid[c1][1]
					, grid[corner][0], grid[corner][1]
					, grid[c2][0], grid[c2][1]
				));
			}
		}
	}
}

QPointF ContourGenerator::getIntersectionPt(int arrIndex[4]) {
	double v[2] = {
		_data[(arrIndex[0]) * _nWidth + arrIndex[1]],
		_data[(arrIndex[2]) * _nWidth + arrIndex[3]]
	};

	double bias0 = abs(v[0] - _fValue);
	double bias1 = abs(v[1] - _fValue);

	double x0 = bias1 / (bias0 + bias1)*arrIndex[0] + bias0 / (bias0 + bias1)*arrIndex[2];
	double y0 = bias1 / (bias0 + bias1)*arrIndex[1] + bias0 / (bias0 + bias1)*arrIndex[3];

	return QPointF(y0, x0);
}

void ContourGenerator::combineLineStrip(QList<ContourLine>& listContour) {
	// process until the strip list is empty
	while (!_listSeg.empty())
	{
		// 1.get the first segment as the start point
		LineSeg lineSeg = _listSeg[0];
		_listSeg.removeFirst();
		ContourLine contour;

		// 2.add the first two points
		contour._listPt.append(getIntersectionPt(lineSeg._arrIndices));
		contour._listPt.append(getIntersectionPt(lineSeg._arrIndices + 4));

		// 3.record the position of the first two points
		int arrLastIndices[8] = {
			lineSeg._arrIndices[0],
			lineSeg._arrIndices[1],
			lineSeg._arrIndices[2],
			lineSeg._arrIndices[3],
			lineSeg._arrIndices[4],
			lineSeg._arrIndices[5],
			lineSeg._arrIndices[6],
			lineSeg._arrIndices[7]
		};
		// 4.go on to finished the current contour
		bool bContinue = true; // this contour is continue
		while (bContinue && !_listSeg.empty())
		{
			bContinue = false;
			for (int i = 0, len = _listSeg.length(); i < len; i++)
			{
				LineSeg lineSeg = _listSeg[i];
				bool bTail = true;				// add the new point to tail

				int nStartOrEndWith = lineSeg.StartOrEndWith(arrLastIndices);
				if (nStartOrEndWith) {
					bContinue = true;
					switch (nStartOrEndWith)
					{
					case 3:
					{
						arrLastIndices[0] = lineSeg._arrIndices[4];
						arrLastIndices[1] = lineSeg._arrIndices[5];
						arrLastIndices[2] = lineSeg._arrIndices[6];
						arrLastIndices[3] = lineSeg._arrIndices[7];
						bTail = false;
					}
					break;
					case 1:
					{
						arrLastIndices[0] = lineSeg._arrIndices[0];
						arrLastIndices[1] = lineSeg._arrIndices[1];
						arrLastIndices[2] = lineSeg._arrIndices[2];
						arrLastIndices[3] = lineSeg._arrIndices[3];
						bTail = false;
					}
					break;
					case 4:
					{
						arrLastIndices[4] = lineSeg._arrIndices[4];
						arrLastIndices[5] = lineSeg._arrIndices[5];
						arrLastIndices[6] = lineSeg._arrIndices[6];
						arrLastIndices[7] = lineSeg._arrIndices[7];
					}
					break;
					case 2:
					{
						arrLastIndices[4] = lineSeg._arrIndices[0];
						arrLastIndices[5] = lineSeg._arrIndices[1];
						arrLastIndices[6] = lineSeg._arrIndices[2];
						arrLastIndices[7] = lineSeg._arrIndices[3];
					}
					break;
					default:
						break;
					}

					// add the first two points
					if (bTail)
					{
						QPointF newPt = getIntersectionPt(arrLastIndices + 4);
						contour._listPt.append(newPt);
					}
					else
					{
						QPointF newPt = getIntersectionPt(arrLastIndices);
						contour._listPt.insert(0, newPt);
					}

					_listSeg.removeAt(i);
					break;
				}
			}
		}
		// check whether contour is closed
		contour._nClosed = checkPoints(arrLastIndices, arrLastIndices + 4);
		// old version
		//{
		//	QPointF pt1 = contour._listPt[0];
		//	QPointF pt2 = contour._listPt[contour._listPt.size() - 1];
		//	if (abs(pt1.x() - pt2.x()) < g_fDifference && (abs(pt1.y() - pt2.y()) < g_fDifference)) contour._nClosed = 1;
		//	else contour._nClosed = 0;
		//}
		// ignore the contour shorter than 4
		if (contour._listPt.size() > 2)
		{
			listContour.append(contour);
		}


	}
}