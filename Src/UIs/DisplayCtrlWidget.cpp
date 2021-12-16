#include "DisplayCtrlWidget.h"

#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

#include <QDebug>
#include "def.h"


extern const int g_nClusterMax;

DisplayCtrlWidget::DisplayCtrlWidget(QWidget *parent)
	: QWidget(parent)
{
	createWidgets();
	createLayout();
	createConnections();
}

DisplayCtrlWidget::~DisplayCtrlWidget()
{
}

void DisplayCtrlWidget::createWidgets() {

	_pCombBgFunction = new QComboBox;
	_pCombBgFunction->addItem("Mean", 0);
	_pCombBgFunction->addItem("Variance", 1);
	_pCombBgFunction->addItem("Cluster", 2);
	_pCombBgFunction->addItem("VarThreshold", 3);
	_pCombBgFunction->addItem("Smoothed", 4);
	_pCombBgFunction->addItem("DipValue", 5);
	_pCombBgFunction->addItem("DipValueThreshold", 6);
	_pCombBgFunction->addItem("EOF", 7);
	_pCombBgFunction->addItem("Obs", 8);
	_pCombBgFunction->addItem("Error", 9);
	_pCombBgFunction->addItem("SDF", 10);
	_pCombBgFunction->addItem("Line Kernel Density", 11);
	_pCombBgFunction->addItem("Line Kernel Density X", 12);
	_pCombBgFunction->addItem("Line Kernel Density Y", 13);
	_pCombBgFunction->addItem("Line Kernel Density Z", 14);
	_pCombBgFunction->addItem("ContourDensity X", 15);
	_pCombBgFunction->addItem("ContourDensity Y", 16);
	_pCombBgFunction->addItem("ContourDensity Z", 17);
	_pCombBgFunction->addItem("ContourDensity", 18);

	_pSpinBoxSmooth = new QSpinBox;
	_pSpinBoxSmooth->setRange(1, 10);
	_pSpinBoxSmooth->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxSmooth->setValue(1);
	_pSpinBoxSmooth->setSingleStep(1);

	_pSpinBoxTransparency = new QSpinBox;
	_pSpinBoxTransparency->setRange(0, 9);
	_pSpinBoxTransparency->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxTransparency->setValue(0);
	_pSpinBoxTransparency->setSingleStep(1);

	_pSpinBoxFocusedCluster = new QSpinBox;
	_pSpinBoxFocusedCluster->setRange(0, 5);
	_pSpinBoxFocusedCluster->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxFocusedCluster->setValue(0);
	_pSpinBoxFocusedCluster->setSingleStep(1);

	_pSpinBoxFocusedRegion = new QSpinBox;
	_pSpinBoxFocusedRegion->setRange(0, 5);
	_pSpinBoxFocusedRegion->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxFocusedRegion->setValue(0);
	_pSpinBoxFocusedRegion->setSingleStep(1);



	_pSpinBoxMember = new QSpinBox;
	_pSpinBoxMember->setRange(0, GlobalSetting::g_nMembers);
	_pSpinBoxMember->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxMember->setValue(0);
	_pSpinBoxMember->setSingleStep(1);

	_pSpinBoxEnsCluster = new QSpinBox;
	_pSpinBoxEnsCluster->setRange(0, GlobalSetting::g_nClusters);
	_pSpinBoxEnsCluster->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxEnsCluster->setValue(0);
	_pSpinBoxEnsCluster->setSingleStep(1);

	_pSpinBoxEnsClusterLen = new QSpinBox;
	_pSpinBoxEnsClusterLen->setRange(1, g_nClusterMax);
	_pSpinBoxEnsClusterLen->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxEnsClusterLen->setValue(1);
	_pSpinBoxEnsClusterLen->setSingleStep(1);


	_pSpinBoxContourLevel0 = new QSpinBox;
	_pSpinBoxContourLevel0->setRange(0, 5);
	_pSpinBoxContourLevel0->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxContourLevel0->setValue(0);
	_pSpinBoxContourLevel0->setSingleStep(1);


	_pSpinBoxContourLevel1 = new QSpinBox;
	_pSpinBoxContourLevel2 = new QSpinBox;
	_pSpinBoxContourLevel3 = new QSpinBox;
	_pSpinBoxContourLevel4 = new QSpinBox;
	_pSpinBoxContourLevel5 = new QSpinBox;
	_pSpinBoxContourLevel6 = new QSpinBox;
	_pSpinBoxContourLevel7 = new QSpinBox;
	_pSpinBoxContourLevel8 = new QSpinBox;
	_pSpinBoxContourLevel9 = new QSpinBox;


	_pSpinBoxTimeStep = new QSpinBox;
	_pSpinBoxTimeStep->setRange(GlobalSetting::g_nTimeStart, GlobalSetting::g_nTimeEnd);
	_pSpinBoxTimeStep->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_pSpinBoxTimeStep->setValue(GlobalSetting::g_nTimeStart);
	_pSpinBoxTimeStep->setSingleStep(6);

	_pCheckBoxIsoValue0 = new QCheckBox; _pCheckBoxIsoValue0->setChecked(true);
	_pCheckBoxIsoValue1 = new QCheckBox; _pCheckBoxIsoValue1->setChecked(true);
	_pCheckBoxIsoValue2 = new QCheckBox; _pCheckBoxIsoValue2->setChecked(true);
	_pCheckBoxIsoValue3 = new QCheckBox; _pCheckBoxIsoValue3->setChecked(true);
	_pCheckBoxIsoValue4 = new QCheckBox; _pCheckBoxIsoValue4->setChecked(true);
	_pCheckBoxIsoValue5 = new QCheckBox; _pCheckBoxIsoValue5->setChecked(true);
	_pCheckBoxIsoValue6 = new QCheckBox; _pCheckBoxIsoValue6->setChecked(true);
	_pCheckBoxIsoValue7 = new QCheckBox; _pCheckBoxIsoValue7->setChecked(true);
	_pCheckBoxIsoValue8 = new QCheckBox; _pCheckBoxIsoValue8->setChecked(true);
	_pCheckBoxIsoValue9 = new QCheckBox; _pCheckBoxIsoValue9->setChecked(true);
	
	_pBtnOptimization = new QPushButton;
	_pBtnOptimization->setText("Optimizing");

}

