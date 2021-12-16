#include "TimeStep.h"
#include "DataField.h"

TimeStep::TimeStep() {

}
TimeStep::~TimeStep() {

	if (_pData)
	{
		delete _pData;
	}
	for each (FeatureSet* pFeature in _listFeature)
		delete pFeature;
	//	if (_pFeature)
	//	{
	//		delete _pFeature;
	//	}
}

void TimeStep::Init(int w, int h, int e) {

	_pData = new DataField(w,h,e);


}