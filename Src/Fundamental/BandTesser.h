#pragma once
#include <QGLWidget>
#include <gl/GLU.h>

#include "UnCertaintyArea.h"

/*
	class used for bands tessellation
	by Mingdong
	2019/06/10
	This class initialized and released in EnsembleLayer, but the object I want to put in each featureSet
	2019/06/11
*/

class BandTesser {

	// 网格化对象指针
	static GLUtesselator *_tobj;
	GLuint _gllist;                           // display index

	void tessSegmentation(GLuint gllist, QList<UnCertaintyArea*> areas);
	void tessSegmentation_uncertainOnly(GLuint gllist, QList<UnCertaintyArea*> areas);
public:
	BandTesser();
	~BandTesser();

public:
	// tess bands
	void Tess(QList<UnCertaintyArea*> areas);
	// draw bands
	void Draw();

	// initialization
	static void BuildTess();

	// release
	static void Release();
};

