void DisplayCtrlWidget::createLayout() {

	QFormLayout *layout = new QFormLayout;
	layout->addRow(tr("Background:"), _pCombBgFunction);
	layout->addRow(tr("Smooth:"), _pSpinBoxSmooth);
	layout->addRow(tr("Transparency:"), _pSpinBoxTransparency);
	layout->addRow(tr("Focused Cluster:"), _pSpinBoxFocusedCluster);
	layout->addRow(tr("Focused Region:"), _pSpinBoxFocusedRegion);
	layout->addRow(tr("Member:"), _pSpinBoxMember);
	layout->addRow(tr("Ens Cluster:"), _pSpinBoxEnsCluster);
	//layout->addRow(tr("Number of Ens Clusters:"), _pSpinBoxEnsClusterLen);
	//layout->addRow(tr("Contour Level:"), _pSpinBoxContourLevel);
	layout->addRow(tr("Time Step:"), _pSpinBoxTimeStep);
	layout->addRow(tr("Iso-value 0:"), _pCheckBoxIsoValue0);
	layout->addRow(tr("Iso-value 1:"), _pCheckBoxIsoValue1);
	layout->addRow(tr("Iso-value 2:"), _pCheckBoxIsoValue2);
	layout->addRow(tr("Iso-value 3:"), _pCheckBoxIsoValue3);
	layout->addRow(tr("Iso-value 4:"), _pCheckBoxIsoValue4);
//	layout->addRow(tr("Iso-value 5:"), _pCheckBoxIsoValue5);
//	layout->addRow(tr("Iso-value 6:"), _pCheckBoxIsoValue6);
//	layout->addRow(tr("Iso-value 7:"), _pCheckBoxIsoValue7);
//	layout->addRow(tr("Iso-value 8:"), _pCheckBoxIsoValue8);
//	layout->addRow(tr("Iso-value 9:"), _pCheckBoxIsoValue9);
	layout->addRow(tr("Iso-value 0:"), _pSpinBoxContourLevel0);
	layout->addRow(tr("Iso-value 1:"), _pSpinBoxContourLevel1);
	layout->addRow(tr("Iso-value 2:"), _pSpinBoxContourLevel2);
	layout->addRow(tr("Iso-value 3:"), _pSpinBoxContourLevel3);
	layout->addRow(tr("Iso-value 4:"), _pSpinBoxContourLevel4);
//	layout->addRow(tr("Iso-value 5:"), _pSpinBoxContourLevel5);
//	layout->addRow(tr("Iso-value 6:"), _pSpinBoxContourLevel6);
//	layout->addRow(tr("Iso-value 7:"), _pSpinBoxContourLevel7);
//	layout->addRow(tr("Iso-value 8:"), _pSpinBoxContourLevel8);
//	layout->addRow(tr("Iso-value 9:"), _pSpinBoxContourLevel9);
//	layout->addRow(tr("Optimization:"), _pBtnOptimization);
	setLayout(layout);
}

