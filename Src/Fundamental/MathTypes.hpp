// ==================================================
//  name:		MathTypes   
//	date:		09/09/2009
//	demo:	
//	creator:	Mingdong
// ==================================================
#pragma once
//--------------------------------------------------------------------------------
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
//--------------------------------------------------------------------------------
#ifndef PId
 #define PId	3.14159265358979323846264338
 #define PIf 	3.14159265358979323846264338f
 #define PI2d 	6.28318530717958647692528676
 #define PI2f 	6.28318530717958647692528676f
 #define PID2d	1.57079632679489661923132169
 #define PID2f	1.57079632679489661923132169f
 #define PID3d	1.04719755119659774615421446
 #define Ed		2.71828182846
 #define SQRTP2d	2.506628274631000502415765284811
#endif
//--------------------------------------------------------------------------------
/*
	Data structure of 3d points
	2011/12/14
*/
struct DPoint3{
	DPoint3() { x = y = z = 0.0f; }
	DPoint3(double fx, double fy, double fz) { x=fx; y=fy; z=fz; }
	DPoint3 &operator=(const DPoint3 &v) { x = v.x; y = v.y; z = v.z; return *this; }
	double x, y, z;

	int _ulPos=0;
	DPoint3 operator-(const DPoint3& p) {
		return DPoint3(x - p.x, y - p.y, z - p.z);
	}

	double operator*(const DPoint3& p) {
		return x*p.x + y*p.y + z*p.z;
	}
	DPoint3 operator*(double dbScale) {
		return DPoint3(x*dbScale, y*dbScale, z*dbScale);
	}

	bool operator==(const DPoint3& p) {
		return abs(p.x - x) < 0.00001 && abs(p.y - y) < 0.00001 && abs(p.z - z) < 0.00001;
	}
	DPoint3 operator+(const DPoint3& p) {
		return DPoint3(x + p.x, y + p.y, z + p.z);
	}
	double norm() {
		return sqrt(x*x + y * y + z * z);
	}
};//--------------------------------------------------------------------------------
struct DPoint2 {
	DPoint2() { x = y = 0.0f; }
	DPoint2(double fx, double fy) { x = fx; y = fy;  }
	DPoint2 &operator=(const DPoint2 &v) { x = v.x; y = v.y;  return *this; }
	double x, y;

	DPoint2 operator-(const DPoint2& p) {
		return DPoint2(x - p.x, y - p.y);
	}
	DPoint2 operator+(const DPoint2& p) {
		return DPoint2(x + p.x, y + p.y);
	}
	DPoint2 operator*(double dbScale) {
		return DPoint2(x*dbScale,y*dbScale);
	}

	double operator*(const DPoint2& p) {
		return x*p.x + y*p.y;
	}

	double cross(const DPoint2& p) {
		return x * p.y - y * p.x;
	}
	double norm() {
		return sqrt(x*x + y * y);
	}

	// calculate distance to another point p
	double distance(const DPoint2& p) {
		double dx = p.x - x;
		double dy = p.y - y;
		return sqrt(dx * dx + dy * dy);
	}
};//--------------------------------------------------------------------------------

/*
	二维点的数据结构
	DPoint3用来表示世界坐标系中的点，而IPoint2用来表示屏幕坐标系中的点，
	因此允许两点相加，这在世界坐标系中是没有实际意义的，但在屏幕坐标系中
	可以用来描述一些显示要素
	20111214
*/
struct IPoint2 {
	IPoint2() {x = 0; y = 0;}
	IPoint2(int ix, int iy) { x=ix; y=iy; }
	IPoint2 &operator=(const IPoint2 &v) { x = v.x; y = v.y; return *this; }
	int x, y;

	IPoint2 operator +(const IPoint2 &v) const { return IPoint2(x+v.x, y+v.y); }
};//--------------------------------------------------------------------------------
/*
	颜色定义
	20111214
*/
struct RGBAf{
	RGBAf() {}
	RGBAf(float _r, float _g, float _b) { r = _r; g = _g; b = _b; a = 1.0f; }
	RGBAf(float _r, float _g, float _b, float _a) { r = _r; g = _g; b = _b; a = _a; }
	// assignment
	RGBAf &operator=(const RGBAf &v) {
		r = v.r;
		g = v.g;
		b = v.b; 
		a=v.a; 
		return *this; 
	}
	float r, g, b, a;
};
/*
	暂不细考
	20111214
*/
struct FBox3
{
public:
	FBox3() {}
	FBox3(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		_x1 = x1;
		_x2 = x2;
		_y1 = y1;
		_y2 = y2;
	}

	void Set(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		_x1 = x1;
		_x2 = x2;
		_y1 = y1;
		_y2 = y2;
	}


	void GrowToContainBox(const FBox3 &box)
	{
		if (box._x1 < _x1) _x1 = box._x1;
		if (box._y1 < _y1) _y1 = box._y1;
		if (box._x2 > _x2) _x2 = box._x2;
		if (box._y2 > _y2) _y2 = box._y2;
	}
	void GrowToContainPoint(const DPoint3 &point)
	{
		if (point.x < _x1) _x1 = point.x;
		if (point.y < _y1) _y1 = point.y;
		if (point.x > _x2) _x2 = point.x;
		if (point.y > _y2) _y2 = point.y;
	}

//	FPoint3	min, max;
	float _x1;
	float _x2;
	float _y1;
	float _y2;
	float _z1;
	float _z2;
	float MidX(){return (_x1+_x2)/2;};
	float MidY(){return (_y1+_y2)/2;};
	void operator *=(float f)
	{
		_x1 *= f;
		_x2 *= f;
		_y1 *= f;
		_y2 *= f;
	}
	void operator /=(float f)
	{
		_x1 /= f;
		_x2 /= f;
		_y1 /= f;
		_y2 /= f;
	}
};//--------------------------------------------------------------------------------


