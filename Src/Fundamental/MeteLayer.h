/*
	Meteorology display layer
	Mingdong
	2017/05/05
*/
#pragma once

#include <QGLWidget>
#include <QObject>

class MeteModel;
class LayerLayout;

// callback interface
class ILayerCB{
public:
	virtual void DrawText(char* pText, double fX, double fY) = 0;
};

class MeteLayer
{
public:
	static ILayerCB* _pCB;
public:
	struct DisplayStates 
	{
		bool _bShowBackground = false;
		bool _bShowContourLineMin = false;
		bool _bShowContourLineMax = false;
		bool _bShowContourLineMean = false;
		bool _bShowContourLineMedian = false;
		bool _bShowContourLineOutlier = false;
		bool _bShowUnionE = false;
		bool _bShowGradientE = false;
		bool _bShowContourLine = false;
		bool _bShowContourLineSorted = false;
		bool _bShowContourLineSDF = false;
		bool _bShowContourLineSortedSDF = false;
		bool _bShowContourLineResampled = false;
		bool _bShowContourLineDomainResampled = false;
		bool _bShowBeliefEllipse = false;

		bool _bShowCB = false;
		bool _bShowVP = false;
		bool _bShowCPP = false;

		bool _bShowCluster = false;

		bool _bShowRCBands = false;
	};
public:
	MeteLayer();
	virtual ~MeteLayer();
public:
	// interface for render
	void DrawLayer(DisplayStates states);
	// interface for initialization
	void InitLayer(const LayerLayout* pLayout, double fScaleW, double fScaleH);
	// set model for the layer
	void SetModel(MeteModel* pModel){ _pModel = pModel; };
	// show or hide
	void Show(bool bShow){ _bShow = bShow; }
	// reload texture
	virtual void ReloadTexture() {};
	// set transparency
	virtual void SetTransparency(int nT, int isoIndex = 0) {};
	// set the index of the focused cluster
	virtual void SetFocusedCluster(int nFocusedCluster) {};
	// brushing
	virtual void Brush(int nLeft, int nRight, int nTop, int nBottom) {};

	

private:
	// implementation for render
	virtual void draw(DisplayStates states) = 0;
	// implementation for initialization
	virtual void init() = 0;


protected:
	// set cluster color
	void SetClusterColor(int nIndex);
	// set region color
	void SetRegionColor(int nIndex);

protected:
	MeteModel* _pModel = NULL;

	double _fScaleW;
	double _fScaleH;
	bool _bShow;
	// layout of the layers
	const LayerLayout* _pLayout = NULL;
protected:	// opengl function
	// draw circle
	void drawCircle(double x, double y, double dbRadius, bool bFill = false);
};

