#include <QApplication>

#include "MainWindow.h"


#include <iostream>
#include "INIReader.h"
#include "timer.h"

#include <QDebug>


int GlobalSetting::g_nWidth = 0;
int GlobalSetting::g_nHeight = 0;
int GlobalSetting::g_nWest = 0;
int GlobalSetting::g_nEast = 0;
int GlobalSetting::g_nNorth = 0;
int GlobalSetting::g_nSouth = 0;
int GlobalSetting::g_nMembers = 0;
int GlobalSetting::g_nTimeStart = 0;
int GlobalSetting::g_nTimeEnd = 0;
char GlobalSetting::g_strPath[100];

int GlobalSetting::g_nFeatureLen = 0;
double GlobalSetting::g_arrIso[100];

int GlobalSetting::g_nClusters = 0;

int main(int argc, char *argv[])
{
	MyTimer::Time("main() start");

    QApplication app(argc, argv);

//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-72.ini";
//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-96.ini";
//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-120.ini";
//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-144.ini";
//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-168.ini";
//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-192.ini";
//	const char* strConfigFileName = "../../data/config/t2-mod-ecmwf-20160105-00-216.ini";
//	const char* strConfigFileName = "../../data/config/pre_20171016_0-360-by-6.ini";
//	const char* strConfigFileName = "../../data/config/h500-d20121001-mb360-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20121001-mb0-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20061001-mb0-50-full.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb0-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb90-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb114-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb120-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb126-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb240-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101-mb360-50.ini";				// h500


//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb0-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb48-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb72-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb120-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb120-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb240-50.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20180601-mb360-50.ini";				// h500


//	const char* strConfigFileName = "../../data/config/h500-d20180601.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190601.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20061001-mb0-50-full.ini";				// h500

//	const char* strConfigFileName = "../../data/config/t2-d20160105.ini";				// h500


//	const char* strConfigFileName = "../../data/config/t2-d20160105.ini";
//	const char* strConfigFileName = "../../data/config/t2-d20190101.ini";				// t2

//	const char* strConfigFileName = "../../data/config/pre-d20190601.ini";				// pre


//	const char* strConfigFileName = "../../data/config/h500-d20210715.ini";				// h500





//	const char* strConfigFileName = "../../data/config/h500-d20190101.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101_2.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20190101_3.ini";				// h500
	const char* strConfigFileName = "../../data/config/95-h500-d20190101.ini";				// h500
//	const char* strConfigFileName = "../../data/config/ECCC-h500-d20190101.ini";				// h500
//	const char* strConfigFileName = "../../data/config/CMA-h500-d20190101.ini";				// h500
//	const char* strConfigFileName = "../../data/config/NECP-h500-d20190101.ini";				// h500


//	const char* strConfigFileName = "../../data/config/h500-d20190601.ini";				// h500
//	const char* strConfigFileName = "../../data/config/h500-d20210701.ini";				// h500

//	const char* strConfigFileName = "../../data/config/g-h500-d20190101.ini";				// h500

	// read config file
	INIReader reader(strConfigFileName);

	if (reader.ParseError() < 0) {
		std::cout << "Can't load '"<< strConfigFileName <<"'\n";
		return 1;
	}



	GlobalSetting::g_nWidth = reader.GetInteger("region", "width", 0);
	GlobalSetting::g_nHeight = reader.GetInteger("region", "height", 0);
	GlobalSetting::g_nWest = reader.GetInteger("region", "west", 0);
	GlobalSetting::g_nEast = reader.GetInteger("region", "east", 0);
	GlobalSetting::g_nNorth = reader.GetInteger("region", "north", 0);
	GlobalSetting::g_nSouth = reader.GetInteger("region", "south", 0);
	GlobalSetting::g_nMembers = reader.GetInteger("ensemble", "members", 0);
	GlobalSetting::g_nTimeStart = reader.GetInteger("time", "start", 0);
	GlobalSetting::g_nTimeEnd = reader.GetInteger("time", "end", 0);
	GlobalSetting::g_nClusters = reader.GetInteger("feature", "clusters", 0);
	strcpy(GlobalSetting::g_strPath, reader.Get("file", "path", "").c_str());

	// read isovalues
	GlobalSetting::g_nFeatureLen = reader.GetInteger("feature", "length", 0);
	char arrFeatureName[6];
	for (int i = 0; i < GlobalSetting::g_nFeatureLen; i++)
	{
		sprintf(arrFeatureName, "iso_%d", i);
		GlobalSetting::g_arrIso[i] = reader.GetReal("feature", arrFeatureName, 0);
	}

	app.setApplicationName(app.translate("main", "EnsembleVis"));
	app.setOrganizationName("CG&CAD");
	app.setOrganizationDomain("cg&cad.tsinghua.edu.cn");


	MainWindow mainwindow;
	mainwindow.show();
    return app.exec();
}
