#include <QtGui>
#include <QtWidgets/QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QSet>
#include <QMessageBox>
#include <QDockWidget>
#include <QSplitter>

#include "MainWindow.h"
#include "MyMapWidget.h"
#include "MyChartWidget.h"
#include "MeteModel.h"
#include "ArtificialModel.h"
#include "DisplayCtrlWidget.h"
#include "Switch.h"
#include "BandTesser.h"

const QString ShowMap("ShowMap");
const QString ShowGridLines("ShowGridLines");
const QString ShowBackground("ShowBackground");
const QString ShowCB("ShowCB");
const QString ShowVP("ShowVP");
const QString ShowCPP("ShowCPP");
const QString ShowIntersection("ShowIntersection");
const QString ShowUnionB("ShowUnionB");
const QString ShowUnionE("ShowUnionE");
const QString ShowGradientE("ShowGradientE");
const QString ShowLineChart("ShowLineChart");
const QString ShowContourLineTruth("ShowContourLineTruth");
const QString ShowContourLine("ShowContourLine");
const QString ShowContourLineSorted("ShowContourLineSorted");
const QString ShowContourLineSortedSDF("ShowContourLineSortedSDF");
const QString ShowContourLineResampled("ShowContourLineResampled");
const QString ShowContourLineDomainResampled("ShowContourLineDomainResampled");
const QString ShowContourLineSDF("ShowContourLineSDF");
const QString ShowContourLineMin("ShowContourLineMin");
const QString ShowContourLineMax("ShowContourLineMax");
const QString ShowContourLineMean("ShowContourLineMean");
const QString ShowContourLineMedian("ShowContourLineMedian");
const QString ShowContourLineOutlier("ShowContourLineOutlier");
const QString ShowCluster("ShowCluster");
const QString ShowRCBands("ShowRCBands");
const QString ShowBeliefEllipse("ShowBeliefEllipse");


MainWindow::~MainWindow(){
	BandTesser::Release();
	if (_pModel) delete _pModel;
}


MainWindow::MainWindow()
{
	// finished read config file
	setWindowState(Qt::WindowMaximized);


	BandTesser::BuildTess();

	_pModel = MeteModel::CreateModel(g_bArtificialModel);


	createSceneAndView();
	createActions();
	createDockWidgets();
	createConnections();
	populateMenusAndToolBars();

	QSettings settings;
	viewShowMapAction->setChecked(settings.value(ShowMap, true).toBool());
	viewShowGridLinesAction->setChecked(settings.value(ShowGridLines, true).toBool());
	viewShowBackgroundAction->setChecked(settings.value(ShowBackground, true).toBool());

	viewShowCBAction->setChecked(settings.value(ShowCB, true).toBool());
	viewShowVPAction->setChecked(settings.value(ShowVP, true).toBool());
	viewShowCPPAction->setChecked(settings.value(ShowCPP, true).toBool());

	viewShowContourLineAction->setChecked(settings.value(ShowContourLine, true).toBool());
	viewShowContourLineSortedAction->setChecked(settings.value(ShowContourLineSorted, true).toBool());
	viewShowContourLineSortedSDFAction->setChecked(settings.value(ShowContourLineSortedSDF, true).toBool());
	viewShowContourLineResampledAction->setChecked(settings.value(ShowContourLineResampled, true).toBool());
	viewShowContourLineDomainResampledAction->setChecked(settings.value(ShowContourLineDomainResampled, true).toBool());
	viewShowContourLineSDFAction->setChecked(settings.value(ShowContourLineSDF, true).toBool());
	viewShowContourLineMinAction->setChecked(settings.value(ShowContourLineMin, true).toBool());
	viewShowContourLineMaxAction->setChecked(settings.value(ShowContourLineMax, true).toBool());
	viewShowContourLineMeanAction->setChecked(settings.value(ShowContourLineMean, true).toBool());
	viewShowContourLineMedianAction->setChecked(settings.value(ShowContourLineMedian, true).toBool());
	viewShowContourLineOutlierAction->setChecked(settings.value(ShowContourLineOutlier, true).toBool());
	viewShowClusterAction->setChecked(settings.value(ShowCluster, true).toBool());
	viewShowRCBandsAction->setChecked(settings.value(ShowRCBands, true).toBool());

//	_pControlWidget->ui.radioButtonBackgroundCluster->setChecked(settings.value(ShowClusterBV, true).toBool());


    setWindowTitle(tr("Ensembles"));

}

