#include "mychartwidget.h"
#include <gl/GLU.h>





#include "MeteModel.h"
#include "LayerLayout.h"
#include "DataField.h"
#include "ColorMap.h"

#include "Switch.h"

#include <QDebug>
#include <QWheelEvent>





MyChartWidget::MyChartWidget(QWidget *parent)
	: MyGLWidget(parent)
{



}

MyChartWidget::~MyChartWidget()
{
}
void SetClusterColor(int nIndex, double dbOpacity = .8) {
	glColor4f(ColorMap::GetCategory10D(nIndex, 0)
		, ColorMap::GetCategory10D(nIndex, 1)
		, ColorMap::GetCategory10D(nIndex, 2)
		, dbOpacity);
}
void MyChartWidget::paint() {
	drawPoints();
	return;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_dbFovy, (_nHeight == 0) ? 1.0 : (double)_nWidth / (double)_nHeight, m_dbZNear, m_dbZFar);
	glMatrixMode(GL_MODELVIEW);


	glLineWidth(1.0f);
	glColor3f(1.0, 0, 0);

	glPushMatrix();
	

	//	
	/*
	glScaled(.1, .1, 1);
	glTranslatef(-5, -5, 0);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0,0);
	glVertex3f(0, 10,0);
	glVertex3f(10, 10,0);
	glVertex3f(10, 0,0);
	glEnd();
	*/
	/*
	// rectangle
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-10, -10);
	glVertex2d(-10, 10);
	glVertex2d(10, 10);
	glVertex2d(10, -10);	
	glEnd();
	*/

	// axis
	/*
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(0, -10000);
	glVertex2d(0, 10000);
	glVertex2d(-10000, 0);
	glVertex2d(10000, 0);
	glEnd();
	*/

	drawPoints();

	//drawLines();
	drawHierarchy();

	drawClusters();


	glPopMatrix();
}

// draw the mds of data points
void MyChartWidget::drawPoints() {
	int nEnsemble = _pModelE->GetEnsembleLen();
	int nClusters = _pModelE->GetClusters();

	// draw points
	glPointSize(5);
	glBegin(GL_POINTS);


	for (int i = 0; i < nEnsemble; i++)
	{
		SetClusterColor(_pModelE->GetLabel(i));
		glVertex2d(_pModelE->GetPC(i, 0)/60, _pModelE->GetPC(i, 1) / 60);
	}

	glEnd();
}

void MyChartWidget::drawLines() {

	int nEnsemble = _pModelE->GetEnsembleLen();
	int nClusters = _pModelE->GetClusters();

	// draw lines
	DPoint2 arrTarget[50];
	int arrStates[50];
	int arrLabels[50];
	for (size_t i = 0; i < 50; i++) {
		arrStates[i] = 0;
		arrLabels[i] = i;
	}

	int nSource;
	int nTarget;
	glBegin(GL_LINES);
	qDebug() << "draw chart lines";
	for (int i = 0; i < nEnsemble - nClusters; i++)
	{
		_pModelE->GetMerge(i, nSource, nTarget);
		SetClusterColor(_pModelE->GetLabel(nTarget));

		DPoint2 ptSource = arrStates[arrLabels[nSource]] ? arrTarget[arrLabels[nSource]] : DPoint2(_pModelE->GetPC(arrLabels[nSource], 0), _pModelE->GetPC(arrLabels[nSource], 1));
		DPoint2 ptTarget = arrStates[arrLabels[nTarget]] ? arrTarget[arrLabels[nTarget]] : DPoint2(_pModelE->GetPC(arrLabels[nTarget], 0), _pModelE->GetPC(arrLabels[nTarget], 1));

		glVertex2d(ptSource.x, ptSource.y);
		glVertex2d(ptTarget.x, ptTarget.y);

		DPoint2 ptMid = (ptSource + ptTarget)*0.5;

		// update source and target
		for (int j = 0; j < nEnsemble; j++)
		{
			if (arrLabels[j] == arrLabels[nSource] || arrLabels[j] == arrLabels[nTarget]) {
				arrStates[j] = 1;
				arrLabels[j] = arrLabels[nTarget];
				arrTarget[j] = ptMid;
			}
		}

	}
	glEnd();


}

