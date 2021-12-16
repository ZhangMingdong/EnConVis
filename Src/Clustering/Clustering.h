// ============================================================
// Interface for clustering
// Written by Mingdong
// 2017/05/12
// ============================================================
#pragma once
namespace CLUSTER {
	class Clustering
	{
	public:
		Clustering();
		virtual ~Clustering();
	protected:
		int _n;						// number of items
		int _m;						// dimensions of items
		int _k;						// number of clusters
		const double *_arrData;		// data items
		int* _arrLabels;			// labels
		int* _arrMergeSource;		// source index of each merge
		int* _arrMergeTarget;		// target index of each merge
	protected:
		/*
		implementation of clustering
		*/
		virtual void doCluster() = 0;
	public:
		/*
			set input data
			n:			number of vectors
			m:			length of vectors
			k:			number of clusters
			arrData:	data buffer
			arrLabels:	return the labels. [length:n][0~r-1]

			return number of clusters
		*/
		int DoCluster(int n, int m, int k, const double *arrData, int* arrLabels, int *arrMergeSource = 0, int* arrMergeTarget = 0);

		// set parameters for DBSCAN
		virtual void SetDBSCANParams(int minPts, double eps) {};

		static void AlignLabels(int* arrLabesl, int nLen);			// align the lables: 1,1,5,5,... -> 0,0,1,1
	};

	// add vector v to vector vSum
	void vectorAdd(double* vSum, const double* v, int nLen);

	// divide a vector
	void vectorDiv(double* v, double dbScale, int nLen);

	// reset a vector
	void vectorReset(double*v, int nLen);

	// calculate distance between the two vectors
	double vectorDis(const double *v1, const double* v2, int nLen);


	// copy vector v1 to v0
	void vectorCopy(double* v0, const double* v1, int nLen);


}