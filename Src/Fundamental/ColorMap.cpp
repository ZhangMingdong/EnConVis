#include "ColorMap.h"


MYGLColor::MYGLColor() {
	_rgb[0] = _rgb[1] = _rgb[2] = 0;
}

MYGLColor::MYGLColor(GLubyte r, GLubyte g, GLubyte b) {
	_rgb[0] = r;
	_rgb[1] = g;
	_rgb[2] = b;
}

// return base of the value, e.g., 210 return 100, 3200 return 1000, 3 return 1;
int getBase(int nValue) {
	nValue /= 10;
	int nBase = 1;
	while (nValue > 0) {
		nValue /= 10;
		nBase *= 10;
	}
	return nBase;
}

void ColorMap::SetRange(double dbMin, double dbMax) {
	int nBase = getBase(dbMax - dbMin);
	_nMin = (int(dbMin)) / nBase * nBase;
	int nMax = (int(dbMax)) / nBase * nBase;
	if (nMax < dbMax) nMax += nBase;

	int nRange = nMax - _nMin;
	_nStep = nRange /(_nLen-1);					// for mean
	for (int i = 0; i < _nLen; i++)
	{
		//double dbHH= _nMin + i * _nStep;
		_pValues[i] = _nMin + i * _nStep;
	}
}