void MainWindow::createSceneAndView(){
	_view3D = new MyMapWidget;
	_view3D->SetModelE(_pModel);

	/*
	if (g_bChart) {
		QSplitter* splitter = new QSplitter();
		splitter->setOrientation(Qt::Vertical);
		splitter->addWidget(_view3D);

		_viewChart = new MyChartWidget;
		_viewChart->SetModelE(_pModel);
		splitter->addWidget(_viewChart);

		setCentralWidget(splitter);

	}
	else 
	*/
	{
		setCentralWidget(_view3D);
	}

	//
}

void MainWindow::createDockWidgets() {
	setDockOptions(QMainWindow::AnimatedDocks);
	QDockWidget::DockWidgetFeatures features =
		QDockWidget::DockWidgetMovable |
		QDockWidget::DockWidgetFloatable;

	_pDisplayCtrlWidget = new DisplayCtrlWidget();

//	QSplitter* splitter = new QSplitter();
//	splitter->setOrientation(Qt::Vertical);
//	splitter->addWidget(_pDisplayCtrlWidget);



	_viewChart = new MyChartWidget;
	_viewChart->SetModelE(_pModel);

	QTabWidget* tabWidget = new QTabWidget(this);
	tabWidget->addTab(_pDisplayCtrlWidget, "Chart");
	tabWidget->addTab(_viewChart, "DR");


	QDockWidget *controlDockWidget = new QDockWidget(
		tr("Control"), this);
	controlDockWidget->setFeatures(features);
	controlDockWidget->setWidget(tabWidget);
	addDockWidget(Qt::RightDockWidgetArea, controlDockWidget);

	/*
	QDockWidget *controlDockWidget = new QDockWidget(
		tr("Control"), this);
	controlDockWidget->setFeatures(features);
	controlDockWidget->setWidget(_pDisplayCtrlWidget);
	addDockWidget(Qt::RightDockWidgetArea, controlDockWidget);
	*/

	/*
	ClusteringWidget* _pWidgetClustering = new ClusteringWidget();
	QDockWidget *pDockWidgetClustering = new QDockWidget(tr("Clustering"), this);
	pDockWidgetClustering->setFeatures(features);
	pDockWidgetClustering->setWidget(_pWidgetClustering);
	addDockWidget(Qt::LeftDockWidgetArea, pDockWidgetClustering);
	*/
}

void MainWindow::populateMenusAndToolBars()
{
	QAction *separator = 0;


	// view
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
	QToolBar* viewToolBar = addToolBar(tr("View"));
	populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
		<< separator
		<< viewShowMapAction
		<< viewShowGridLinesAction
		<< viewShowBackgroundAction
		<< viewShowCBAction
		<< viewShowVPAction
		<< viewShowCPPAction
		<< viewShowRCBandsAction
		<< viewShowContourLineAction
		<< viewShowContourLineSortedAction
		<< viewShowContourLineSDFAction
		<< viewShowContourLineSortedSDFAction
		<< viewShowContourLineResampledAction
		<< viewShowContourLineDomainResampledAction
		<< viewShowContourLineMinAction
		<< viewShowContourLineMaxAction
		<< viewShowClusterAction
		<< viewShowContourLineMeanAction
		//<< viewShowContourLineMedianAction
		<< viewShowContourLineOutlierAction
);

}

void MainWindow::populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions)
{
	foreach(QAction *action, actions) {
		if (!action) {
			menu->addSeparator();
			toolBar->addSeparator();
		}
		else {
			menu->addAction(action);
			toolBar->addAction(action);
		}
	}
}

