// ============================================================
// PCA
// Written by Mingdong
// 2017/05/13
// ============================================================
#pragma once

#include "tnt_array2d.h"
#include <MathTypes.hpp>
#include <vector>
namespace PCA {
	//按第一行展开计算|A|
	double getA(const TNT::Array2D<double>& arcs, int n);

	//计算每一行每一列的每个元素所对应的余子式，组成A*
	void  getAStart(const TNT::Array2D<double>& arcs, int n, TNT::Array2D<double>& ans);

	// 得到给定矩阵src的逆矩阵保存到des中。
	bool GetMatrixInverse(const TNT::Array2D<double>& src, int n, TNT::Array2D<double>& des);

	// 自己实现的方法，使用初等变换
	void GetMatrixInverse_2(const TNT::Array2D<double>& src, int n, TNT::Array2D<double>& des);

	template < class T>
	void convert_from_string(T& value, const std::string& s);

	void load_data_from_file(TNT::Array2D<double>& d, char*& file_path);

	void adjust_data(TNT::Array2D<double>& d, TNT::Array1D<double>& means);

	double compute_covariance(const TNT::Array2D<double>& d, int i, int j);

	void compute_covariance_matrix(const TNT::Array2D<double> & d, TNT::Array2D<double> & covar_matrix);

	// Calculate the eigenvectors and eigenvalues of the covariance
	// matrix
	void eigen(const TNT::Array2D<double> & covar_matrix, TNT::Array2D<double>& eigenvector, TNT::Array2D<double>& eigenvalue);


	void transpose(const TNT::Array2D<double>& src, TNT::Array2D<double>& target);

	// z = x * y
	void multiply(const TNT::Array2D<double>& x, const TNT::Array2D<double>& y, TNT::Array2D<double>& z);
}
class MyPCA
{
public:
	MyPCA();
	~MyPCA();
public:


	/*
		do the pca
		n:			number of vectors
		mI:			length of the input vectors
		mO:			length of the output vectors
		arrInput:	input data: n*m
		arrOutput:	output data: n*(n-1)
		bNewData:	whether recalculate for new data
	*/
	void DoPCA(const double* arrInput, double* arrOutput, int n, int mI, int mO, bool bNewData = false);

	/*
		do the EOF
		n:			number of vectors
		mI:			length of the input vectors
		mO:			length of the output vectors
		arrInput:	input data: n*mI
		arrOutput:	output modes: n*mO
		arrFactors:	output factors: n*mO
	*/
	void DoEOF(const double* arrInput, double* arrModes,double* arrFactors, int n, int mI, int mO);

	/*
		recover nRow of vectors
		nRow: how many data item
		nCol: length of pca
	*/
	void Recover(double* arrInput, double* arrOutput,int nRow,int nCol);
	/*
		recover 1 data item
		nCol: length of pca
	*/
	void Recover(double* arrInput, double* arrOutput, int nCol);

	// test pca and recover
	void TestPCARecover(double* arrInput, int n, int mI, int mO, bool bNewData = false);
private:
	int _nRow;
	int _nCol;
	double* _arrDataMean;					// array of data means
	TNT::Array2D<double>* _pR;				// function to transform back
	TNT::Array2D<double>* _pCovar_matrix;	// covariance matrix
	TNT::Array2D<double>* _pEigenvector;	// eigen vector
	TNT::Array2D<double>* _pEigenvalue;		// eigen value
	TNT::Array2D<double>* _pD;				// input data
private:
	// read the four matrices
	void readMatrices();
	// calculate the four matrices
	void calculateMatrices();
	// write the four matrices
	void writeMatrices();
public:
	/*
		points:			return the generated points
		arrGridLabel:	label of the grid points
		nWidth,nHeight:	the width and height of the field
		dbDis:			the threshold
	*/
	static void generateEllipse(std::vector<DPoint3>& points, int* arrGridLabel, int label, int nWidth, int nHeight,double dbDis);
};