ColorMap* ColorMap::GetInstance(Enum_Color_Pallet cp) {
	if (!_bInitialized)
	{
		_bInitialized = true;
		for (size_t i = 0; i < CP_Length; i++)
		{
			_pInstance[i] = NULL;
		}
	}
	if (!_pInstance[cp])
	{
		_pInstance[cp] = new ColorMap();
		switch (cp)
		{
		case ColorMap::CP_Perception:
			_pInstance[cp]->_nLen = 9;
			_pInstance[cp]->_nStep = 2;
			_pInstance[cp]->_nMin = 0;

			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];

			for (int i = 0; i < _pInstance[cp]->_nLen; i++)
			{
				_pInstance[cp]->_pValues[i] = i * _pInstance[cp]->_nStep;
			}
			_pInstance[cp]->_pColors[0] = MYGLColor(165, 206, 227);
			_pInstance[cp]->_pColors[1] = MYGLColor(81, 154, 165);
			_pInstance[cp]->_pColors[2] = MYGLColor(86, 177, 70);
			_pInstance[cp]->_pColors[3] = MYGLColor(249, 141, 141);
			_pInstance[cp]->_pColors[4] = MYGLColor(239, 105, 68);
			_pInstance[cp]->_pColors[5] = MYGLColor(255, 135, 16);
			_pInstance[cp]->_pColors[6] = MYGLColor(178, 149, 200);
			_pInstance[cp]->_pColors[7] = MYGLColor(198, 180, 153);
			_pInstance[cp]->_pColors[8] = MYGLColor(176, 88, 40);
			break;

		case ColorMap::CP_0_1:
			_pInstance[cp]->_nLen = 3;
			_pInstance[cp]->_nStep = 1;
			_pInstance[cp]->_nMin = -1;

			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];

			for (int i = 0; i < _pInstance[cp]->_nLen; i++)
			{
				_pInstance[cp]->_pValues[i] = _pInstance[cp]->_nMin + i * _pInstance[cp]->_nStep;
			}
			_pInstance[cp]->_pColors[0] = MYGLColor(0, 0, 255);
			_pInstance[cp]->_pColors[1] = MYGLColor(255, 255, 255);
			_pInstance[cp]->_pColors[2] = MYGLColor(255, 0, 0);
			break;
		case ColorMap::CP_RB:
			_pInstance[cp]->_nLen = 3;
			_pInstance[cp]->_nStep = 10;					// for mean
															// 	_nStep = 7;					// for variance
			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];


			_pInstance[cp]->_pValues[0] = -10;
			_pInstance[cp]->_pValues[1] = 0;
			_pInstance[cp]->_pValues[2] = 10;
			_pInstance[cp]->_pColors[0] = MYGLColor(0, 0, 255);
			_pInstance[cp]->_pColors[1] = MYGLColor(255, 255, 255);
			_pInstance[cp]->_pColors[2] = MYGLColor(255, 0, 0);
		case ColorMap::CP_RainBow:
			_pInstance[cp]->_nLen = 8;
			_pInstance[cp]->_nStep = 2;					// for mean
														// 	_nStep = 7;					// for variance
			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];


			_pInstance[cp]->_pValues[0] = -9;
			_pInstance[cp]->_pValues[1] = -6;
			_pInstance[cp]->_pValues[2] = -3;
			_pInstance[cp]->_pValues[3] = 0;
			_pInstance[cp]->_pValues[4] = 3;
			_pInstance[cp]->_pValues[5] = 6;
			_pInstance[cp]->_pValues[6] = 9;

			_pInstance[cp]->_pColors[0] = MYGLColor(87, 0, 255);
			_pInstance[cp]->_pColors[1] = MYGLColor(0, 0, 255);
			_pInstance[cp]->_pColors[2] = MYGLColor(0, 255, 255);
			_pInstance[cp]->_pColors[3] = MYGLColor(0, 255, 0);
			_pInstance[cp]->_pColors[4] = MYGLColor(255, 255, 0);
			_pInstance[cp]->_pColors[5] = MYGLColor(255, 165, 0);
			_pInstance[cp]->_pColors[6] = MYGLColor(255, 0, 0);
			break;
		case ColorMap::CP_12:
			_pInstance[cp]->_nLen = 12;
			_pInstance[cp]->_nStep = 1;					// for mean
														// 	_nStep = 7;					// for variance
			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];

			for (size_t i = 0; i < _pInstance[cp]->_nLen; i++)
			{
				_pInstance[cp]->_pValues[i] = i;
			}

			_pInstance[cp]->_pColors[0] = MYGLColor(255, 0, 0);
			_pInstance[cp]->_pColors[1] = MYGLColor(0, 255, 0);
			_pInstance[cp]->_pColors[2] = MYGLColor(0, 0, 255);
			_pInstance[cp]->_pColors[3] = MYGLColor(255, 255, 0);
			_pInstance[cp]->_pColors[4] = MYGLColor(0, 255, 255);
			_pInstance[cp]->_pColors[5] = MYGLColor(255, 0, 255);
			_pInstance[cp]->_pColors[6] = MYGLColor(128, 0, 0);
			_pInstance[cp]->_pColors[7] = MYGLColor(0, 128, 0);
			_pInstance[cp]->_pColors[8] = MYGLColor(0, 0, 128);
			_pInstance[cp]->_pColors[9] = MYGLColor(128, 128, 0);
			_pInstance[cp]->_pColors[10] = MYGLColor(0, 128, 128);
			_pInstance[cp]->_pColors[11] = MYGLColor(128, 0, 128);
			break;
		case ColorMap::CP_EOF:
			_pInstance[cp]->_nLen = 7;
			_pInstance[cp]->_nStep = 100;					// for mean
			_pInstance[cp]->_nMin = -300;

			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];


			_pInstance[cp]->_pValues[0] = -300;
			_pInstance[cp]->_pValues[1] = -200;
			_pInstance[cp]->_pValues[2] = -100;
			_pInstance[cp]->_pValues[3] = 0;
			_pInstance[cp]->_pValues[4] = 100;
			_pInstance[cp]->_pValues[5] = 200;
			_pInstance[cp]->_pValues[6] = 300;

			_pInstance[cp]->_pColors[0] = MYGLColor(87, 0, 255);
			_pInstance[cp]->_pColors[1] = MYGLColor(0, 0, 255);
			_pInstance[cp]->_pColors[2] = MYGLColor(0, 255, 255);
			_pInstance[cp]->_pColors[3] = MYGLColor(0, 255, 0);
			_pInstance[cp]->_pColors[4] = MYGLColor(255, 255, 0);
			_pInstance[cp]->_pColors[5] = MYGLColor(255, 165, 0);
			_pInstance[cp]->_pColors[6] = MYGLColor(255, 0, 0);
			break;
		case ColorMap::CP_T2:
			_pInstance[cp]->_nLen = 7;
			_pInstance[cp]->_nStep = 15;					// for mean
			_pInstance[cp]->_nMin = 230;

			_pInstance[cp]->_pValues = new double[_pInstance[cp]->_nLen];
			_pInstance[cp]->_pColors = new MYGLColor[_pInstance[cp]->_nLen];


			_pInstance[cp]->_pValues[0] = 230;
			_pInstance[cp]->_pValues[1] = 245;
			_pInstance[cp]->_pValues[2] = 260;
			_pInstance[cp]->_pValues[3] = 275;
			_pInstance[cp]->_pValues[4] = 290;
			_pInstance[cp]->_pValues[5] = 305;
			_pInstance[cp]->_pValues[6] = 320;

			_pInstance[cp]->_pColors[0] = MYGLColor(87, 0, 255);
			_pInstance[cp]->_pColors[1] = MYGLColor(0, 0, 255);
			_pInstance[cp]->_pColors[2] = MYGLColor(0, 255, 255);
			_pInstance[cp]->_pColors[3] = MYGLColor(0, 255, 0);
			_pInstance[cp]->_pColors[4] = MYGLColor(255, 255, 0);
			_pInstance[cp]->_pColors[5] = MYGLColor(255, 165, 0);
			_pInstance[cp]->_pColors[6] = MYGLColor(255, 0, 0);
			break;
		case ColorMap::CP_Gray:
			_pInstance[cp]->_nLen = 2;
			_pInstance[cp]->_nStep = 1;					// for mean
			_pInstance[cp]->_nMin = 0;
			_pInstance[cp]->_pValues = new double[2];
			_pInstance[cp]->_pColors = new MYGLColor[2];


			_pInstance[cp]->_pValues[0] = 0;
			_pInstance[cp]->_pValues[1] = 1;

			_pInstance[cp]->_pColors[0] = MYGLColor(255,255,255);
			_pInstance[cp]->_pColors[1] = MYGLColor(150,150,150);
			break;
		case ColorMap::CP_Length:

			break;
		default:
			break;
		}

	}
	return _pInstance[cp];
}

