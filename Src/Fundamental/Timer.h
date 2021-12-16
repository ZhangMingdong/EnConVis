#pragma once
#include <QList>
/*
* class used for time calculation
*/
class MyTimer
{
public:
	// list of stored times
	static QList<long> _listTimes;

	// time an event;
	static void Time(QString str);

};