void MainWindow::createActions()
{
	// view
	viewShowMapAction = new QAction(tr("Show Map"), this);
	viewShowMapAction->setIcon(QIcon(":/images/Map.png"));
	viewShowMapAction->setCheckable(true);

	viewShowGridLinesAction = new QAction(tr("Show Gridlines"), this);
	viewShowGridLinesAction->setIcon(QIcon(":/images/showgrid.png"));
	viewShowGridLinesAction->setCheckable(true);

	viewShowBackgroundAction = new QAction(tr("Show Background"), this);
	viewShowBackgroundAction->setIcon(QIcon(":/images/BG.png"));
	viewShowBackgroundAction->setCheckable(true);

	viewShowCBAction = new QAction(tr("Show Contour Boxplot"), this);
	viewShowCBAction->setIcon(QIcon(":/images/cbp.png"));
	viewShowCBAction->setCheckable(true);

	viewShowVPAction = new QAction(tr("Show Variability Plot"), this);
	viewShowVPAction->setIcon(QIcon(":/images/cvp.png"));
	viewShowVPAction->setCheckable(true);

	viewShowCPPAction = new QAction(tr("Show Contour Probability Plot"), this);
	viewShowCPPAction->setIcon(QIcon(":/images/cpp.png"));
	viewShowCPPAction->setCheckable(true);

	viewShowContourLineAction = new QAction(tr("Show Contour of E"), this);
	viewShowContourLineAction->setIcon(QIcon(":/images/S.png"));
	viewShowContourLineAction->setCheckable(true);

	viewShowContourLineSDFAction = new QAction(tr("Show Contour of SDF"), this);
	viewShowContourLineSDFAction->setIcon(QIcon(":/images/SDF.png"));
	viewShowContourLineSDFAction->setCheckable(true);


	viewShowContourLineSortedAction = new QAction(tr("Show Sorted Contour of E"), this);
	viewShowContourLineSortedAction->setIcon(QIcon(":/images/S_S.png"));
	viewShowContourLineSortedAction->setCheckable(true);


	viewShowContourLineSortedSDFAction = new QAction(tr("Show Sorted Contour of SDF"), this);
	viewShowContourLineSortedSDFAction->setIcon(QIcon(":/images/SDF_S.png"));
	viewShowContourLineSortedSDFAction->setCheckable(true);

	viewShowContourLineResampledAction = new QAction(tr("Show Resampled Contours"), this);
	viewShowContourLineResampledAction->setIcon(QIcon(":/images/SDF_R.png"));
	viewShowContourLineResampledAction->setCheckable(true);

	viewShowContourLineDomainResampledAction = new QAction(tr("Show Domain Resampled Contours"), this);
	viewShowContourLineDomainResampledAction->setIcon(QIcon(":/images/S_R.png"));
	viewShowContourLineDomainResampledAction->setCheckable(true);

	viewShowContourLineMinAction = new QAction(tr("Show Contour of Min"), this);
	viewShowContourLineMinAction->setIcon(QIcon(":/images/min.png"));
	viewShowContourLineMinAction->setCheckable(true);

	viewShowContourLineMaxAction = new QAction(tr("Show Contour of Max"), this);
	viewShowContourLineMaxAction->setIcon(QIcon(":/images/max.png"));
	viewShowContourLineMaxAction->setCheckable(true);

	viewShowContourLineMeanAction = new QAction(tr("Show Contour of Mean"), this);
	viewShowContourLineMeanAction->setIcon(QIcon(":/images/Mean.png"));
	viewShowContourLineMeanAction->setCheckable(true);

	viewShowContourLineMedianAction = new QAction(tr("Show Contour of Median"), this);
	viewShowContourLineMedianAction->setIcon(QIcon(":/images/Median.png"));
	viewShowContourLineMedianAction->setCheckable(true);

	viewShowContourLineOutlierAction = new QAction(tr("Show Contour of Outlier"), this);
	viewShowContourLineOutlierAction->setIcon(QIcon(":/images/O.png"));
	viewShowContourLineOutlierAction->setCheckable(true);


	viewShowClusterAction = new QAction(tr("Show Cluster"), this);
	viewShowClusterAction->setIcon(QIcon(":/images/C.png"));
	viewShowClusterAction->setCheckable(true);

	viewShowRCBandsAction = new QAction(tr("Show EnConVis Bands"), this);
	viewShowRCBandsAction->setIcon(QIcon(":/images/cdp.png"));
	viewShowRCBandsAction->setCheckable(true);

}

