#pragma once
#include<QList>

class DataField;
class FeatureSet;

class TimeStep {
public:
	// 1.raw data
	DataField* _pData = 0;					// the data	
	QList<FeatureSet*> _listFeature;		// the features
	~TimeStep();
	TimeStep();
	void Init(int w, int h, int e);
};