// 距离和索引，用于返回寻找最近点对的结果
struct DistanceAndIndices
{
	double _dbDis;
	int _nIndex1;
	int _nIndex2;
};

/*
	point with label
*/
class LabeledPoint
{
public:
	LabeledPoint() {};
	LabeledPoint(double x, double y, int nLabel) :_nLabel(nLabel) {
		_arrCoord[0] = x;
		_arrCoord[1] = y;
	}
	double _arrCoord[2];
	int _nLabel;
};

class ColoredPoint
{
public:
	ColoredPoint() {};
	ColoredPoint(double x, double y, double r,double g, double b) {
		_arrCoord[0] = x;
		_arrCoord[1] = y;
		_arrColor[0] = r;
		_arrColor[1] = g;
		_arrColor[2] = b;
	}
	double _arrCoord[2];
	double _arrColor[3];
};



struct MathMethod {
	/*
		check whether a point located inside a triangle
		Determine whether point P in triangle ABC	
		from "http://www.cnblogs.com/graphics/archive/2010/08/05/1793393.html", the 3rd method
		2017/08/15
	*/
	static bool PointinTriangle(DPoint3 A, DPoint3 B, DPoint3 C, DPoint3 P)
	{
		DPoint3 v0 = C - A;
		DPoint3 v1 = B - A;
		DPoint3 v2 = P - A;

		float dot00 = v0*v0;
		float dot01 = v0*v1;
		float dot02 = v0*v2;
		float dot11 = v1*v1;
		float dot12 = v1*v2;

		float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

		float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
		if (u < 0 || u > 1) // if u out of range, return directly
		{
			return false;
		}

		float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
		if (v < 0 || v > 1) // if v out of range, return directly
		{
			return false;
		}

		return u + v <= 1;
	}

	static int comPareByX(DPoint2& pt1, DPoint2& pt2) {
		return pt1.x < pt2.x;
	}

	// get the convex hull of the points
	static std::vector<DPoint2> GetConvexHull(std::vector<DPoint2> pts) {
		std::vector<DPoint2> vecHull;
		int nLen = pts.size();
		if (nLen < 3) return vecHull;

		// 1.get the hull (may be not convex)
		std::sort(pts.begin(), pts.end(), comPareByX);
		std::vector<double> vecAngles;
		DPoint2 pt0 = pts[0];
		pts.erase(pts.begin());
		nLen--;

		vecHull.push_back(pt0);
		for (size_t i = 0; i < nLen; i++)
		{
			DPoint2 pt = pts[i];
			vecAngles.push_back(atan((pt.y - pt0.y) / (pt.x - pt0.x)));
		}

		while (nLen > 1) {
			double dbMin = 1000;
			int nMinIndex = -1;
			for (size_t i = 0; i < nLen; i++)
			{
				if (vecAngles[i] < dbMin) {
					nMinIndex = i;
					dbMin = vecAngles[i];
				}
			}
			vecHull.push_back(pts[nMinIndex]);
			pts.erase(pts.begin() + nMinIndex);
			vecAngles.erase(vecAngles.begin() + nMinIndex);
			nLen--;
		}
		vecHull.push_back(pts[0]);

		// 2.remove the concave points
		nLen = vecHull.size();
		while (true)
		{
			bool bBreak = true;
			for (size_t i = 1; i < nLen; i++)
			{
				DPoint2 pt0 = vecHull[i - 1];
				DPoint2 pt1 = vecHull[i];
				DPoint2 pt2 = vecHull[(i + 1) % nLen];
				DPoint2 pt01 = pt1 - pt0;
				DPoint2 pt12 = pt2 - pt1;
				if (pt01.cross(pt12) < 0) {
					bBreak = false;
					vecHull.erase(vecHull.begin() + i);
					nLen--;
					break;
				}
			}
			if (bBreak) break;
		}
		return vecHull;
	}

	/*
	* trace root of an index in the array
	* Mingdong, 2021/10/25
	* used in contour tree
	*/
	static int TraceRoot(int* arr, int nIndex) {
		if (arr[nIndex] == nIndex) return nIndex;
		else {
			int nRootIndex = TraceRoot(arr, arr[nIndex]);
			arr[nIndex] = nRootIndex;
			return nRootIndex;
		}
	}


	/*
	* rearrange the labels in the array, use the minimum n labels, return number of ids
	* Mingdong, 2021/10/25
	* used in Contour tree
	*/
	static int RearrangeLabels(int* arr, int nLen) {
		int nCurrentUnusedID = 0;
		for (int i = 0; i < nLen; i++)
		{
			if (arr[i] == nCurrentUnusedID) nCurrentUnusedID++;
			else if (arr[i] > nCurrentUnusedID) {
				int nId = arr[i];
				for (int j = i; j < nLen; j++)
				{
					if (arr[j] == nId) arr[j] = nCurrentUnusedID;
					else if (arr[j] == nCurrentUnusedID) arr[j] = nId;
				}
				nCurrentUnusedID++;
			}
		}
		return nCurrentUnusedID;
	}


};