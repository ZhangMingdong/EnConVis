#pragma once

#include <QWidget>
class QComboBox;
class QSpinBox;
class QCheckBox;
class QPushButton;

class DisplayCtrlWidget : public QWidget
{
	Q_OBJECT

public:
	DisplayCtrlWidget(QWidget *parent=NULL);
	~DisplayCtrlWidget();
private:
	// clustering method
	QComboBox *_pCombBgFunction;
	QSpinBox *_pSpinBoxSmooth;
	QSpinBox *_pSpinBoxTransparency;
	QSpinBox *_pSpinBoxFocusedCluster;
	QSpinBox *_pSpinBoxFocusedRegion;
	QSpinBox *_pSpinBoxMember;
	QSpinBox *_pSpinBoxEnsCluster;
	QSpinBox *_pSpinBoxEnsClusterLen;

	QCheckBox *_pCheckBoxIsoValue0;
	QCheckBox *_pCheckBoxIsoValue1;
	QCheckBox *_pCheckBoxIsoValue2;
	QCheckBox *_pCheckBoxIsoValue3;
	QCheckBox *_pCheckBoxIsoValue4;
	QCheckBox *_pCheckBoxIsoValue5;
	QCheckBox *_pCheckBoxIsoValue6;
	QCheckBox *_pCheckBoxIsoValue7;
	QCheckBox *_pCheckBoxIsoValue8;
	QCheckBox *_pCheckBoxIsoValue9;

	/*
		displayed level of contour
		0-1;
		1-3;
		2-7;
		3-15;
		4-31;
		5-50;
	*/
	QSpinBox *_pSpinBoxContourLevel0;
	QSpinBox *_pSpinBoxContourLevel1;
	QSpinBox *_pSpinBoxContourLevel2;
	QSpinBox *_pSpinBoxContourLevel3;
	QSpinBox *_pSpinBoxContourLevel4;
	QSpinBox *_pSpinBoxContourLevel5;
	QSpinBox *_pSpinBoxContourLevel6;
	QSpinBox *_pSpinBoxContourLevel7;
	QSpinBox *_pSpinBoxContourLevel8;
	QSpinBox *_pSpinBoxContourLevel9;

	/*
		time step
		0-360 by 6
	*/
	QSpinBox *_pSpinBoxTimeStep;

	QPushButton *_pBtnOptimization;			// detail level optimization

private:
	void createWidgets();
	void createLayout();
	void createConnections();

private slots:
	void updateBgFunction(int nFunction);
	void updateSmooth(int nSmooth);
	void updateTransparency(int nT);
	void updateFocusedCluster(int nFocusedCluster);
	void updateEOF(int nEOF);
	void updateMember(int nMember);
	void updateEnsCluster(int nEnsCluster);
	void updateEnsClusterLen(int nEnsCluster);
	void updateTimeStep(int nTS);
	void onIsoValue0(bool bState);
	void onIsoValue1(bool bState);
	void onIsoValue2(bool bState);
	void onIsoValue3(bool bState);
	void onIsoValue4(bool bState);
	void onIsoValue5(bool bState);
	void onIsoValue6(bool bState);
	void onIsoValue7(bool bState);
	void onIsoValue8(bool bState);
	void onIsoValue9(bool bState);
	void updateContourLevel0(int nLevel);
	void updateContourLevel1(int nLevel);
	void updateContourLevel2(int nLevel);
	void updateContourLevel3(int nLevel);
	void updateContourLevel4(int nLevel);
	void updateContourLevel5(int nLevel);
	void updateContourLevel6(int nLevel);
	void updateContourLevel7(int nLevel);
	void updateContourLevel8(int nLevel);
	void updateContourLevel9(int nLevel);
	void onOptimizing();
signals:
	void bgFunctionChanged(int nFunction);
	void smoothChanged(int nSmooth);
	void transparencyChanged(int nArea);
	void focusedClusterChanged(int nFocusedCluster);
	void EOFChanged(int nEOF);
	void MemberChanged(int nMember);
	void EnsClusterChanged(int nCluster);
	void EnsClusterLenChanged(int nClusterLen);
	void TimeStepChanged(int nTS);
	void SelectIsoValue0(bool bState);
	void SelectIsoValue1(bool bState);
	void SelectIsoValue2(bool bState);
	void SelectIsoValue3(bool bState);
	void SelectIsoValue4(bool bState);
	void SelectIsoValue5(bool bState);
	void SelectIsoValue6(bool bState);
	void SelectIsoValue7(bool bState);
	void SelectIsoValue8(bool bState);
	void SelectIsoValue9(bool bState);
	void ContourLevel0Changed(int nLevel);
	void ContourLevel1Changed(int nLevel);
	void ContourLevel2Changed(int nLevel);
	void ContourLevel3Changed(int nLevel);
	void ContourLevel4Changed(int nLevel);
	void ContourLevel5Changed(int nLevel);
	void ContourLevel6Changed(int nLevel);
	void ContourLevel7Changed(int nLevel);
	void ContourLevel8Changed(int nLevel);
	void ContourLevel9Changed(int nLevel);
	void Optimizing();
};