void DisplayCtrlWidget::createConnections() {
	connect(_pCombBgFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBgFunction(int)));
	connect(_pSpinBoxSmooth, SIGNAL(valueChanged(int)), this, SLOT(updateSmooth(int)));
	connect(_pSpinBoxTransparency, SIGNAL(valueChanged(int)), this, SLOT(updateTransparency(int)));
	connect(_pSpinBoxFocusedCluster, SIGNAL(valueChanged(int)), this, SLOT(updateFocusedCluster(int)));
	connect(_pSpinBoxMember, SIGNAL(valueChanged(int)), this, SLOT(updateMember(int)));
	connect(_pSpinBoxEnsCluster, SIGNAL(valueChanged(int)), this, SLOT(updateEnsCluster(int)));
	connect(_pSpinBoxEnsClusterLen, SIGNAL(valueChanged(int)), this, SLOT(updateEnsClusterLen(int)));
	connect(_pSpinBoxTimeStep, SIGNAL(valueChanged(int)), this, SLOT(updateTimeStep(int)));
	connect(_pSpinBoxContourLevel0, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel0(int)));
	connect(_pSpinBoxContourLevel1, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel1(int)));
	connect(_pSpinBoxContourLevel2, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel2(int)));
	connect(_pSpinBoxContourLevel3, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel3(int)));
	connect(_pSpinBoxContourLevel4, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel4(int)));
	connect(_pSpinBoxContourLevel5, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel5(int)));
	connect(_pSpinBoxContourLevel6, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel6(int)));
	connect(_pSpinBoxContourLevel7, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel7(int)));
	connect(_pSpinBoxContourLevel8, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel8(int)));
	connect(_pSpinBoxContourLevel9, SIGNAL(valueChanged(int)), this, SLOT(updateContourLevel9(int)));

	connect(_pCheckBoxIsoValue0, SIGNAL(clicked(bool)), this, SLOT(onIsoValue0(bool)));
	connect(_pCheckBoxIsoValue1, SIGNAL(clicked(bool)), this, SLOT(onIsoValue1(bool)));
	connect(_pCheckBoxIsoValue2, SIGNAL(clicked(bool)), this, SLOT(onIsoValue2(bool)));
	connect(_pCheckBoxIsoValue3, SIGNAL(clicked(bool)), this, SLOT(onIsoValue3(bool)));
	connect(_pCheckBoxIsoValue4, SIGNAL(clicked(bool)), this, SLOT(onIsoValue4(bool)));
	connect(_pCheckBoxIsoValue5, SIGNAL(clicked(bool)), this, SLOT(onIsoValue5(bool)));
	connect(_pCheckBoxIsoValue6, SIGNAL(clicked(bool)), this, SLOT(onIsoValue6(bool)));
	connect(_pCheckBoxIsoValue7, SIGNAL(clicked(bool)), this, SLOT(onIsoValue7(bool)));
	connect(_pCheckBoxIsoValue8, SIGNAL(clicked(bool)), this, SLOT(onIsoValue8(bool)));
	connect(_pCheckBoxIsoValue9, SIGNAL(clicked(bool)), this, SLOT(onIsoValue9(bool)));
	connect(_pBtnOptimization, SIGNAL(clicked()), this, SLOT(onOptimizing()));
}

