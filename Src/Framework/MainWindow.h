#ifndef CITYSCAPE_H
#define CITYSCAPE_H

#include <QWidget>
#include <QMainWindow>

#include "def.h"

class MyMapWidget;
class MyChartWidget;
class MeteModel;
class ArtificialModel;

class DisplayCtrlWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow();
	~MainWindow();

private:	// action
	QAction *viewShowMapAction;
	QAction *viewShowGridLinesAction;			
	QAction *viewShowBackgroundAction;

	QAction *viewShowCBAction;
	QAction *viewShowVPAction;
	QAction *viewShowCPPAction;



	QAction *viewShowContourLineAction;
	QAction *viewShowContourLineSortedAction;
	QAction *viewShowContourLineSortedSDFAction;
	QAction *viewShowContourLineResampledAction;
	QAction *viewShowContourLineDomainResampledAction;
	QAction *viewShowContourLineSDFAction;
	QAction *viewShowContourLineMinAction;
	QAction *viewShowContourLineMaxAction;
	QAction *viewShowContourLineMeanAction;
	QAction *viewShowContourLineMedianAction;
	QAction *viewShowContourLineOutlierAction;


	QAction *viewShowClusterAction;		


	QAction *viewShowRCBandsAction;		
private:		// widget
	MyMapWidget *_view3D;						// map view
	MyChartWidget *_viewChart;					// chart view
	DisplayCtrlWidget * _pDisplayCtrlWidget;	// control widget


private:		// model
	// used model
	MeteModel* _pModel;
private:
	void createSceneAndView();
	void createActions();
	void createConnections();
	void createDockWidgets();
	void populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions);
	void populateMenusAndToolBars();
public slots:
	void onMousePressed(int x, int y);

protected:
	virtual void closeEvent(QCloseEvent *event);
};

#endif
