/*
	cost line display layer
	Mingdong
	2017/05/05
*/
#pragma once
#include "MeteLayer.h"
class CostLineLayer :
	public MeteLayer
{
public:
	CostLineLayer();
	virtual ~CostLineLayer();
private:
	virtual void draw(DisplayStates states);
	virtual void init();
	GLuint _gllist;                           // display index
private:
	// read cost line
	void readCostline();
	// draw cost line
	void drawCostline();
};

