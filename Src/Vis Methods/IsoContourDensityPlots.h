#pragma once
#include "GeneralPlots.h"


/*
	this class used to show iso-contour density
	by Mingdong
	2019/06/11
*/
class IsoContourDensityPlots :
	public GeneralPlots
{
protected:
	virtual void doStatistics();
	virtual void generateBands();
	virtual void generateContours();
	virtual void draw();
	virtual void init();
private:
	GLuint _uiTexID[2];
	GLubyte* _dataTexture = NULL;				// data of the texture

	int _nDetailScale = 10;								// scale of detail texture
	double* _pICD=NULL;					// data of iso-contour density, using kde
private:
	void calculateICD();
public:
	IsoContourDensityPlots();
	virtual ~IsoContourDensityPlots();
	void GenerateTexture();
};
