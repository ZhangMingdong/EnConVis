#pragma once
#include <QObject>

/*
	class for frame of grids
	by Mingdong
	2020/06/16
*/
class GridFrame:public QObject
{
public:
	GridFrame();
	~GridFrame();

protected:
	int _nWidth;
	int _nHeight;
	int _nGrids;								// _nWidth*_nHeight

	int _nWest;
	int _nEast;
	int _nSouth;
	int _nNorth;

	int _nEnsembleLen;						// number of ensemble members
	
public:
	int GetW() { return _nWidth; }
	int GetH() { return _nHeight; }
	int GetWest() { return _nWest; }
	int GetEast() { return _nEast; }
	int GetSouth() { return _nSouth; }
	int GetNorth() { return _nNorth; }
	int GetEnsembleLen() { return _nEnsembleLen; }
};

