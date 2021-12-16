#pragma once




// 2017/09/14
const int g_nEnsemblesMax = 200;				// max number of ensemble members
const int g_nClusterMax = 10;					// max number of clusters




/*
* class of the global setting
*/
class GlobalSetting
{
public:	
	static int g_nWidth;				// grid width
	static int g_nHeight;				// grid height
	static int g_nWest;					// west of the data
	static int g_nEast;					// east of the data
	static int g_nNorth;				// north of the data
	static int g_nSouth;				// south of the data
	static int g_nTimeStart;			// start of time steps
	static int g_nTimeEnd;				// end of time steps
	static char g_strPath[100];			// data file path
	static int g_nMembers;				// number of ensemble members
	static int g_nFeatureLen;			// length of features
	static double g_arrIso[100];		// array of isovalues
	static int g_nClusters;				// number of clusters
};