void MyChartWidget::drawHierarchy() {
	glPointSize(10);

	int nEnsemble = _pModelE->GetEnsembleLen();
	int nClusters = _pModelE->GetClusters();

	DPoint3 arrRoots[50];	// roots points of each item
	int arrStates[50];		// states of each item: 0-not merged;1-merged
	int arrLabels[50];		// current label of each item;
	bool arrOutofCluster[50];// whether this cluster has been out of clusters
	for (size_t i = 0; i < nEnsemble; i++) {
		arrStates[i] = 0;
		arrLabels[i] = i;
		arrOutofCluster[i] = false;
	}
	int nSource;
	int nTarget;
	qDebug() << "draw chart hierarchy";
	for (int i = 0; i < nEnsemble-1; i++)
	{
		_pModelE->GetMerge(i, nSource, nTarget);
		int nLabel1 = _pModelE->GetLabel(nTarget);
		int nLabel2 = _pModelE->GetLabel(nSource);
		if (nLabel1 == nLabel2)
			SetClusterColor(_pModelE->GetLabel(nTarget));
		else
			glColor4f(.5, .5, .5, .5);

		DPoint3 ptSource = arrStates[arrLabels[nSource]] ? arrRoots[arrLabels[nSource]] : DPoint3(_pModelE->GetPC(arrLabels[nSource], 0), _pModelE->GetPC(arrLabels[nSource], 1),0);
		DPoint3 ptTarget = arrStates[arrLabels[nTarget]] ? arrRoots[arrLabels[nTarget]] : DPoint3(_pModelE->GetPC(arrLabels[nTarget], 0), _pModelE->GetPC(arrLabels[nTarget], 1),0);

		double dbZ = _dbBaseZ + i * _dbStepZ;

		DPoint3 ptMid = (ptSource + ptTarget)*0.5;
		ptMid.z = dbZ;

		// draw lines
		glBegin(GL_LINE_STRIP);
		glVertex3d(ptSource.x, ptSource.y, ptSource.z);
		glVertex3d(ptMid.x, ptMid.y, ptMid.z);
		glVertex3d(ptTarget.x, ptTarget.y, ptTarget.z);
		/*
		glVertex3d(ptSource.x, ptSource.y, ptSource.z);
		glVertex3d(ptSource.x, ptSource.y, dbZ);
		glVertex3d(ptTarget.x, ptTarget.y, dbZ);
		glVertex3d(ptTarget.x, ptTarget.y, ptTarget.z);
		*/
		glEnd();
		// draw points
		glBegin(GL_POINTS);
		//glVertex3d(ptMid.x, ptMid.y, ptMid.z);
		if (nLabel1 != nLabel2) {
			if (!arrOutofCluster[nTarget])
			{
				SetClusterColor(nLabel1);
				glVertex3d(ptTarget.x, ptTarget.y, ptTarget.z);
			}
			if (!arrOutofCluster[nSource]) {
				SetClusterColor(nLabel2);
				glVertex3d(ptSource.x, ptSource.y, ptSource.z);
			}
		}
		glEnd();


		// update source and target
		for (int j = 0; j < nEnsemble; j++)
		{
			if (arrLabels[j] == arrLabels[nSource] || arrLabels[j] == arrLabels[nTarget]) {
				arrStates[j] = 1;
				arrRoots[j] = ptMid;
				arrLabels[j] = arrLabels[nTarget];

				if (nLabel1 != nLabel2) {
					arrOutofCluster[j] = true;
				}
			}
		}

	}
}

void MyChartWidget::init() {

	glEnable(GL_POINT_SMOOTH);
	//qDebug() << "MyChartWidget::init";
}

void MyChartWidget::SetModelE(MeteModel* pModelE) {	
	_pModelE = pModelE;	
}

void MyChartWidget::mouseDoubleClickEvent(QMouseEvent *event) {

}


void MyChartWidget::drawClusters() {
	int nEnsemble = _pModelE->GetEnsembleLen();
	int nClusters = _pModelE->GetClusters();

	// draw each cluster
	for (int i = 0; i < nClusters; i++)
	{
		std::vector<DPoint2> vecPoints;
		for (int j = 0; j < nEnsemble; j++)
		{
			if (_pModelE->GetLabel(j) == i)
			{
				vecPoints.push_back(DPoint2(_pModelE->GetPC(j, 0), _pModelE->GetPC(j, 1)));
			}
		}

		std::vector<DPoint2> vecHull = MathMethod::GetConvexHull(vecPoints);

		// draw area
		SetClusterColor(i, .2);
		glBegin(GL_TRIANGLE_FAN);
		for each (DPoint2 pt in vecHull)
		{
			glVertex2d(pt.x, pt.y);
		}
		glEnd();

		
		// draw border
		if (false) {
			glLineWidth(3);
			glColor4d(0, 0, 0, .5);
			glBegin(GL_LINE_LOOP);
			for each (DPoint2 pt in vecHull)
			{
				glVertex2d(pt.x, pt.y);
			}
			glEnd();
		}
	}
}