void DisplayCtrlWidget::updateBgFunction(int nBgFunction)
{
	emit bgFunctionChanged(nBgFunction);
}

void DisplayCtrlWidget::updateSmooth(int nSmooth)
{
	emit smoothChanged(nSmooth);
}

void DisplayCtrlWidget::updateTransparency(int nT)
{
	emit transparencyChanged(nT);
}

void DisplayCtrlWidget::updateFocusedCluster(int nFocusedCluster) {
	emit focusedClusterChanged(nFocusedCluster);
}


void DisplayCtrlWidget::updateEOF(int nEOF) {

	emit EOFChanged(nEOF);
}
void DisplayCtrlWidget::updateMember(int nMember) {

	emit MemberChanged(nMember);
}
void DisplayCtrlWidget::updateEnsCluster(int nEnsCluster) {

	emit EnsClusterChanged(nEnsCluster);
}
void DisplayCtrlWidget::updateEnsClusterLen(int nEnsClusterLen) {

	emit EnsClusterLenChanged(nEnsClusterLen);
}

void DisplayCtrlWidget::updateTimeStep(int nTS) {
	qDebug() << "DisplayCtrlWidget::updateTimeStep(int nTS) {";
	emit TimeStepChanged(nTS);
}
void DisplayCtrlWidget::onIsoValue0(bool bState) { emit SelectIsoValue0(bState); };
void DisplayCtrlWidget::onIsoValue1(bool bState) { emit SelectIsoValue1(bState); };
void DisplayCtrlWidget::onIsoValue2(bool bState) { emit SelectIsoValue2(bState); };
void DisplayCtrlWidget::onIsoValue3(bool bState) { emit SelectIsoValue3(bState); };
void DisplayCtrlWidget::onIsoValue4(bool bState) { emit SelectIsoValue4(bState); };
void DisplayCtrlWidget::onIsoValue5(bool bState) { emit SelectIsoValue5(bState); };
void DisplayCtrlWidget::onIsoValue6(bool bState) { emit SelectIsoValue6(bState); };
void DisplayCtrlWidget::onIsoValue7(bool bState) { emit SelectIsoValue7(bState); };
void DisplayCtrlWidget::onIsoValue8(bool bState) { emit SelectIsoValue8(bState); };
void DisplayCtrlWidget::onIsoValue9(bool bState) { emit SelectIsoValue9(bState); };


void DisplayCtrlWidget::updateContourLevel0(int nLevel) { emit ContourLevel0Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel1(int nLevel) { emit ContourLevel1Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel2(int nLevel) { emit ContourLevel2Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel3(int nLevel) { emit ContourLevel3Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel4(int nLevel) { emit ContourLevel4Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel5(int nLevel) { emit ContourLevel5Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel6(int nLevel) { emit ContourLevel6Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel7(int nLevel) { emit ContourLevel7Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel8(int nLevel) { emit ContourLevel8Changed(nLevel); }
void DisplayCtrlWidget::updateContourLevel9(int nLevel) { emit ContourLevel9Changed(nLevel); }
void DisplayCtrlWidget::onOptimizing() {
	//qDebug() << "DisplayCtrlWidget::onOptimizing()";
	emit Optimizing(); }