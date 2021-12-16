#include "Timer.h"

#include <Windows.h>

#include <QDebug>

bool const bShowTime = true;

QList<long> MyTimer::_listTimes;

// time an event;
void MyTimer::Time(QString str) {

	long t1 = GetTickCount();
	if (bShowTime) {
		int len = _listTimes.length();
		if (len > 0)
		{
			qDebug() << str << "\t" << t1 - _listTimes[len - 1];// << "ms";
		}
	}

	_listTimes.append(t1);
}