bool ColorMap::_bInitialized = false;

ColorMap* ColorMap::_pInstance[CP_Length];

ColorMap::ColorMap()
{

}


ColorMap::~ColorMap()
{
	delete _pColors;
	delete _pValues;
}

MYGLColor ColorMap::GetColor(double fValue) const {
	int i = 1;
	while (fValue > _pValues[i] && i < _nLen) i++;


	if (i == _nLen) return _pColors[_nLen - 1];
	else {
		MYGLColor color0 = _pColors[i - 1];
		MYGLColor color1 = _pColors[i];
		return interpolateColor(color0, color1, fValue - _pValues[i - 1], _pValues[i] - fValue);
	}
}
MYGLColor ColorMap::interpolateColor(MYGLColor color0, MYGLColor color1, double fBias0, double fBias1) const {
	MYGLColor result;
	for (int i = 0; i < 3; i++)
	{
		result._rgb[i] = (color0._rgb[i] * fBias1 + color1._rgb[i] * fBias0) / (fBias0 + fBias1);
	}
	return result;
}

int ColorMap::s_arrQChartColor5D[5][3] = {
	{32,159,223},
	{153,202,83},
	{246,166,37},
	{109,95,213},
	{191,89,62 }
};

int ColorMap::s_arrCategoryColor20[20][3] = {
	// rearranged first 5 for temperature
	{ 148, 103, 189 },{ 197, 176, 213 },
	{ 31, 119, 180 },{ 174, 199, 232 },
	{ 44, 160, 44 },{ 152, 223, 138 },
	{ 255, 127, 14 },{ 255, 187, 120 },
	{ 214, 39, 40 },{ 255, 152, 150 },
	{ 140, 86, 75 },{ 196, 156, 148 },
	{ 227, 119, 194 },{ 247, 182, 210 },
	{ 127, 127, 127 },{ 199, 199, 199 },
	{ 188, 189, 34 },{ 219, 219, 141 },
	{ 23, 190, 207 },{ 158, 218, 229 }
	/*
	// original
	{ 31, 119, 180 },
	{ 174, 199, 232 },
	{ 255, 127, 14 },
	{ 255, 187, 120 },
	{ 44, 160, 44 },
	{ 152, 223, 138 },
	{ 214, 39, 40 },
	{ 255, 152, 150 },
	{ 148, 103, 189 },
	{ 197, 176, 213 },
	{ 140, 86, 75 },
	{ 196, 156, 148 },
	{ 227, 119, 194 },
	{ 247, 182, 210 },
	{ 127, 127, 127 },
	{ 199, 199, 199 },
	{ 188, 189, 34 },
	{ 219, 219, 141 },
	{ 23, 190, 207 },
	{ 158, 218, 229 }
	*/
};

