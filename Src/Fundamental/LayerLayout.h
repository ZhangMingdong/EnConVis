#pragma once
/*
	struct to store the layout of the layer
*/
class LayerLayout
{
public:
	LayerLayout() {};
	~LayerLayout() {};
public:
	double _dbSpace = .01;
	double _dbSpaceII = .08;
	// the map area[(-1.8,-0.9)~(1.8,0.9)]
	double _dbLeft = -1.8;
	double _dbRight = 1.8;
	double _dbBottom = -0.9;
	double _dbTop = 0.9;
	double _dbHeight = _dbTop - _dbBottom;



	// color bar
	double _dbColorBarLeft = _dbRight + .03;
	double _dbColorBarRight = _dbRight + .06;


	// variance chart
	int _nVarHLayers = 100;												// Horizontal layers of the chart
	double _dbVarChartBottom = _dbTop + _dbSpaceII;						// bottom position of the chart, on top of the map
	double _dbVarChartLeft = _dbLeft;									// left position of the chart
	double _dbVarChartRight = _dbRight;
	double _dbVarChartHeight = .5;
	double _dbVarChartWidth = _dbVarChartRight - _dbVarChartLeft;
	double _dbVarChartTop = _dbVarChartBottom + _dbVarChartHeight;
	double _dbVarHLayer = _dbVarChartWidth / _nVarHLayers;



	// pca point chart
	double _dbPCAChartRadius = (_dbRight - _dbLeft) / 8.0;				// .5;
	double _dbPCAChartLeft = _dbLeft;
	double _dbPCAChartBottom = _dbVarChartTop + _dbSpaceII;

	// var bar
	double _dbBarChartLeft = _dbLeft;
	double _dbBarChartBottom = _dbPCAChartBottom + _dbSpace + _dbPCAChartRadius * 2;

	// uc area relation
	double dbChartRadius = _dbTop;
	double _dbSpaceV = .1;												// vertical space
	double _dbUCARChartRight = _dbLeft- _dbSpaceV;
	double _dbUCARChartLeft = _dbUCARChartRight - _dbHeight;
	double _dbUCARChartBottom = _dbBottom;
	double _dbUCARChartTop = _dbTop;
	double _dbUCARChartCenterX = _dbUCARChartLeft + dbChartRadius;
	double _dbUCARChartCenterY = _dbUCARChartBottom + dbChartRadius;

};