void MainWindow::createConnections(){
	// view
	connect(viewShowMapAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowMap(bool)));
	connect(viewShowGridLinesAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowGrid(bool)));
	connect(viewShowBackgroundAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowBackground(bool)));

	connect(viewShowCBAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowCB(bool)));
	connect(viewShowVPAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowVP(bool)));
	connect(viewShowCPPAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowCPP(bool)));

	connect(viewShowContourLineAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLine(bool)));
	connect(viewShowContourLineSortedAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineSorted(bool)));
	connect(viewShowContourLineSortedSDFAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineSortedSDF(bool)));
	connect(viewShowContourLineResampledAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineResampled(bool)));
	connect(viewShowContourLineDomainResampledAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineDomainResampled(bool)));
	connect(viewShowContourLineSDFAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineSDF(bool)));
	connect(viewShowContourLineMinAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineMin(bool)));
	connect(viewShowContourLineMaxAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineMax(bool)));
	connect(viewShowContourLineMeanAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineMean(bool)));
	connect(viewShowContourLineMedianAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineMedian(bool)));
	connect(viewShowContourLineOutlierAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowContourLineOutlier(bool)));
	connect(viewShowClusterAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowCluster(bool)));
	connect(viewShowRCBandsAction, SIGNAL(toggled(bool)), _view3D, SLOT(viewShowRCBands(bool)));

	connect(_pDisplayCtrlWidget, SIGNAL(bgFunctionChanged(int)), _view3D, SLOT(updateBgFunction(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(smoothChanged(int)), _view3D, SLOT(updateVarSmooth(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(transparencyChanged(int)), _view3D, SLOT(updateTransparency(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(focusedClusterChanged(int)), _view3D, SLOT(updateFocusedCluster(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(EOFChanged(int)), _view3D, SLOT(updateEOF(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(MemberChanged(int)), _view3D, SLOT(updateMember(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(EnsClusterChanged(int)), _view3D, SLOT(updateEnsCluster(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(EnsClusterLenChanged(int)), _view3D, SLOT(updateEnsClusterLen(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevelChanged(int)), _view3D, SLOT(updateContourLevel(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(TimeStepChanged(int)), _pModel, SLOT(updateTimeStep(int)));
	connect(_pModel, SIGNAL(UpdateView()), _view3D, SLOT(onUpdateView()));


	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue0(bool)), _view3D, SLOT(onIsoValue0(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue1(bool)), _view3D, SLOT(onIsoValue1(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue2(bool)), _view3D, SLOT(onIsoValue2(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue3(bool)), _view3D, SLOT(onIsoValue3(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue4(bool)), _view3D, SLOT(onIsoValue4(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue5(bool)), _view3D, SLOT(onIsoValue5(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue6(bool)), _view3D, SLOT(onIsoValue6(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue7(bool)), _view3D, SLOT(onIsoValue7(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue8(bool)), _view3D, SLOT(onIsoValue8(bool)));
	connect(_pDisplayCtrlWidget, SIGNAL(SelectIsoValue9(bool)), _view3D, SLOT(onIsoValue9(bool)));


	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel0Changed(int)), _pModel, SLOT(updateContourLevel0(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel1Changed(int)), _pModel, SLOT(updateContourLevel1(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel2Changed(int)), _pModel, SLOT(updateContourLevel2(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel3Changed(int)), _pModel, SLOT(updateContourLevel3(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel4Changed(int)), _pModel, SLOT(updateContourLevel4(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel5Changed(int)), _pModel, SLOT(updateContourLevel5(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel6Changed(int)), _pModel, SLOT(updateContourLevel6(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel7Changed(int)), _pModel, SLOT(updateContourLevel7(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel8Changed(int)), _pModel, SLOT(updateContourLevel8(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(ContourLevel9Changed(int)), _pModel, SLOT(updateContourLevel9(int)));
	connect(_pDisplayCtrlWidget, SIGNAL(Optimizing()), _pModel, SLOT(onOptimizing()));


}

void MainWindow::onMousePressed(int x, int y){
}
   
void MainWindow::closeEvent(QCloseEvent *event)
{
// 	if (okToClearData()) 
	{
		QSettings settings;
		settings.setValue(ShowMap, viewShowMapAction->isChecked());
		settings.setValue(ShowGridLines, viewShowGridLinesAction->isChecked());
		settings.setValue(ShowBackground, viewShowBackgroundAction->isChecked());

		settings.setValue(ShowCB, viewShowCBAction->isChecked());
		settings.setValue(ShowVP, viewShowVPAction->isChecked());
		settings.setValue(ShowCPP, viewShowCPPAction->isChecked());


		settings.setValue(ShowContourLine, viewShowContourLineAction->isChecked());
		settings.setValue(ShowContourLineMin, viewShowContourLineMinAction->isChecked());
		settings.setValue(ShowContourLineSDF, viewShowContourLineSDFAction->isChecked());
		settings.setValue(ShowContourLineMax, viewShowContourLineMaxAction->isChecked());
		settings.setValue(ShowContourLineSorted, viewShowContourLineSortedAction->isChecked());
		settings.setValue(ShowContourLineSortedSDF, viewShowContourLineSortedSDFAction->isChecked());
		settings.setValue(ShowContourLineResampled, viewShowContourLineResampledAction->isChecked());
		settings.setValue(ShowContourLineDomainResampled, viewShowContourLineDomainResampledAction->isChecked());
		settings.setValue(ShowContourLineMean, viewShowContourLineMeanAction->isChecked());
		settings.setValue(ShowContourLineMedian, viewShowContourLineMedianAction->isChecked());
		settings.setValue(ShowContourLineOutlier, viewShowContourLineOutlierAction->isChecked());
		settings.setValue(ShowCluster, viewShowClusterAction->isChecked());
		settings.setValue(ShowRCBands, viewShowRCBandsAction->isChecked());

//		settings.setValue(ShowBeliefEllipse, _pControlWidget->ui.radioButtonBackgroundCluster->isChecked());

		event->accept();
	}
// 	else
// 		event->ignore();
}