double ColorMap::s_arrThreshold[3] = { 23, 190, 207 };

double ColorMap::s_arrSelfDefinedColor20[20][3] = {
	{ 1, 0, 0 },			// R
	{ 0, 1, 0 },			// G
	{ 0, 0, 1 },			// B

	{ 1, 1, 0 },			// yellow
	{ 0, 1, 1 },			// purple
	{ 1, 0, 1 },			// 

	{ .6, .3, 1 },			// R
	{ 1, .6, .3 },			// R
	{ .3, 1, .6 },			// R

	{ .3, .6, 1 },			// R
	{ 1, .3, .6 },			// R
	{ .6, 1, .3 },			// R

	{ .5, 1, 1 },			// R
	{ 1, .5, 1 },			// R
	{ 1, 1, .5 },			// R

	{ .5, 1, 0 },			// R
	{ 0, .5, 1 },			// R
	{ 1, 0, .5 },			// R

	{ .2, .7, .2 },			// R
	{ .2, .2, .7 },			// R
};

int ColorMap::GetCategory20I(int nColorIndex, int nComponentIndex) {
	return s_arrCategoryColor20[nColorIndex][nComponentIndex];
}
double ColorMap::GetCategory20D(int nColorIndex, int nComponentIndex) {
	return s_arrCategoryColor20[nColorIndex][nComponentIndex] / 255.0;
}

// get 10 colors in category20: 0~255
int ColorMap::GetCategory10I(int nColorIndex, int nComponentIndex, int nBias) {
	return s_arrCategoryColor20[nColorIndex * 2 + nBias][nComponentIndex];
}

// get 10 colors in category20: 0.0~1.0
double ColorMap::GetCategory10D(int nColorIndex, int nComponentIndex, int nBias) {

	// using qchart color
	//return s_arrQChartColor5D[nColorIndex % 5][nComponentIndex] / 255.0;

	return s_arrCategoryColor20[nColorIndex * 2 + nBias][nComponentIndex] / 255.0;
	return s_arrCategoryColor20[nColorIndex + nBias][nComponentIndex] / 255.0;
}


double ColorMap::GetRGB(int nColorIndex, int nComponentIndex) {
	return s_arrSelfDefinedColor20[nColorIndex][nComponentIndex];
}


double ColorMap::GetThresholdColorD(int nComponentIndex)
{
	return s_arrThreshold[nComponentIndex] / 255.0;
}

int ColorMap::GetThresholdColorI(int nComponentIndex)
{
	return s_arrThreshold[nComponentIndex];
}